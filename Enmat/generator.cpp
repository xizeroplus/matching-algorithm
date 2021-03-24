#include "generator.h"
using namespace std;


//----------------------tools---------------------------------
bool CheckExist(const vector<int> &a,int x)
{
    for (int i = 0; i < a.size(); i++)
        if (a[i] == x)
            return true;
    return false;
}

static inline int random(int x)
{
    return (int) (x * (rand() / (RAND_MAX + 1.0)));
}


void intervalGenerator::GenHot(){
    if (hot) return;
    for(int i = 0; i < atts; ++i){
        for(int j = 0; j < HOT_NUMBERS - 1; ++j){
            randnumtmp[j] = rand() / (RAND_MAX + 1.0);
        }
        sort(randnumtmp, randnumtmp + HOT_NUMBERS - 1);
        for(int j = 0; j < HOT_NUMBERS - 1; ++j){
            hotSplit[i][j] = randnumtmp[j];
        }
        hotSplit[i][HOT_NUMBERS - 1] = 2.0; //top poss
        for(int j = 0; j < HOT_NUMBERS; ++j){
            hotStart[i][j] = random((int)(valDom*0.95));
        }
    }
    hot = true;
}

int intervalGenerator::GetHot(int att, int num=-1){
    if(!hot) return 0;
    if(num>=0)
        return random(valDom / 20) + hotStart[att][num];
    double poss = rand() / (RAND_MAX + 1.0);
    int hotptr = -1;
    while (hotSplit[att][++hotptr] < poss);
    int hotcenter = random(valDom / 20) + hotStart[att][hotptr];
    return hotcenter;
}

//----------------------sub---------------------------------

void intervalGenerator::ReadSubList(){
    ifstream fileStream;
    fileStream.open("./data/subList.txt");
    for (int i=0; i<subs; ++i){
        IntervalSub sub;
        fileStream >> sub.id >> sub.size;
        for (int j=0; j<sub.size; ++j){
            IntervalCnt tmp;
            fileStream >> tmp.att >> tmp.lowValue >> tmp.highValue;
            sub.constraints.push_back(tmp);
        }
        subList.push_back(sub);
    }
    fileStream.close();
}

void intervalGenerator::GenSubList()
{
    for (int i = 0; i < subs; i++)
    {
        IntervalSub sub = GenOneSub(i);

        subList.push_back(sub);
    }

    ofstream fileStream;
    fileStream.open("./data/subList.txt");
    for (int i=0; i<subs; ++i)
    {
        IntervalSub &sub = subList[i];
        fileStream << sub.id << "\t" << sub.size << "\t";
        for (int j=0; j<sub.size; ++j)
            fileStream << sub.constraints[j].att << "\t" << sub.constraints[j].lowValue << "\t" << sub.constraints[j].highValue << "\t";
        fileStream << "\n";
    }
    fileStream.close();
}

IntervalSub intervalGenerator::GenOneSub(int id)
{
    IntervalSub sub;
    sub.id = id;
    sub.size = random(cons)+1;
    width = 0.8 * (rand() / (RAND_MAX + 1.0)) + 0.1;
    vector<int> a;
    for (int i = 0; i < sub.size; i++)
    {
        int x = random(atts);
        while (CheckExist(a,x))
            x = random(atts);
        a.push_back(x);
        IntervalCnt tmp;
        tmp.att = x;
        int y = random(int(valDom * (1.0 - width)));
        tmp.lowValue = y;
        tmp.highValue = y + int(valDom * width);
        sub.constraints.push_back(tmp);
    }
    return sub;
}

void intervalGenerator::GenHotSubList()
{
    GenHot();
    for (int i = 0; i < subs; i++)
    {
        IntervalSub sub = GenOneHotSub(i);

        subList.push_back(sub);
    }

    ofstream fileStream;
    fileStream.open("./data/subList.txt");
    for (int i=0; i<subs; ++i)
    {
        IntervalSub &sub = subList[i];
        fileStream << sub.id << "\t" << sub.size << "\t";
        for (int j=0; j<sub.size; ++j)
            fileStream << sub.constraints[j].att << "\t" << sub.constraints[j].lowValue << "\t" << sub.constraints[j].highValue << "\t";;
        fileStream << "\n";
    }
    fileStream.close();
}

IntervalSub intervalGenerator::GenOneHotSub(int id)
{
    IntervalSub sub;
    sub.id = id;
    sub.size = random(cons)+1;
    width = 0.9 * (rand() / (RAND_MAX + 1.0)) + 0.1;
    vector<int> a;
    for (int i = 0; i < sub.size; i++)
    {
        int x = random(atts);
        while (CheckExist(a,x))
            x = random(atts);
        a.push_back(x);
        IntervalCnt tmp;
        tmp.att = x;
        int hotcenter = GetHot(x);
        int xx = hotcenter - (int)(valDom / 2 * width);
        int y = hotcenter + (int)(valDom / 2 * width);
        if (xx < 0){
            xx = 0;
            y = valDom * width - 1;
        } else if (y >= valDom){
            y = valDom - 1;
            xx = valDom * (1 - width);
        }
        tmp.lowValue = xx;
        tmp.highValue = y;
        sub.constraints.push_back(tmp);
    }
    return sub;
}


//----------------------pub---------------------------------
void intervalGenerator::ReadPubList(){
    ifstream fileStream;
    fileStream.open("./data/pubList.txt");
    for (int i=0; i<pubs; ++i){
        Pub pub;
        fileStream >> pub.id >> pub.size;
        for (int j = 0; j < pub.size; ++j){
            Pair tmp;
            fileStream >> tmp.att >> tmp.value;
            pub.pairs.push_back(tmp);
        }
        pubList.push_back(pub);
    }
    fileStream.close();
}

void intervalGenerator::GenPubList()
{
    for (int i = 0; i < pubs; i++)
    {   
        Pub pub = GenOnePub(i);
        pubList.push_back(pub);
    }
    ofstream fileStream;
    fileStream.open("./data/pubList.txt");
    for (int i = 0; i < pubs; i++)
    {
        Pub &pub = pubList[i];
        fileStream << pub.id << "\t" << pub.size << "\t";
        for (int j=0; j<pub.size; ++j)
            fileStream << pub.pairs[j].att << "\t" << pub.pairs[j].value << "\t";
        fileStream << "\n";
    }
    fileStream.close();
}

Pub intervalGenerator::GenOnePub(int id)
{
    Pub pub;
    pub.id = id;
    pub.size = m;
    for (int i = 0; i < m; i++)
    {
        Pair tmp;
        tmp.att = i;
        tmp.value = random(valDom);
        pub.pairs.push_back(tmp);
    }
    return pub;
}

void intervalGenerator::GenHotPubList()
{
    GenHot();
    for (int i = 0; i < pubs; i++)
    {   
        Pub pub = GenOneHotPub(i);
        pubList.push_back(pub);
    }
    ofstream fileStream;
    fileStream.open("./data/pubList.txt");
    for (int i = 0; i < pubs; i++)
    {
        Pub &pub = pubList[i];
        fileStream << pub.id << "\t" << pub.size << "\t";
        for (int j=0; j<pub.size; ++j)
            fileStream << pub.pairs[j].att << "\t" << pub.pairs[j].value << "\t";
        fileStream << "\n";
    }
    fileStream.close();
}

Pub intervalGenerator::GenOneHotPub(int id)
{
    Pub pub;
    pub.id = id;
    pub.size = m;
    int bias = random(valDom);
    for (int i = 0; i < m; i++)
    {
        Pair tmp;
        tmp.att = i;
        tmp.value = (GetHot(i) + bias) % valDom;
        pub.pairs.push_back(tmp);
    }
    return pub;
}

