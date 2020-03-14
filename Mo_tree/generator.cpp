#include "generator.h"
using namespace std;


/*void intervalGenerator::GenSubList()
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
		fileStream << sub.size << "\t";
        for (int j=0; j<sub.size; ++j)
            fileStream << sub.constraints[j].att << "\t" << sub.constraints[j].lowValue << "\t" << sub.constraints[j].highValue << "\t";;
        fileStream << "\n";
    }
    fileStream.close();
}*/

void intervalGenerator::ReadSubList(){
	ifstream fileStream;
	fileStream.open("../data/subList.txt");
	for (int i=0; i<subs; ++i){
		IntervalSub sub;
		fileStream >> sub.id >> sub.lowlng >> sub.highlng >> sub.lowlat >> sub.highlat >> sub.size;
		for (int j=0; j<sub.size; ++j){
			IntervalCnt tmp;
			fileStream >> tmp.att >> tmp.lowValue >> tmp.highValue;
			sub.constraints.push_back(tmp);
		}
		subList.push_back(sub);
	}
	fileStream.close();
}

/*IntervalSub intervalGenerator::GenOneSub(int id)
{
    IntervalSub sub;
    sub.id = id;
    sub.size = random(cons)+1;
	
	vector<int> a;
    for (int i = 0; i < sub.size; i++)
    {
        int x = random(atts);
        while (CheckExist(a,x))
            x = random(atts);
        a.push_back(x);
        IntervalCnt tmp;
        tmp.att = x;
		
		
		
		int xx = random(valDom);
        int y = random(valDom);
        tmp.lowValue = xx<y?xx:y;
        tmp.highValue = xx>y?xx:y;
        sub.constraints.push_back(tmp);
    }

    return sub;
}*/



//----------------------pub---------------------------------



/*void intervalGenerator::GenPubList()
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
        for (int j=0; j<pub.size; ++j)
            fileStream << pub.pairs[j].value << "\t";
        fileStream << "\n";
    }
    fileStream.close();
}*/

void intervalGenerator::ReadPubList(){
	ifstream fileStream;
	fileStream.open("../data/pubList.txt");
	for (int i=0; i<pubs; ++i){
		Pub pub;
		fileStream >> pub.id >> pub.lng >> pub.lat >> pub.size;
		for (int j = 0; j < pub.size; ++j){
			Pair tmp;
			fileStream >> tmp.att >> tmp.value;
			pub.pairs.push_back(tmp);
		}
		pubList.push_back(pub);
	}
	fileStream.close();
}

/*Pub intervalGenerator::GenOnePub(int id)
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
*/




//----------------------tools---------------------------------
/*bool intervalGenerator::CheckExist(const vector<int> &a,int x)
{
    for (int i = 0; i < a.size(); i++)
        if (a[i] == x)
            return true;
    return false;
}

int intervalGenerator::random(int x)
{
    return (int) (x * (rand() / (RAND_MAX + 1.0)));
}*/