#include "PobaTree.h"

size_t PobaTree::mem_size()
{
    size_t sum = sizeof(pair<IntBplusTree *, IntBplusTree *>) * data.size();
    for (auto ele : data)
    {
        sum += ele.first->mem_size();
        sum += ele.second->mem_size();
    }
    return sum;
}

void PobaTree::insert(IntervalSub sub)
{
    for (int i = 0; i < sub.size; i++)
    {
        IntervalCnt &cnt = sub.constraints[i];

        data[cnt.att].first->insert(cnt.lowValue, sub.id);
        data[cnt.att].second->insert(cnt.highValue, sub.id);
    }
}

void PobaTree::deleteSub(IntervalSub sub)
{
    for (int i = 0; i < sub.size; i++)
    {
        IntervalCnt &cnt = sub.constraints[i];

        data[cnt.att].first->erase(cnt.lowValue, sub.id);
        data[cnt.att].second->erase(cnt.highValue, sub.id);
    }
}

void PobaTree::match(const Pub &pub,
                     int &matchSubs,
                     const vector<IntervalSub> &subList)
{
    vector<bool> bits(subList.size(), false);

    for (int i = 0; i < pub.size; i++)
    {
        int value = pub.pairs[i].value, att = pub.pairs[i].att;

        // handle low value tree
        IntBplusTree::iterator itLow = data[att].first->upper_bound(value);
        for (IntBplusTree::iterator cur = itLow;
             cur != data[att].first->end(); cur++)
        {
            bits[cur.get_val()] = true;
        }

        // handle high value tree
        IntBplusTree::iterator itHigh = data[att].second->lower_bound(value);
        for (IntBplusTree::iterator cur = data[att].second->begin();
             cur != itHigh; cur++)
        {
            bits[cur.get_val()] = true;
        }
    }

    // std::cout << "matched subs of pub [";
    // for (int i = 0; i < pub.size; i++)
    // {
    //     std::cout << pub.pairs[i].att << "=" << pub.pairs[i].value << ",";
    // }
    // std::cout << "]\n";

    for (int i = 0; i < subList.size(); i++)
        if (!bits[i])
        {
            ++matchSubs;
        }
}

void PobaTree::match_parallel(const Pub &pub,
                      int &matchSubs,
                      const vector<IntervalSub> &subList)
{
    vector<bool> bits(subList.size(), false);
    vector<median_search_struct> outputLow;
    vector<median_search_struct> outputHigh;

    for (int i = 0; i < pub.size; i++)
    {
        int value = pub.pairs[i].value, att = pub.pairs[i].att;
        median_search_struct low, high;

        outputLow.emplace_back(low);
        outputHigh.emplace_back(high);

        IntBplusTree::iterator it;
        it = data[att].first->parallel_upper_bound(value, outputLow[outputLow.size() - 1].relative_nodes);
        outputLow[outputLow.size() - 1].offset = it.idx;
        outputLow[outputLow.size() - 1].working_model = true;

        it = data[att].second->parallel_lower_bound(value, outputHigh[outputHigh.size() - 1].relative_nodes);
        outputHigh[outputHigh.size() - 1].offset = it.idx;
        outputHigh[outputHigh.size() - 1].working_model = false;
    }

    for (median_search_struct &m_struct : outputLow)
    {
        // decide the first node's range
        size_t first_node_begin, first_node_end;
        if (m_struct.relative_nodes.empty())
        {
            continue;
        }
        else if (m_struct.working_model)
        {
            first_node_begin = m_struct.offset;
            first_node_end = m_struct.relative_nodes[0]->vals.size();
        }
        else
        {
            first_node_begin = 0;
            first_node_end = m_struct.offset;
        }

        // handle the fisrt node
        for (size_t cur = first_node_begin; cur < first_node_end; cur++)
        {
            bits.at(m_struct.relative_nodes[0]->vals[cur]) = true;
        }

        // handle rest nodes
        for (size_t i = 1; i < m_struct.relative_nodes.size(); i++)
        {
            size_t node_size = m_struct.relative_nodes[i]->vals.size();
            for (size_t cur = 0; cur < node_size; cur++)
            {
                bits.at(m_struct.relative_nodes[i]->vals[cur]) = true;
            }
        }
    }

    for (median_search_struct &m_struct : outputHigh)
    {
        // decide the first node's range
        size_t first_node_begin, first_node_end;
        if (m_struct.relative_nodes.empty())
        {
            continue;
        }
        else if (m_struct.working_model)
        {
            first_node_begin = m_struct.offset;
            first_node_end = m_struct.relative_nodes[0]->vals.size();
        }
        else
        {
            first_node_begin = 0;
            first_node_end = m_struct.offset;
        }

        // handle the fisrt node
        for (size_t cur = first_node_begin; cur < first_node_end; cur++)
        {
            bits.at(m_struct.relative_nodes[0]->vals[cur]) = true;
        }

        // handle rest nodes
        for (size_t i = 1; i < m_struct.relative_nodes.size(); i++)
        {
            size_t node_size = m_struct.relative_nodes[i]->vals.size();
            for (size_t cur = 0; cur < node_size; cur++)
            {
                bits.at(m_struct.relative_nodes[i]->vals[cur]) = true;
            }
        }
    }

    for (int i = 0; i < subList.size(); i++)
        if (!bits[i])
        {
            ++matchSubs;
        }
}
