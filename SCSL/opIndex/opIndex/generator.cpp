#include "generator.h"

using namespace std;

void generator::GenSubList()
{
    for (int i = 0; i < subs; i++)
    {
        //cout << "gen sub" << i << endl;
        Sub sub = GenOneSub(i);

        subList.push_back(sub);
    }

    ofstream fileStream;
    fileStream.open("./Detail/subList.txt");
    for (int i=0; i<subs; ++i)
    {
        Sub &sub = subList[i];
        fileStream << "sub" << sub.id << "\t" << sub.size << "\t";
        for (int j=0; j<sub.size; ++j)
            fileStream << sub.constraints[j].att << OPR[sub.constraints[j].op] << sub.constraints[j].value << "\t";
        fileStream << "\n";
    }
    fileStream.close();
}


Sub generator::GenOneSub(int id)
{
    Sub sub;
    sub.id = id;
    sub.size = random(k) + 1;
    if (subAttDis == 0)
        GenUniformAtts(sub);
    else if (subAttDis==1)
        GenZipfAtts(sub);
    if (subValDis == 0)
        GenUniformValues(sub);
    else if (subValDis == 1)
        GenZiefValues(sub);
    return sub;
}

void generator::GenUniformAtts(Sub &sub)
{
    vector<int> a;
    for (int i=0; i<sub.size; ++i)
    {
        int x = random(attDom);
        while (CheckExist(a,x))
            x = random(attDom);
        Cnt tmp;
        tmp.att = x;
        sub.constraints.push_back(tmp);
    }
}

void generator::GenZipfAtts(Sub &sub)
{
    vector<int> a;
    first = true;
    for (int i = 0; i < sub.size; i++)
    {
        int x = zipfDistribution(attDom, subAttalpha);
        while (CheckExist(a,x))
            x = zipfDistribution(attDom, subAttalpha);
        a.push_back(x);
        Cnt tmp;
        tmp.att = x;
        sub.constraints.push_back(tmp);
    }
}

void generator::GenUniformValues(Sub &sub)
{
    for (int i = 0; i < sub.size; i++)
    {
        int x = random(valDom);
        sub.constraints[i].value = x;
        double y = rand()/(RAND_MAX + 1.0);
        if (y <equalRatio)
            sub.constraints[i].op = 0;
        else if (y>(1+equalRatio)/2)
            sub.constraints[i].op = 2;
        else
            sub.constraints[i].op = 1;
    }
}

void generator::GenZiefValues(Sub &sub)
{
    first = true;
    for (int i = 0; i < sub.size; i++)
    {
        int x = zipfDistribution(valDom, subValalpha);
        sub.constraints[i].value = x;
        double y = rand()/(RAND_MAX + 1.0);
        if (y <equalRatio)
            sub.constraints[i].op = 0;
        else if (y>(1+equalRatio)/2)
            sub.constraints[i].op = 2;
        else
            sub.constraints[i].op = 1;
    }
}


void generator::GenPubList()
{
    for (int i = 0; i < pubs; i++)
    {
        Pub pub = GenOnePub();
        pubList.push_back(pub);
    }
    ofstream fileStream;
    fileStream.open("./Detail/pubList.txt");
    for (int i = 0; i < pubs; i++)
    {
        Pub &pub = pubList[i];
        fileStream << "pub" << i << "\t";
        for (int j=0; j<pub.size; ++j)
            fileStream << pub.pairs[j].att << "=" << pub.pairs[j].value << "\t";
        fileStream << "\n";
    }
    fileStream.close();
}


Pub generator::GenOnePub()
{
    Pub pub;
    pub.size = m;
    if (pubAttDis == 0)
        GenUniformAtts(pub);
    else if (pubAttDis == 1)
        GenZipfAtts(pub);
    if (pubValDis == 0)
        GenUniformValues(pub);
    else if (pubValDis == 1)
        GenZiefValues(pub);
    return pub;
}

void generator::GenUniformAtts(Pub &pub)
{
    vector<int> a;
    for (int i = 0; i < pub.size; i++)
    {
        int x = random(attDom);
        while (CheckExist(a,x))
            x = random(attDom);
        a.push_back(x);
        Pair tmp;
        tmp.att = x;
        pub.pairs.push_back(tmp);
    }
}

void generator::GenZipfAtts(Pub &pub)
{
    first = true;
    vector<int> a;
    for (int i = 0; i < pub.size; i++)
    {
        int x = zipfDistribution(attDom, pubAttalpha);
        while (CheckExist(a,x))
            x = zipfDistribution(attDom, pubAttalpha);
        a.push_back(x);
        Pair tmp;
        tmp.att = x;
        pub.pairs.push_back(tmp);
    }
}


void generator::GenUniformValues(Pub &pub)
{
    for (int i = 0; i < pub.size; i++)
    {
        int x = random(valDom);
        pub.pairs[i].value = x;
    }
}

void generator::GenZiefValues(Pub &pub)
{
    first = true;
    for (int i = 0; i < pub.size; i++)
    {
        int x = zipfDistribution(valDom, pubValalpha);
        pub.pairs[i].value = x;
    }
}






//*********************intervalGenerator*******************************



void intervalGenerator::GenSubList()
{
    for (int i = 0; i < subs; i++)
    {
        IntervalSub sub = GenOneSub(i);

        subList.push_back(sub);
    }

    ofstream fileStream;
    fileStream.open("./Detail/subList.txt");
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

void intervalGenerator::ReadSubList(){
	ifstream fileStream;
	fileStream.open("./Detail/subList.txt");
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
		
		

IntervalSub intervalGenerator::GenOneSub(int id)
{
    IntervalSub sub;
    sub.id = id;
    sub.size = random(k) + 1;
    if (subAttDis == 0)
        GenUniformAtts(sub);
    else if (subAttDis==1)
        GenZipfAtts(sub);
    if (subValDis == 0)
        GenUniformValues(sub);
    else if (subValDis == 1)
        GenZiefValues(sub);
    return sub;
}

void intervalGenerator::GenUniformAtts(IntervalSub &sub)
{
    vector<int> a;
    for (int i=0; i<sub.size; ++i)
    {
        int x = random(attDom);
        while (CheckExist(a,x))
            x = random(attDom);
		a.push_back(x);
        IntervalCnt tmp;
        tmp.att = x;
        sub.constraints.push_back(tmp);
    }
}

void intervalGenerator::GenZipfAtts(IntervalSub &sub)
{
    vector<int> a;
    first = true;
    for (int i = 0; i < sub.size; i++)
    {
        int x = zipfDistribution(attDom, subAttalpha);
        while (CheckExist(a,x))
            x = zipfDistribution(attDom, subAttalpha);
        a.push_back(x);
        IntervalCnt tmp;
        tmp.att = x;
        sub.constraints.push_back(tmp);
    }
}

void intervalGenerator::GenUniformValues(IntervalSub &sub)
{
    for (int i = 0; i < sub.size; i++)
    {
		width = 0.4 * (rand() / (RAND_MAX + 1.0)) +0.1; //0.1-0.5 random
        if (width<1){
            int x = random( int(valDom * (1.0 - width)) );
            int y = x + int(valDom * width);
            sub.constraints[i].lowValue = x;
            sub.constraints[i].highValue = y;
        }
        else {
            int x = random(valDom);
            int y = random(valDom);
            if (x<y)
            {
                sub.constraints[i].lowValue = x;
                sub.constraints[i].highValue = y;
            }
            else{
                sub.constraints[i].lowValue = y;
                sub.constraints[i].highValue = x;
            }
        }

    }
}

void intervalGenerator::GenZiefValues(IntervalSub &sub)
{
    first = true;
    int Dom = int(valDom * (1.0 - width));
    for (int i = 0; i < sub.size; i++)
    {
        int x = zipfDistribution(Dom, subValalpha);
        int y = x + int(valDom * width);
        sub.constraints[i].lowValue = x;
        sub.constraints[i].highValue = y;
    }
}


void intervalGenerator::GenPubList()
{
	c=0;
	first=true;
    for (int i = 0; i < pubs; i++)
    {	
		nowpub = i;
		if(i == nexthot){
			Pub hot = GenHotPub();
			hotlist.push_back(hot);
			hotlife.push_back(random(400)+200); //200-600 half-life time
			hotscale.push_back(random(100)+100); //100-200 reduce time
			++hotsum;
			nexthot += random(200) + 200; //200-400 step to create new hotspot
			pubList.push_back(hot);
		}
		else{
			Pub pub = GenOnePub();
			pubList.push_back(pub);
		}
    }
	cout<<"total generated "<<hotsum<<" hotspots"<<endl;
    ofstream fileStream;
    fileStream.open("./Detail/pubList.txt");
    for (int i = 0; i < pubs; i++)
    {
        Pub &pub = pubList[i];
        //fileStream << i << "\t";
        for (int j=0; j<pub.size; ++j)
            fileStream << pub.pairs[j].att << "\t" << pub.pairs[j].value << "\t";
        fileStream << "\n";
    }
    fileStream.close();
}

void intervalGenerator::ReadPubList(){
	ifstream fileStream;
	fileStream.open("./Detail/pubList.txt");
	for (int i=0; i<pubs; ++i){
		Pub pub;
		pub.size = m;
		for (int j=0; j<pub.size; ++j){
			Pair tmp;
			fileStream >> tmp.att >> tmp.value;
			pub.pairs.push_back(tmp);
		}
		pubList.push_back(pub);
	}
	fileStream.close();
}



Pub intervalGenerator::GenHotPub()
{
    Pub pub;
    pub.size = m;
    if (pubAttDis == 0)
        GenUniformAtts(pub);
    else if (pubAttDis == 1)
        GenZipfAtts(pub);
    if (true)
        GenHotValues(pub);
    else if (pubValDis == 1)
        GenZiefValues(pub);
    return pub;
}

Pub intervalGenerator::GenOnePub()
{
    Pub pub;
    pub.size = m;
    if (pubAttDis == 0)
        GenUniformAtts(pub);
    else if (pubAttDis == 1)
        GenZipfAtts(pub);
    if (pubValDis == 0)
        GenUniformValues(pub);
    else if (pubValDis == 1)
        GenZiefValues(pub);
    return pub;
}




void intervalGenerator::GenUniformAtts(Pub &pub)
{
    for (int i = 0; i < pub.size; i++)
    {
        Pair tmp;
        tmp.att = i;
		tmp.value = 0;
        pub.pairs.push_back(tmp);
    }
}

void intervalGenerator::GenZipfAtts(Pub &pub)
{
    first = true;
    vector<int> a;
    for (int i = 0; i < pub.size; i++)
    {
        int x = zipfDistribution(attDom, pubAttalpha);
        while (CheckExist(a,x))
            x = zipfDistribution(attDom, pubAttalpha);
        a.push_back(x);
        Pair tmp;
        tmp.att = x;
        pub.pairs.push_back(tmp);
    }
}


void intervalGenerator::GenHotValues(Pub &pub)
{
    for (int i = 0; i < pub.size; i++)
    {
        int x = random(valDom);
        pub.pairs[i].value = x;
    }
}

void intervalGenerator::GenUniformValues(Pub &pub)
{
	int changesize = random(pub.size+1);
	vector<int> a;
    for (int i = 0; i < changesize; i++)
    {
        int x = random(attDom);
        while (CheckExist(a,x))
            x = random(attDom);
		a.push_back(x);
        pub.pairs[x].value = random(valDom/10)-valDom/20; //range of away from hot event: ±5%
    }
	for (int i = 0; i < pub.size; i++){
		pub.pairs[i].value += hotlist[0].pairs[i].value;
		if (pub.pairs[i].value < 0) pub.pairs[i].value = 0;
		if (pub.pairs[i].value >= valDom) pub.pairs[i].value = valDom-1;
	}

}



void intervalGenerator::GenZiefValues(Pub &pub)
{
    for (int i = 0; i < pub.size; i++)
    {
        int x = zipfDistribution(valDom/2, pubValalpha);
		int sign = random(2);
        if(sign)pub.pairs[i].value = x;
		else pub.pairs[i].value = -x;
    }
	vector<double> hotposs;
	double posssum = 0, tmpsum = 0, nowposs;
	int result = 0;
	for(int i = 0; i < hotsum; ++i){
		double poss = 1.0/(1+exp(10*(nowpub-hotlife[i])/hotscale[i])); //sigmod-like function
		hotposs.push_back(poss);
		posssum += poss;
	}
	nowposs = posssum * (rand() / (RAND_MAX + 1.0));
	for(int i = 0; i < hotsum; ++i){
		tmpsum += hotposs[i];
		if(nowposs < tmpsum){
			result = i;
			break;
		}
		if(i == hotsum-1){
			cout<<"generate poss error!"<<endl;
		}
	}
	Pub &hot = hotlist[result];
	for (int i = 0; i < pub.size; i++){
		pub.pairs[i].value += hot.pairs[i].value;
		if (pub.pairs[i].value < 0) pub.pairs[i].value = 0;
		if (pub.pairs[i].value >= valDom) pub.pairs[i].value = valDom-1;
	}
}


int generator::zipfDistribution(int n, double alpha)
{
    // alpha > 0
    double z;                     // Uniform random number (0 < z < 1)
    double sum_prob;              // Sum of probabilities
    double zipf_value = 0;            // Computed exponential value to be returned
    int i;                     // Loop counter

    if (first)
    {
        for (i = 1; i <= n; i++)
            c += (1.0 / pow((double) i, alpha));
        c = 1.0 / c;
        first = false;
    }

    while (true)
    {
        // Pull a uniform random number (0 < z < 1)
        do
        {
            z = (double) rand() / RAND_MAX;
        } while ((z == 0) || (z == 1));

        // Map z to the value
        sum_prob = 0;
        for (i = 1; i <= n; i++)
        {
            sum_prob = sum_prob + c / pow((double) i, alpha);
            if (sum_prob >= z)
            {
                zipf_value = i;
                break;
            }
        }
        if (zipf_value >= 1 && zipf_value <= n)
            return int(zipf_value - 1);
    }
}

bool generator::CheckExist(vector<int> a,int x)
{
    for (int i = 0; i < a.size(); i++)
        if (a[i] == x)
            return true;
    return false;
}

int intervalGenerator::zipfDistribution(int n, double alpha)
{
    double z;                     // Uniform random number (0 < z < 1)
    double sum_prob;              // Sum of probabilities
    int zipf_value=0;            // Computed exponential value to be returned
    int    i;                     // Loop counter
    if (first)
    {	
		c=0;
        for (i=1; i<=n; i++)
            c += 1.0 / pow((double) i, alpha);
        c = 1.0 / c;
        first = false;
    }

    while(true)
    {
        // Pull a uniform random number (0 < z < 1)
        do {
            z = (double) rand() / RAND_MAX;
        } while ((z == 0) || (z == 1));

        // Map z to the value
        sum_prob = 0;
        for (i = 1; i <= n; i++) {
			//cout<<z<<' '<<i<<endl;
            sum_prob += c / pow((double) i, alpha);
            if (sum_prob >= z) {
                zipf_value = i;
                break;
            }
        }

        //cout << zipf_value << endl;
        if (zipf_value>=1&&zipf_value<=n)
            return (zipf_value-1);

    }
}

bool intervalGenerator::CheckExist(vector<int> a,int x)
{
    for (int i = 0; i < a.size(); i++)
        if (a[i] == x)
            return true;
    return false;
}

int generator::random(int x)
{
    return (int) (x * (rand() / (RAND_MAX + 1.0)));
}

int intervalGenerator::random(int x)
{
    return (int) (x * (rand() / (RAND_MAX + 1.0)));
}

/*
void intervalGenerator::GenzipfPubList() {
    ofstream pubDetail;
    pubDetail.open("pubDetail.txt");
    pubDetail << "pubID" << "\tpair\n";

    //srand(time(NULL));

    vector<int> attsVec;
    vector<int> valuesVec;

    first = true;
    c = 0;

    for (int i = 0; i < pubs; i++) {
        vector<int> a;
        for (int j = 0; j < m; j++) {
            int x = zipfDistribution(atts, alpha);
            while (CheckExist(a, x))
                x = zipfDistribution(atts, alpha);
            a.push_back(x);
            attsVec.push_back(29-x);
        }
    }

    //cout << "check1" << endl;
    first = true;
    c = 0;
    int limit = pubs * m;
    for (int i=0; i<limit; i++){
        valuesVec.push_back(zipfDistribution(valDom, alpha));
    }

    //cout << "check2" << endl;

    int k = 0;

    for (int i = 0; i < pubs; i++) {
        Pub pub;
        pub.size = m;
        for (int j = 0; j < m; j++) {
            Pair tmp;
            tmp.att = attsVec[k];
            tmp.value = (valuesVec[k]+400000)%valDom;
            pub.pairs.push_back(tmp);
            ++k;
        }
        pubList.push_back(pub);
    }

    for (int i = 0; i < pubs; i++)
    {
        pubDetail << i << "\t";
        for (size_t j=0; j< m; ++j)
        {
            pubDetail << pubList[i].pairs[j].att << "=" << pubList[i].pairs[j].value << " ";
        }
        pubDetail << "\n";
    }

    pubDetail.close();
}

void intervalGenerator::GenUniformSubList() {
    ofstream subDetail;
    subDetail.open("subList.txt");
    subDetail << "subID" << "\tcons\n";

    vector<int> attsVec;
    vector<int> valuesVec;

    for (int i = 0; i < subs; i++) {
        vector<int> a;
        for (int j = 0; j < cons; j++) {
            int x = random(atts);
            while (CheckExist(a, x))
                x = random(atts);
            a.push_back(x);
            attsVec.push_back(x);
        }
    }

    int limit = subs * cons;
    for (int i=0; i<limit; i++)
        valuesVec.push_back(random( int(valDom * (1.0 - width)) ));


    int k=0;
    for (int i = 0; i < subs; i++) {
        IntervalSub sub;
        sub.id = i;
        sub.size = cons;
        for (int j = 0; j < cons; j++) {
            IntervalCnt tmp;
            tmp.att = attsVec[k];
            tmp.lowValue = valuesVec[k];
            tmp.highValue = valuesVec[k] + int(valDom * width);
            sub.constraints.push_back(tmp);
            ++k;
        }
        subList.push_back(sub);
    }

    for (int i = 0; i < subs; i++)
    {
        subDetail << subList[i].id << "\t";
        for (size_t j=0; j<subList[i].constraints.size(); ++j)
            subDetail << subList[i].constraints[j].att << ":" << subList[i].constraints[j].lowValue << "-" << subList[i].constraints[j].highValue << " ";
        subDetail << "\n";
    }
    subDetail << flush;
    subDetail.close();

}
*/
