#pragma once
#include <vector>
#include <iostream>
#include <iterator>
using namespace std;

namespace BPlusTree
{
  template <class key_type, class val_type>
  class Node
  {
  public:
    Node()
    {
      next_leaf = nullptr;
      prev_leaf = nullptr;
    };

    size_t mem_size()
    {
      size_t sum = sizeof(key_type) * keys.size() + sizeof(keys) +
                   sizeof(val_type) * vals.size() + sizeof(vals) +
                   sizeof(Node<key_type, val_type> *) * nodes.size() + sizeof(nodes);
      if (nodes.size())
      {
        for (auto n : nodes)
        {
          sum += n->mem_size();
        }
      }
      return sum;
    }

    vector<key_type> keys;
    vector<val_type> vals;
    vector<Node<key_type, val_type> *> nodes;
    class Node<key_type, val_type> *next_leaf;
    class Node<key_type, val_type> *prev_leaf;
  };

  template <class key_type, class val_type>
  class Tree
  {
  public:
    class reverse_iterator
    {
    private:
      friend class Tree;

    public:
      Node<key_type, val_type> *node;
      size_t idx;

      key_type get_key() const
      {
        if (node == nullptr)
          throw std::out_of_range("B+Tree: iterator is out of range");
        return node->keys[idx];
      }

      val_type get_val() const
      {
        if (node == nullptr)
          throw std::out_of_range("B+Tree: iterator is out of range");
        return node->vals[idx];
      }

      void set_val(val_type v)
      {
        if (node == nullptr)
          throw std::out_of_range("B+Tree: iterator is out of range");
        node->vals[idx] = v;
      }

      void advance(int distance)
      {

        if (distance < 0)
        {
          while (distance != 0)
          {
            --(*this);
            distance++;
          }
        }
        else
        {
          while (distance != 0)
          {
            ++(*this);
            distance--;
          }
        }
      }

      // it--
      reverse_iterator operator--(int)
      {
        reverse_iterator rit = *this;
        --(*this);
        return rit;
      }

      // --it
      const reverse_iterator &operator--()
      {

        if (node == nullptr)
          throw std::out_of_range("B+Tree: iterator is out of range");
        if (idx + 1 < node->vals.size())
        {
          idx++;
        }
        else
        {
          idx = 0;
          node = node->next_leaf;
        }
        return *this;
      }

      // postfix increment operator (it++). It makes a copy.
      reverse_iterator operator++(int)
      {

        reverse_iterator rit = *this;
        ++(*this);
        return rit;
      }

      // prefix increment operator (++it). it returns a referece to the incremented iterator, avoiding the copy.
      const reverse_iterator &operator++()
      {

        if (node == nullptr)
          throw std::out_of_range("B+Tree: iterator is out of range");
        if (idx == 0)
        {
          node = node->prev_leaf;
          if (node == nullptr)
            idx = 0;
          else
            idx = node->vals.size() - 1;
        }
        else
        {
          idx--;
        }
        return *this;
      }

      bool operator!=(const reverse_iterator &rit) const
      {
        return !(*this == rit);
      }

      bool operator==(const reverse_iterator &rit) const
      {
        return (this->node == rit.node && this->idx == rit.idx);
      }

    }; // end of reverse_iterator

    class iterator
    {
    private:
      friend class Tree;

    public:
      Node<key_type, val_type> *node;
      size_t idx;

      key_type get_key() const
      {
        if (node == nullptr)
          throw std::out_of_range("B+Tree: iterator is out of range");
        return node->keys[idx];
      }

      val_type get_val() const
      {
        if (node == nullptr)
          throw std::out_of_range("B+Tree: iterator is out of range");
        return node->vals[idx];
      }

      void set_val(val_type v)
      {
        if (node == nullptr)
          throw std::out_of_range("B+Tree: iterator is out of range");
        node->vals[idx] = v;
      }

      void advance(int distance)
      {

        if (distance < 0)
        {
          while (distance != 0)
          {
            --(*this);
            distance++;
          }
        }
        else
        {
          while (distance != 0)
          {
            ++(*this);
            distance--;
          }
        }
      }

      // it--
      iterator operator--(int)
      {
        iterator it = *this;
        --(*this);
        return it;
      }

      // --it
      const iterator &operator--()
      {

        if (node == nullptr)
          throw std::out_of_range("B+Tree: iterator is out of range");
        if (idx == 0)
        {
          node = node->prev_leaf;
          idx = node->vals.size() - 1;
        }
        else
        {
          idx--;
        }

        return *this;
      }

      // postfix increment operator (it++). It makes a copy.
      iterator operator++(int)
      {

        iterator it = *this;
        ++(*this);
        return it;
      }

      // prefix increment operator (++it). it returns a referece to the incremented iterator, avoiding the copy.
      const iterator &operator++()
      {

        if (node == nullptr)
          throw std::out_of_range("B+Tree: iterator is out of range");
        if (idx + 1 < node->vals.size())
        {
          idx++;
        }
        else
        {
          idx = 0;
          node = node->next_leaf;
        }
        return *this;
      }

      bool operator!=(const iterator &it) const
      {
        return !(*this == it);
      }

      bool operator==(const iterator &it) const
      {
        return (this->node == it.node && this->idx == it.idx);
      }

    }; // end of iterator

    // B+Tree public functions

    Tree(size_t degree)
    {
      if (degree < 3)
        throw std::runtime_error("B+Tree - max_degree must > 3");
      this->max_degree = degree;
      root = new Node<key_type, val_type>;
      num_elements = 0;
    }

    ~Tree()
    {
      clear();
      delete root;
    }

    void insert(const key_type &key, val_type val)
    {

      size_t i, j, traverse_index;
      key_type median_key;
      vector<size_t> traverse_indices;
      vector<Node<key_type, val_type> *> parents;

      Node<key_type, val_type> *right;
      Node<key_type, val_type> *n = root;
      Node<key_type, val_type> *parent;
      bool records = true;

      // it = find(key);
      // if (it != end()) {
      //   it.set_val(val);
      //   return;
      // }

      /* find the leaf node */
      while (1)
      {

        for (i = 0; i < n->keys.size(); i++)
        {
          if (key < n->keys[i])
            break;
        }
        if (n->nodes.size() != 0)
        {
          traverse_indices.push_back(i);
          parents.push_back(n);
          n = n->nodes[i];
        }
        else
          break;
      }

      /* key exists */
      // for (j = 0; j < n->keys.size(); j++) {
      //   if (n->keys[j] == key) {
      //     n->vals[j] = val;
      //     return;
      //   }
      // }

      num_elements++;
      /* put the val and key in the proper postion */
      n->keys.insert(n->keys.begin() + i, key);
      n->vals.insert(n->vals.begin() + i, val);

      /* split the node until the bucket(key) is not full any more */
      while (n->keys.size() == max_degree)
      {
        median_key = n->keys[max_degree / 2];

        /* no matter weather we split the internal node or root node 
       We need the "right" node. When we split the nodes that contain records, the median was kept. 
       Otherwise, the median was deleted. 

    */
        right = new Node<key_type, val_type>;
        if (records)
          j = max_degree / 2;
        else
          j = max_degree / 2 + 1;

        for (i = j; i < max_degree; i++)
        {
          if (records)
            right->vals.push_back(n->vals[i]);
          right->keys.push_back(n->keys[i]);
        }

        for (i = (n->nodes.size() + 1) / 2; i < n->nodes.size(); i++)
        {
          right->nodes.push_back(n->nodes[i]);
        }

        // when we split the root node, create the new parent node.
        //   The original node became the "left" node.

        if (traverse_indices.size() == 0)
        {

          parent = new Node<key_type, val_type>;

          parent->nodes.push_back(n);
          parent->nodes.push_back(right);
          parent->keys.push_back(median_key);

          /* connect the leaf nodes */

          right->next_leaf = n->next_leaf;
          if (n->next_leaf != nullptr)
            n->next_leaf->prev_leaf = right;
          n->next_leaf = right;
          right->prev_leaf = n;

          root = parent;
          n->keys.resize(max_degree / 2);
          if (records)
            n->vals.resize(max_degree / 2);

          if (n->nodes.size() != 0)
            n->nodes.resize((n->nodes.size() + 1) / 2);
          // cout << "size: " << parent->nodes.size() << " " << n->nodes.size() << " " << right->nodes.size() << endl;
        }
        else
        {

          /* when we split the internal node, the original node keeps the half capacity as the left node.
         Also, the median key was added to it's parent.
       */

          if (records)
            n->vals.resize(max_degree / 2);
          n->keys.resize(max_degree / 2);

          if (n->nodes.size() != 0)
            n->nodes.resize((n->nodes.size() + 1) / 2); // inrernal node

          /* connect the leaf nodes */
          right->next_leaf = n->next_leaf;
          if (n->next_leaf != nullptr)
            n->next_leaf->prev_leaf = right;
          n->next_leaf = right;
          right->prev_leaf = n;

          parent = parents[parents.size() - 1];
          parents.pop_back();

          traverse_index = traverse_indices[traverse_indices.size() - 1];
          traverse_indices.pop_back();

          parent->keys.insert(parent->keys.begin() + traverse_index, median_key);
          parent->nodes.insert(parent->nodes.begin() + traverse_index + 1, right);

          n = parent;
          records = false;
        }
      }

      return;
    }

    size_t mem_size()
    {
      return root->mem_size();
    }

    iterator find(const key_type &key) const
    {

      Node<key_type, val_type> *n = root;
      iterator it;
      size_t i;

      /* find the leaf node first */
      while (n->nodes.size() != 0)
      {
        for (i = 0; i < n->keys.size(); i++)
        {
          if (key < n->keys[i])
            break;
        }

        if (n->nodes.size() != 0)
          n = n->nodes[i];
      }

      /* check to see if we find the key */
      for (i = 0; i < n->keys.size(); i++)
      {
        if (key == n->keys[i])
        {
          it.idx = i;
          it.node = n;
          return it;
        }
      }
      return end();
    }

    void erase(const key_type &key, const val_type &value)
    {
      Node<key_type, val_type> *n = root;
      size_t i;
      int delete_index = -1;
      size_t min_keys = (max_degree - 1) / 2;
      size_t size;
      Node<key_type, val_type> *left, *right;
      Node<key_type, val_type> *parent;
      size_t traverse_index;
      vector<size_t> traverse_indices;
      vector<Node<key_type, val_type> *> parents;
      bool records = true;

      /* remember an internal node along with index, whose key is euqal to param "key" 
        when we delete the leftmost key in the subtree, we will update the internal node's key,
        which has the same value as param "key". The new key will be the new replaced element.
      */
      Node<key_type, val_type> *same_value_node = nullptr;
      int same_value_index = -1;

      /* find the leaf node first */
      while (n->nodes.size() != 0)
      {
        for (i = 0; i < n->keys.size(); i++)
        {
          if (key == n->keys[i])
          {
            same_value_node = n;
            same_value_index = i;
          }
          if (key < n->keys[i])
            break;
        }

        if (n->nodes.size() != 0)
        {
          traverse_indices.push_back(i);
          parents.push_back(n);
          n = n->nodes[i];
        }
      }

      /* find the index */
      for (i = 0; i < n->keys.size(); i++)
      {
        if (key == n->keys[i])
        {
          delete_index = i;
          break;
        }
      }

      /* key is not found in tree */
      if (delete_index == -1)
        return;

      // note: check for the key with target value
      iterator checkIterator;
      checkIterator.node = n;
      checkIterator.idx = delete_index;
      while (checkIterator != end() &&
             checkIterator.get_key() == key &&
             checkIterator.get_val() != value)
      {
        checkIterator++;
      }
      if (checkIterator == end() || checkIterator.get_val() != value)
      {
        return;
      }
      n = checkIterator.node;
      delete_index = checkIterator.idx;

      num_elements--;
      /* delete the record */
      n->keys.erase(n->keys.begin() + delete_index);
      n->vals.erase(n->vals.begin() + delete_index);
      if (n == root)
        return;

      left = n->prev_leaf;
      right = n->next_leaf;

      /* case1: the bucket is big enough after deletion operation */
      if (n->keys.size() >= min_keys)
      {

        /* the leftmost key is deleted */
        if (same_value_node != nullptr)
        {
          same_value_node->keys[same_value_index] = n->keys[0];
        }
        return;
      }

      /* merge or borrow the node from neighbors 
     until the size of current bucket is >= min_keys or get to root node 
  */
      while (n->keys.size() < min_keys && n != root)
      {

        left = n->prev_leaf;
        right = n->next_leaf;
        parent = parents[parents.size() - 1];
        parents.pop_back();
        traverse_index = traverse_indices[traverse_indices.size() - 1];
        traverse_indices.pop_back();

        /* case:2 borrow from left node 
       when it's not the leftmost node in the substree and the left node's size is big enought.
       traverse_index minus 1 is because the index of key is one less than the index of nodes.
    */
        if (left != nullptr && traverse_index != 0)
        {
          size = left->keys.size();

          if (size > min_keys)
          {

            traverse_index--;
            /* if it's leaf nodes, we steal the rightmost key and val in the left node 
           and update the parent's key with its key.
           Otherwise we bring down the parent key to the current node and 
           bring up the rightmost key in the left node.
        */
            if (records)
            {
              n->keys.insert(n->keys.begin(), left->keys[size - 1]);
              parent->keys[traverse_index] = n->keys[0];

              n->vals.insert(n->vals.begin(), left->vals[size - 1]);
              left->vals.pop_back();
            }
            else
            {
              n->keys.insert(n->keys.begin(), parent->keys[traverse_index]);
              parent->keys[traverse_index] = left->keys[size - 1];
              n->nodes.insert(n->nodes.begin(), left->nodes[left->nodes.size() - 1]);
              left->nodes.pop_back();
            }

            left->keys.pop_back();

            return;
          }

          /* case3: borrow from right node */
        }
        else if (right != nullptr && traverse_index != parent->nodes.size() - 1)
        {

          size = right->keys.size();

          if (size > min_keys)
          {
            /* the leftmost key in the subtree could be deleted */
            if (same_value_node != nullptr)
            {
              same_value_node->keys[same_value_index] = n->keys[0];
            }

            /* if it's leaf nodes, we steal the leftmost key and val in the right node 
           and update the parent's key with its key.
           Otherwise we bring down the parent key to the current node and 
           bring up the leftmost key in the right node.
        */

            if (records)
            {
              n->keys.push_back(right->keys[0]);
              // I haven't delete it, so we use index 1.
              parent->keys[traverse_index] = right->keys[1];

              n->vals.push_back(right->vals[0]);
              right->vals.erase(right->vals.begin());
            }
            else
            {

              n->keys.push_back(parent->keys[traverse_index]);
              parent->keys[traverse_index] = right->keys[0];

              n->nodes.push_back(right->nodes[0]);
              right->nodes.erase(right->nodes.begin());
            }

            right->keys.erase(right->keys.begin());

            return;
          }
        }

        /* case:4 merge the node to left node */
        if (left != nullptr && traverse_index != 0)
        {

          /* reset the next and prev leaf */
          left->next_leaf = n->next_leaf;
          if (n->next_leaf != nullptr)
            n->next_leaf->prev_leaf = left;

          /* when it's not leaf nodes, bring down the parent key and merge nodes as well */
          if (!records)
          {
            left->keys.push_back(parent->keys[traverse_index - 1]);
            for (i = 0; i < n->nodes.size(); i++)
            {
              left->nodes.push_back(n->nodes[i]);
            }
          }

          /* merge keys */
          for (i = 0; i < n->keys.size(); i++)
          {
            left->keys.push_back(n->keys[i]);
            if (records)
              left->vals.push_back(n->vals[i]);
          }

          // erase the parent key and node
          parent->keys.erase(parent->keys.begin() + traverse_index - 1);
          parent->nodes.erase(parent->nodes.begin() + traverse_index);

          /* merge into a root node */
          if (parent->keys.size() == 0 && parent == root)
          {

            delete n;
            delete root;
            root = left;
            return;
          }

          delete n;
          n = parent;

          /* case5: merge the right node to n node */
        }
        else if (right != nullptr && traverse_index != parent->nodes.size() - 1)
        {

          /* we may delete the leftmost key in the subtree */
          if (same_value_node != nullptr)
          {
            same_value_node->keys[same_value_index] = n->keys[0];
          }

          n->next_leaf = right->next_leaf;
          if (right->next_leaf != nullptr)
            right->next_leaf->prev_leaf = n;

          /* when it's not leaf nodes, bring down the parent key and merge nodes as well */
          if (!records)
          {
            n->keys.push_back(parent->keys[traverse_index]);
            for (i = 0; i < right->nodes.size(); i++)
            {
              n->nodes.push_back(right->nodes[i]);
            }
          }

          /* get keys and vals */
          for (i = 0; i < right->keys.size(); i++)
          {
            n->keys.push_back(right->keys[i]);
            if (records)
              n->vals.push_back(right->vals[i]);
          }

          /* update parent nodes and keys */
          parent->nodes[traverse_index + 1] = n;
          parent->keys.erase(parent->keys.begin() + traverse_index);
          parent->nodes.erase(parent->nodes.begin() + traverse_index);

          if (parent->keys.size() == 0 && parent == root)
          {
            delete right;
            delete root;
            root = n;
            return;
          }

          delete right;
          n = parent;
        }

        records = false;
        same_value_node = nullptr;
      }
    }

    void erase(const reverse_iterator &rit)
    {
      erase(rit.get_key());
    }

    void erase(const iterator &it)
    {
      erase(it.get_key());
    }

    bool contains(const key_type &k)
    {
      return (find(k) != end());
    }

    size_t size() const { return num_elements; };
    bool empty() const { return (num_elements == 0); };
    void clear()
    {
      recursive_clear_tree(root);
      root = new Node<key_type, val_type>;
      num_elements = 0;
    }

    iterator upper_bound(const key_type &key) const
    {

      iterator it = lower_bound(key);
      if (it == end())
        return it;
      while (it != end() && it.get_key() == key)
        it++;
      return it;
    }

    iterator lower_bound(const key_type &key) const
    {
      Node<key_type, val_type> *n = root;
      iterator it;
      size_t i;

      /* find the leaf node */
      while (n->nodes.size() != 0)
      {
        for (i = 0; i < n->keys.size(); i++)
        {
          if (key < n->keys[i])
            break;
        }

        if (n->nodes.size() != 0)
          n = n->nodes[i];
      }

      /* find the node whose key is >= the given key */
      while (n != nullptr)
      {
        for (i = 0; i < n->keys.size(); i++)
        {
          if (key <= n->keys[i])
          {
            it.idx = i;
            it.node = n;
            return it;
          }
        }
        n = n->next_leaf;
      }

      return end();
    }

    vector<key_type> get_keys() const
    {
      Node<key_type, val_type> *n = root;
      vector<key_type> rv;
      size_t i;

      // go to the leftmost node.
      while (n->nodes.size() != 0)
      {
        n = n->nodes[0];
      }

      do
      {
        for (i = 0; i < n->keys.size(); i++)
        {
          rv.push_back(n->keys[i]);
        }
        n = n->next_leaf;
      } while (n != nullptr);

      return rv;
    }

    vector<val_type> get_vals() const
    {
      Node<key_type, val_type> *n = root;
      vector<val_type> rv;
      size_t i;

      // go to the leftmost node.
      while (n->nodes.size() != 0)
      {
        n = n->nodes[0];
      }

      do
      {
        for (i = 0; i < n->vals.size(); i++)
        {
          rv.push_back(n->vals[i]);
        }
        n = n->next_leaf;
      } while (n != nullptr);

      return rv;
    }

    val_type at(const key_type &key) const
    {
      iterator it = find(key);
      return it.get_val();
    }
    val_type &operator[](const key_type &key)
    {

      static val_type dummy;
      if (find(key) == end())
        insert(key, dummy);

      Node<key_type, val_type> *n = root;
      size_t i;

      /* find the leaf node first */
      while (n->nodes.size() != 0)
      {
        for (i = 0; i < n->keys.size(); i++)
        {
          if (key < n->keys[i])
            break;
        }

        if (n->nodes.size() != 0)
          n = n->nodes[i];
      }

      /* check to see if we find the key */
      for (i = 0; i < n->keys.size(); i++)
      {
        if (key == n->keys[i])
        {
          break;
        }
      }

      if (n->keys.size() == i)
        throw std::runtime_error("B+tree [] internal error");
      return n->vals[i];
    }

    reverse_iterator rbegin() const
    {
      reverse_iterator rit;
      Node<key_type, val_type> *n = root;

      if (num_elements == 0)
      {
        rit.node = nullptr;
        rit.idx = 0;
        return rit;
      }

      /* find the rightmost node */
      while (n->nodes.size() != 0)
      {
        n = n->nodes[n->nodes.size() - 1];
      }

      rit.node = n;
      rit.idx = n->vals.size() - 1;

      return rit;
    }

    reverse_iterator rend() const
    {
      reverse_iterator rit;
      rit.node = nullptr;
      rit.idx = 0;
      return rit;
    }

    iterator begin() const
    {
      iterator it;
      Node<key_type, val_type> *n = root;

      if (num_elements == 0)
      {
        it.node = nullptr;
        it.idx = 0;
        return it;
      }

      /* find the leftmost node */
      while (n->nodes.size() != 0)
      {
        n = n->nodes[0];
      }

      it.node = n;
      it.idx = 0;

      return it;
    }

    iterator end() const
    {
      iterator it;
      it.node = nullptr;
      it.idx = 0;
      return it;
    }

  private:
    size_t num_elements;
    Node<key_type, val_type> *root;
    size_t max_degree;

    void recursive_clear_tree(const Node<key_type, val_type> *n)
    {
      size_t i;
      for (i = 0; i < n->nodes.size(); i++)
      {
        recursive_clear_tree(n->nodes[i]);
      }
      delete n;
    }

    // used in multi-thread
  public:
    Node<key_type, val_type> *begin_leaf_node() const
    {
      Node<key_type, val_type> *n = root;

      if (num_elements == 0)
      {
        return nullptr;
      }
      /* find the leftmost node */
      while (n->nodes.size() != 0)
      {
        n = n->nodes[0];
      }
      return n;
    }

    iterator parallel_upper_bound(const key_type &key,
                                  vector<Node<key_type, val_type> *> &realtive_nodes) const
    {
      iterator it = lower_bound(key);
      if (it == end())
        return it;
      while (it != end() && it.get_key() == key)
        it++;

      // construct relative nodes
      Node<key_type, val_type> *cur_node = it.node;
      while (cur_node)
      {
        realtive_nodes.emplace_back(cur_node);
        cur_node = cur_node->next_leaf;
      }

      return it;
    }

    iterator parallel_lower_bound(const key_type &key,
                                  vector<Node<key_type, val_type> *> &realtive_nodes) const
    {
      Node<key_type, val_type> *n = root;
      Node<key_type, val_type> *pre_n = nullptr;
      iterator it;
      size_t i;

      /* find the leaf node */
      while (n->nodes.size() != 0)
      {
        for (i = 0; i < n->keys.size(); i++)
        {
          if (key < n->keys[i])
            break;
        }

        if (n->nodes.size() != 0)
          n = n->nodes[i];
      }

      Node<key_type, val_type> *cur_leaf_node;

      /* find the node whose key is >= the given key */
      while (n != nullptr)
      {
        for (i = 0; i < n->keys.size(); i++)
        {
          if (key <= n->keys[i])
          {
            it.idx = i;
            it.node = n;

            cur_leaf_node = n;
            while (cur_leaf_node != nullptr)
            {
              // attention it is reverse order
              realtive_nodes.emplace_back(cur_leaf_node);
              cur_leaf_node = cur_leaf_node->prev_leaf;
            }
            return it;
          }
        }
        pre_n = n;
        n = n->next_leaf;
      }

      cur_leaf_node = pre_n;
      while (cur_leaf_node != nullptr)
      {
        // attention it is reverse order
        realtive_nodes.emplace_back(cur_leaf_node);
        cur_leaf_node = cur_leaf_node->prev_leaf;
      }
      return end();
    }

  }; // end of Tree class

}; // namespace BPlusTree
