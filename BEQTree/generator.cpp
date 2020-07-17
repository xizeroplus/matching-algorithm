#include "generator.h"
using namespace std;


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

