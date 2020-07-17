#include "beq.h"

bool comp1(const pair<int, int> &a, const pair<int, int> &b){
    return a.first < b.first;
}

bool comp2(const pair<double, int> &a, const pair<double, int> &b){
    return a.first < b.first;
}

double distance(double ax, double ay, double bx, double by){
    return sqrt((ax-bx)*(ax-bx)+(ay-by)*(ay-by));
}

int binsearch(const vector<pair<int,int> > &arr, int value){
    int low = 0, high = arr.size()-1;
    while(low <=high){
        int mid = (low+high)/2;
        if(arr[mid].first == value){
            return mid;
        }
        else if(arr[mid].first < value){
            low = mid+1;
        }
        else{
            high = mid-1;
        }
    }
    return low;
}

int binsearch(const vector<pair<double,int> > &arr, double value){
    int low = 0, high = arr.size()-1;
    while(low <=high){
        int mid = (low+high)/2;
        if(arr[mid].first == value){
            return mid;
        }
        else if(arr[mid].first < value){
            low = mid+1;
        }
        else{
            high = mid-1;
        }
    }
    return low;
}


void Beq::insert(const Pub &pub, const vector<Pub> &pubList)
{
    insert(pub, root, pubList);
}

void Beq::insert(const Pub &pub, beqnode &node, const vector<Pub> &pubList){
    int pos,insertpos;
    ++node.num;
    if(node.isleaf){
        if(node.num < emax){ //just insert into leaf
            for(int i=0;i<pub.size;++i){
                int att = pub.pairs[i].att, value = pub.pairs[i].value;
                insertpos = binsearch(node.list[att], value);
                node.list[att].emplace(node.list[att].begin()+insertpos, value, pub.id);
            }
            double dis = distance(pub.lng, pub.lat, node.cx, node.cy);
            insertpos = binsearch(node.listy, dis);
            node.listy.emplace(node.listy.begin()+insertpos, dis, pub.id);
            return;
        }
        //split the leaf to 4
        node.isleaf = false;
        for(int i=0;i<4;++i){
            node.children[i] = new beqnode();
            node.children[i]->rx = node.rx/2;
            node.children[i]->ry = node.ry/2;
        }
        node.children[0]->cx = node.cx - node.children[0]->rx;
        node.children[0]->cy = node.cy - node.children[0]->ry;
        node.children[1]->cx = node.cx + node.children[1]->rx;
        node.children[1]->cy = node.cy - node.children[1]->ry;
        node.children[2]->cx = node.cx - node.children[2]->rx;
        node.children[2]->cy = node.cy + node.children[2]->ry;
        node.children[3]->cx = node.cx + node.children[3]->rx;
        node.children[3]->cy = node.cy + node.children[3]->ry;
        for(int i=0;i<node.listy.size();++i){
            const Pub &curpub = pubList[node.listy[i].second];
            if(curpub.lng<=node.cx && curpub.lat<=node.cy)
                pos=0;
            else if(curpub.lng>node.cx && curpub.lat<=node.cy)
                pos=1;
            else if(curpub.lng<=node.cx && curpub.lat>node.cy)
                pos=2;
            else
                pos=3;
            splitmap[curpub.id] = pos;
            node.children[pos]->listy.emplace_back(distance(curpub.lng, curpub.lat, node.children[pos]->cx, node.children[pos]->cy), curpub.id);
            ++node.children[pos]->num;
        }
        node.listy.clear();
        for(int i=0;i<4;++i)
            sort(node.children[i]->listy.begin(),node.children[i]->listy.end(),comp2);
        for(int i=0;i<atts;++i){
            for(int j=0;j<node.list[i].size();++j){
                int pubid = node.list[i][j].second;
                node.children[splitmap[pubid]]->list[i].push_back(node.list[i][j]);
            }
            node.list[i].clear();
        }
    }
    if(pub.lng<=node.cx && pub.lat<=node.cy)
        pos=0;
    else if(pub.lng>node.cx && pub.lat<=node.cy)
        pos=1;
    else if(pub.lng<=node.cx && pub.lat>node.cy)
        pos=2;
    else
        pos=3;
    insert(pub, *(node.children[pos]), pubList);
}


void Beq::match(const IntervalSub &sub, int &matchPubs, const vector<Pub> &pubList){
    match(sub, root, matchPubs, pubList);
}

void Beq::match(const IntervalSub &sub, beqnode &node, int &matchPubs, const vector<Pub> &pubList){
    if(node.isleaf){
        for(int i=0;i<sub.size;++i){ //2-4
            int att=sub.constraints[i].att;
            if(node.list[att].size()==0){
                return;
            }
        }
        memset(counter, 0, sizeof(counter)); //5
        for(int i=0;i<sub.size;++i){ //6-10
            int att = sub.constraints[i].att, low = sub.constraints[i].lowValue, high = sub.constraints[i].highValue;
            int lowptr = binsearch(node.list[att], low), highptr = binsearch(node.list[att], high);
            while(lowptr >= 0 && node.list[att][lowptr].first >= low)--lowptr;
            while(highptr < node.list[att].size() && node.list[att][highptr].first <= high)++highptr;
            for(int j=lowptr+1;j<highptr;++j)
                ++counter[node.list[att][j].second];
        }
        double subcx = (sub.lowlng+sub.highlng)/2, subcy = (sub.lowlat+sub.highlat)/2, subr = distance(sub.highlng,sub.highlat,sub.lowlng,sub.lowlat)/2;//(sub.highlng-sub.lowlng)/2;
        double y = distance(subcx, subcy, node.cx, node.cy); //11
        double dmin,dmax;
        if(fabs(subcx-node.cx)<=node.rx && fabs(subcy-node.cy)<=node.ry){ //12-16
            dmin = y-subr;
            dmax = y+subr;
        }
        else{
            dmin = y-subr;
            dmax = 100;
        }
        int lowptr = binsearch(node.listy, dmin), highptr = binsearch(node.listy, dmax);
        while(lowptr >= 0 && node.listy[lowptr].first >= dmin)--lowptr;
        while(highptr < node.listy.size() && node.listy[highptr].first <= dmax)++highptr;
        for(int i=lowptr+1;i<highptr;++i){ //17-20
            if(counter[node.listy[i].second] == sub.size){
                const Pub &pub = pubList[node.listy[i].second];
                
                if(pub.lng > sub.lowlng && pub.lng < sub.highlng && pub.lat > sub.lowlat && pub.lat < sub.highlat){
                    ++matchPubs;
                }
            }
        }
        return;
    }
    for(int i=0;i<4;++i){
        beqnode *tmpnode=node.children[i];
        if(sub.lowlng <= tmpnode->cx+tmpnode->rx && sub.highlng >= tmpnode->cx-tmpnode->rx && sub.lowlat <= tmpnode->cy+tmpnode->ry && sub.highlat >= tmpnode->cy-tmpnode->ry){
            match(sub, *tmpnode, matchPubs, pubList);
        }
    }
}



void Beq::clearnode(beqnode &node){
    if(node.isleaf)
        return;
    for(int i=0;i<4;++i){
        clearnode(*(node.children[i]));
        delete node.children[i];
    }
}

void Beq::deletePub(const Pub &pub, const vector<Pub> &pubList)
{
    deletePub(pub, root, pubList);
}

void Beq::deletePub(const Pub &pub, beqnode &node, const vector<Pub> &pubList){
    int pos,lowptr,highptr;
    --node.num;
    if(node.isleaf){
        for(int i=0;i<pub.size;++i){
            int att = pub.pairs[i].att, value = pub.pairs[i].value;
            lowptr = highptr = binsearch(node.list[att], value);
            while(lowptr >= 0 && node.list[att][lowptr].first == value)--lowptr;
            while(highptr < node.list[att].size() && node.list[att][highptr].first == value)++highptr;
            for(int j=lowptr+1;j<highptr;++j){
                if(node.list[att][j].second == pub.id){
                    node.list[att].erase(node.list[att].begin()+j);
                    break;
                }
            }
        }
        double dis = distance(pub.lng, pub.lat, node.cx, node.cy);
        lowptr = highptr = binsearch(node.listy, dis);
        while(lowptr >= 0 && node.listy[lowptr].first >= dis-(1e-6))--lowptr;
        while(highptr < node.listy.size() && node.listy[highptr].first <= dis+(1e-6))++highptr;
        for(int j=lowptr+1;j<highptr;++j){
            if(node.listy[j].second == pub.id){
                node.listy.erase(node.listy.begin()+j);
                break;
            }
        }
        return;
    }
    if(pub.lng<=node.cx && pub.lat<=node.cy)
        pos=0;
    else if(pub.lng>node.cx && pub.lat<=node.cy)
        pos=1;
    else if(pub.lng<=node.cx && pub.lat>node.cy)
        pos=2;
    else
        pos=3;
    deletePub(pub, *(node.children[pos]), pubList);
    

    if(node.num<emax){
        node.isleaf = true;
        double dis;
        int insertpos;
        for(int pos=0;pos<4;++pos){
            for(int i=0;i<node.children[pos]->listy.size();++i){
                const Pub &curpub = pubList[node.children[pos]->listy[i].second];
                dis = distance(curpub.lng, curpub.lat, node.cx, node.cy);
                node.listy.emplace_back(dis, curpub.id);
            }
        }
        sort(node.listy.begin(),node.listy.end(),comp2);
        for(int i=0;i<atts;++i){
            for(int pos=0;pos<4;++pos){
                for(int j=0;j<node.children[pos]->list[i].size();++j){
                    node.list[i].push_back(node.children[pos]->list[i][j]);
                }
            }
            sort(node.list[i].begin(),node.list[i].end(),comp1);
        }
        for(int i=0;i<4;++i){
            delete node.children[i];
        }
    }
}