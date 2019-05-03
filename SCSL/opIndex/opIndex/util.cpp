#include "util.h"
using namespace std;


/*
 *  Check the correctness of the algorithm by brute force.
 */
void Util::Check(intervalGenerator &gen)
{
	//vector<double> insertTimeList;
	vector<double> matchTimeList;
	vector<double> matchSubList;

	for (int pub_iter = 0; pub_iter < gen.pubList.size(); pub_iter++)
	{
		Timer matchStart;
		int matchSubs = 0;
		Pub pub = gen.pubList[pub_iter];
		for (int i = 0; i < gen.subList.size(); i++)
		{
			bool matchFlag = true;
			for (int j = 0; matchFlag && j < gen.subList[i].size; j++)
			{
				IntervalCnt cnt = gen.subList[i].constraints[j];
				bool flag = false;
				for (int k = 0; k < pub.size; k++)
				{
					if (pub.pairs[k].att == cnt.att)
						if (pub.pairs[k].value >= cnt.lowValue && pub.pairs[k].value <= cnt.highValue)
							flag = true;

				}
				if (!flag)
					matchFlag = false;
			}
			if (matchFlag)
				++matchSubs;
		}

		int64_t matchTime = matchStart.elapsed_nano();
		matchTimeList.push_back((double) matchTime / 1000000);
		matchSubList.push_back(matchSubs);
	}

	string outputFileName = "opindex_checker.txt";
	string content = Util::Double2String(Util::Average(matchTimeList)) + "\t" +
					 Util::Double2String(Util::Average(matchSubList));
	Util::WriteData(outputFileName.c_str(), content);
}

void Util::Check(generator gen, string filename)
{
	vector<double> insertTimeList;
	vector<double> matchTimeList;
	vector<int> matchSubList;

	for (int pub_iter = 0; pub_iter < gen.pubList.size(); pub_iter++)
	{
		Timer matchStart;
		int matchSubs = 0;
		Pub pub = gen.pubList[pub_iter];
		for (int i = 0; i < gen.subList.size(); i++)
		{
			bool matchFlag = true;
			for (int j = 0; matchFlag && j < gen.subList[i].size; j++)
			{
				Cnt cnt = gen.subList[i].constraints[j];
				bool flag = false;
				for (int k = 0; k < pub.size; k++)
				{
					if (pub.pairs[k].att == cnt.att)
						if (cnt.op == 0 && pub.pairs[k].value == cnt.value)
							flag = true;
						else if (cnt.op == 1 && pub.pairs[k].value <= cnt.value)    // op == 1: >=
							flag = true;
						else if (cnt.op == 2 && pub.pairs[k].value >= cnt.value)    // op == 0: <=
							flag = true;
				}
				if (!flag)
					matchFlag = false;
			}
			if (matchFlag)
				++matchSubs;
		}

		int64_t matchTime = matchStart.elapsed_nano();
		matchTimeList.push_back((double) matchTime / 1000000);
		matchSubList.push_back(matchSubs);
	}
	string fileName = Printer::PrintFileName(filename + "_checker");
	Printer::SaveResults(fileName, insertTimeList, matchTimeList, matchSubList, gen.subs, gen.attDom, gen.k,
						 gen.m, gen.subAttDis, gen.subValDis, gen.subValalpha);
}

/*
 *  Check the correctness of the algorithm for interval generator by brute force.
 */
void Util::Check(intervalGenerator gen, string filename)
{
	vector<double> insertTimeList;
	vector<double> matchTimeList;
	vector<int> matchSubList;

	for (int pub_iter = 0; pub_iter < gen.pubList.size(); pub_iter++)
	{
		Timer matchStart;
		int matchSubs = 0;
		Pub pub = gen.pubList[pub_iter];
		for (int i = 0; i < gen.subList.size(); i++)
		{
			bool matchFlag = true;
			for (int j = 0; matchFlag && j < gen.subList[i].size; j++)
			{
				IntervalCnt cnt = gen.subList[i].constraints[j];
				bool flag = false;
				for (int k = 0; k < pub.size; k++)
					if (pub.pairs[k].att == cnt.att && pub.pairs[k].value >= cnt.lowValue &&
						pub.pairs[k].value <= cnt.highValue)
						flag = true;
				if (!flag)
					matchFlag = false;
			}
			if (matchFlag)
				++matchSubs;
		}

		int64_t matchTime = matchStart.elapsed_nano();
		matchTimeList.push_back((double) matchTime / 1000000);
		matchSubList.push_back(matchSubs);
	}
	string fileName = Printer::PrintFileName(filename + "_checker");
    Printer::SaveResults(fileName, insertTimeList, matchTimeList, matchSubList, gen.subs, gen.attDom, gen.k,
                         gen.m, gen.subAttDis, gen.subValDis, gen.subValalpha);
}


/*
 *  Calculate the average value of a vector.
 */

void Util::storeOldResult(vector<double> *old_pubMatchtime,int pubs,vector<double> *old_subMatchtime,int subs)
{
	ofstream fileStream;
	for (int i=0; i<pubs; ++i)
	{
		fileStream.open(string("../oldpubDetail/") + Util::Int2String(i+1) + string(".txt"), ios::app);
		for (int j=0; j<old_pubMatchtime[i].size(); ++j)
			fileStream << old_pubMatchtime[i][j] << "\n";
		fileStream.close();
		old_pubMatchtime[i].clear();
		old_pubMatchtime[i].shrink_to_fit();
	}
	for (int i=0; i<subs; ++i)
	{
		fileStream.open(string("../oldsubDetail/") + Util::Int2String(i+1) + string(".txt"), ios::app);
		for (int j=0; j<old_subMatchtime[i].size(); ++j)
			fileStream << old_subMatchtime[i][j] << "\n";
		fileStream.close();
		old_subMatchtime[i].clear();
		old_subMatchtime[i].shrink_to_fit();
	}
}

void Util::storeNewResult(vector<double> *new_pubMatchtime,int pubs,vector<double> *new_subMatchtime,int subs)
{
	ofstream fileStream;
	for (int i=0; i<pubs; ++i)
	{
		fileStream.open(string("../newpubDetail/") + Util::Int2String(i+1) + string(".txt"), ios::app);
		for (int j=0; j<new_pubMatchtime[i].size(); ++j)
			fileStream << new_pubMatchtime[i][j] << "\n";
		fileStream.close();
		new_pubMatchtime[i].clear();
		new_pubMatchtime[i].shrink_to_fit();
	}
	for (int i=0; i<subs; ++i)
	{
		fileStream.open(string("../newsubDetail/") + Util::Int2String(i+1) + string(".txt"), ios::app);
		for (int j=0; j<new_subMatchtime[i].size(); ++j)
			fileStream << new_subMatchtime[i][j] << "\n";
		fileStream.close();
		new_subMatchtime[i].clear();
		new_subMatchtime[i].shrink_to_fit();
	}
}

void Util::writeOldtp(int pubs, int subs, string filename) {
	vector<double> t1vec;
	vector<double> t25vec;
	vector<double> t50vec;
	vector<double> t75vec;
	vector<double> t90vec;
	vector<double> t95vec;
	vector<double> t99vec;
	vector<double> subtimevec;
	vector<double> pubtimevec;
	vector<double> avsubtimevec;
	vector<double> avpubtimevec;
	double tp;
	ifstream fileStream;
	ofstream fileStream2;
	for (int i=0; i<pubs; ++i)
	{
		fileStream.open(string("../oldpubDetail/") + Util::Int2String(i+1) + string(".txt"));
		while(fileStream >> tp)
			pubtimevec.push_back(tp);
		fileStream.close();
		if (pubtimevec.size() >= 1) t1vec.push_back(pubtimevec[0]);
		if (pubtimevec.size() >= 4) t25vec.push_back(pubtimevec[int(pubtimevec.size()*0.25)-1]);
		if (pubtimevec.size() >= 2) t50vec.push_back(pubtimevec[int(pubtimevec.size()*0.5)-1]);
		if (pubtimevec.size() >= 10) t75vec.push_back(pubtimevec[int(pubtimevec.size()*0.75)-1]);
		if (pubtimevec.size() >= 10) t90vec.push_back(pubtimevec[int(pubtimevec.size()*0.9)-1]);
		if (pubtimevec.size() >= 10) t95vec.push_back(pubtimevec[int(pubtimevec.size()*0.95)-1]);
		if (pubtimevec.size() >= 10) t99vec.push_back(pubtimevec[int(pubtimevec.size()*0.99)-1]);
        if (pubtimevec.size() >= 1) avpubtimevec.push_back(Util::Average(pubtimevec));
		pubtimevec.clear();
	}

	fileStream2.open(filename,ios::app);
	fileStream2 << "1\t" << Util::Average(t1vec) << "\n" << "25%\t" << Util::Average(t25vec) << "\n" << "50%\t" << Util::Average(t50vec) << "\n" << "75%\t" << Util::Average(t75vec) << "\n" << "90%\t" << Util::Average(t90vec) << "\n" << "95%\t" << Util::Average(t95vec) << "\n" << "99%\t" << Util::Average(t99vec) << "\n" <<  "Avg\t" << Util::Average(avpubtimevec) << "\n\n";

	t1vec.clear();
	t25vec.clear();
	t50vec.clear();
	t75vec.clear();
	t90vec.clear();
	t95vec.clear();
	t99vec.clear();

	for (int i=0; i<subs; ++i)
	{
		fileStream.open(string("../oldsubDetail/") + Util::Int2String(i+1) + string(".txt"));
		while(fileStream >> tp)
			subtimevec.push_back(tp);
		fileStream.close();
		if (subtimevec.size() >= 1) t1vec.push_back(subtimevec[0]);
		if (subtimevec.size() >= 4) t25vec.push_back(subtimevec[int(subtimevec.size()*0.25)-1]);
		if (subtimevec.size() >= 2) t50vec.push_back(subtimevec[int(subtimevec.size()*0.5)-1]);
		if (subtimevec.size() >= 10) t75vec.push_back(subtimevec[int(subtimevec.size()*0.75)-1]);
		if (subtimevec.size() >= 10) t90vec.push_back(subtimevec[int(subtimevec.size()*0.9)-1]);
		if (subtimevec.size() >= 10) t95vec.push_back(subtimevec[int(subtimevec.size()*0.95)-1]);
		if (subtimevec.size() >= 10) t99vec.push_back(subtimevec[int(subtimevec.size()*0.99)-1]);
		if (subtimevec.size() >= 1) avsubtimevec.push_back(Util::Average(subtimevec));
		subtimevec.clear();
	}
	fileStream2 << "1\t" << Util::Average(t1vec) << "\n" << "25%\t" << Util::Average(t25vec) << "\n" << "50%\t" << Util::Average(t50vec) << "\n" << "75%\t" << Util::Average(t75vec) << "\n" << "90%\t" << Util::Average(t90vec) << "\n" << "95%\t" << Util::Average(t95vec) << "\n" << "99%\t" << Util::Average(t99vec) << "\n" <<  "Avg\t" << Util::Average(avsubtimevec) << "\n\n";
	fileStream2.close();
}

void Util::writeNewtp(int pubs, int subs, string filename) {
	vector<double> t1vec;
	vector<double> t25vec;
	vector<double> t50vec;
	vector<double> t75vec;
	vector<double> t90vec;
	vector<double> t95vec;
	vector<double> t99vec;
	vector<double> subtimevec;
	vector<double> pubtimevec;
	vector<double> avsubtimevec;
	vector<double> avpubtimevec;
	double tp;
	ifstream fileStream;
	ofstream fileStream2;
	for (int i=0; i<pubs; ++i)
	{
		fileStream.open(string("../newpubDetail/") + Util::Int2String(i+1) + string(".txt"));
		while(fileStream >> tp)
			pubtimevec.push_back(tp);
		fileStream.close();
		if (pubtimevec.size() >= 1) t1vec.push_back(pubtimevec[0]);
		if (pubtimevec.size() >= 4) t25vec.push_back(pubtimevec[int(pubtimevec.size()*0.25)-1]);
		if (pubtimevec.size() >= 2) t50vec.push_back(pubtimevec[int(pubtimevec.size()*0.5)-1]);
		if (pubtimevec.size() >= 10) t75vec.push_back(pubtimevec[int(pubtimevec.size()*0.75)-1]);
		if (pubtimevec.size() >= 10) t90vec.push_back(pubtimevec[int(pubtimevec.size()*0.9)-1]);
		if (pubtimevec.size() >= 10) t95vec.push_back(pubtimevec[int(pubtimevec.size()*0.95)-1]);
		if (pubtimevec.size() >= 10) t99vec.push_back(pubtimevec[int(pubtimevec.size()*0.99)-1]);
        if (pubtimevec.size() >= 1) avpubtimevec.push_back(Util::Average(pubtimevec));
		pubtimevec.clear();
	}

	fileStream2.open(filename,ios::app);
	fileStream2 << "1\t" << Util::Average(t1vec) << "\n" << "25%\t" << Util::Average(t25vec) << "\n" << "50%\t" << Util::Average(t50vec) << "\n" << "75%\t" << Util::Average(t75vec) << "\n" << "90%\t" << Util::Average(t90vec) << "\n" << "95%\t" << Util::Average(t95vec) << "\n" << "99%\t" << Util::Average(t99vec) << "\n" <<  "Avg\t" << Util::Average(avpubtimevec) << "\n\n";

	t1vec.clear();
	t25vec.clear();
	t50vec.clear();
	t75vec.clear();
	t90vec.clear();
	t95vec.clear();
	t99vec.clear();

	for (int i=0; i<subs; ++i)
	{
		fileStream.open(string("../newsubDetail/") + Util::Int2String(i+1) + string(".txt"));
		while(fileStream >> tp)
			subtimevec.push_back(tp);
		fileStream.close();
		if (subtimevec.size() >= 1) t1vec.push_back(subtimevec[0]);
		if (subtimevec.size() >= 4) t25vec.push_back(subtimevec[int(subtimevec.size()*0.25)-1]);
		if (subtimevec.size() >= 2) t50vec.push_back(subtimevec[int(subtimevec.size()*0.5)-1]);
		if (subtimevec.size() >= 10) t75vec.push_back(subtimevec[int(subtimevec.size()*0.75)-1]);
		if (subtimevec.size() >= 10) t90vec.push_back(subtimevec[int(subtimevec.size()*0.9)-1]);
		if (subtimevec.size() >= 10) t95vec.push_back(subtimevec[int(subtimevec.size()*0.95)-1]);
		if (subtimevec.size() >= 10) t99vec.push_back(subtimevec[int(subtimevec.size()*0.99)-1]);
		if (subtimevec.size() >= 1) avsubtimevec.push_back(Util::Average(subtimevec));
		subtimevec.clear();
	}
	fileStream2 << "1\t" << Util::Average(t1vec) << "\n" << "25%\t" << Util::Average(t25vec) << "\n" << "50%\t" << Util::Average(t50vec) << "\n" << "75%\t" << Util::Average(t75vec) << "\n" << "90%\t" << Util::Average(t90vec) << "\n" << "95%\t" << Util::Average(t95vec) << "\n" << "99%\t" << Util::Average(t99vec) << "\n" <<  "Avg\t" << Util::Average(avsubtimevec) << "\n\n";
	fileStream2.close();
}

double Util::Average(const vector<double> &a)
{
    double sum = 0;
    for (int i = 0; i < a.size(); i ++)
        sum += a[i];
    return sum / a.size();
}


string Util::RemoveBlank(std::string str)
{
	int strLen = str.size();
	while (str.at(strLen - 1) == ' ')
	{
		str = str.substr(0, strLen - 1);
		strLen = str.size();
	}
	return str;
}

std::string Util::RemoveLastZero(std::string str)
{
	int strLen = str.size();
	while (str.at(strLen - 1) == '0')
	{
		str = str.substr(0, strLen - 1);
		strLen = str.size();
	}
	return str;
}

void Util::WriteData(string fileName, string text)
{
	ofstream fileStream;
	fileStream.open(fileName.c_str(), ios::app);
	if (!fileStream)
	{
		cout << "Create file failed!" << endl;
	}

	fileStream << text << endl;
	fileStream << flush;
	fileStream.close();
}

string Util::Double2String(double value)
{
	string result;
	char buffer[200];
	sprintf(buffer, "%lf ", value);
	result.assign(buffer);
	return RemoveBlank(result);
}

double Util::String2Double(string str)
{
	double result;
	std::stringstream ss(str);
	ss >> result;
	return result;
}

string Util::Int2String(int value)
{
	string result;
	ostringstream out;
	out << value;
	result = out.str();
	return RemoveBlank(result);
}

int Util::String2Int(string str)
{
	int result = 0;
	stringstream out;
	out << str;
	out >> result;
	return result;
}

bool Util::CheckExist(int value, vector<int> array)
{
	bool result = false;
	for (unsigned int i = 0; i < array.size(); i++)
	{
		if (array.at(i) == value)
		{
			result = true;
			break;
		}
	}
	return result;
}

bool Util::IsPrime(int number)
{
	number = abs(number);

	if (number == 0 || number == 1)
		return true;

	int divisor;
	for (divisor = number / 2; number % divisor != 0; --divisor);

	return divisor == 1;
}


int Util::Fac(int n)
{
	int result = 1;
	if (n < 0) return -1;
	else if (n == 0) return 1;
	while (n > 1)
	{
		result *= n;
		n--;
	}
	return result;
}


double Util::N2U(double value, double mean, double var)
{
	double v1, x, y, result;
	v1 = (value - mean) / sqrt(var);
	x = v1 / sqrt(2);
	y = 0.0;
	for (int j = 0; j < 10; j++)
	{
		y = y + (pow(-1, j) * pow(x, 2 * j + 1)) / ((2 * j + 1) * Util::Fac(j));
	}
	result = 0.5 * (1 + 2.0 / sqrt(3.1415926) * y);
	return result;
}

double Util::E2U(double value, double mean)
{
	double alpha = 1 / mean;
	double result;
	result = 1 - pow(2.71828, -alpha * value);
	return result;
}

double Util::P2U(double value, double mean, double scale)
{
	double xm = mean * ((scale - 1) / scale);
	double result;
	result = 1 - pow((xm / value), scale);
	return result;
}

void Util::OutputResult(string filename, int ope, int att, int ite, int subDis, int pubDis, double wid, int sub, int pub, double zip, double total, double avg, double max, double min, double std, int matchNo, string matchList)
{
	Util::WriteData(filename.c_str(), "Operation: \t" + Util::Int2String(ope));
	Util::WriteData(filename.c_str(), "Attributes: \t" + Util::Int2String(att));
	Util::WriteData(filename.c_str(), "Constraints: \t" + Util::Int2String(ite));
	Util::WriteData(filename.c_str(), "Constraint dis: \t" + Util::Int2String(subDis));
	Util::WriteData(filename.c_str(), "Event dis: \t" + Util::Int2String(pubDis));
	Util::WriteData(filename.c_str(), "Width : \t" + Util::RemoveLastZero(Util::Double2String(wid)));
	Util::WriteData(filename.c_str(), "Subs: \t" + Util::Int2String(sub));
	Util::WriteData(filename.c_str(), "Pubs: \t" + Util::Int2String(pub));
	Util::WriteData(filename.c_str(), "Zipf: \t" + Util::RemoveLastZero(Util::Double2String(zip)));
	Util::WriteData(filename.c_str(), "Total time(s): \t" + Util::RemoveLastZero(Util::Double2String(total / 1000)));
	Util::WriteData(filename.c_str(), "Avg time(s): \t" + Util::RemoveLastZero(Util::Double2String(avg / 1000)));
	Util::WriteData(filename.c_str(), "Max time(s): \t" + Util::RemoveLastZero(Util::Double2String(max / 1000)));
	Util::WriteData(filename.c_str(), "Min time(s): \t" + Util::RemoveLastZero(Util::Double2String(min / 1000)));
	Util::WriteData(filename.c_str(), "Std time(s): \t" + Util::RemoveLastZero(Util::Double2String(std / 1000)));

	Util::WriteData(filename.c_str(), "Matched Subs: \t" + Util::Int2String(matchNo));
	//Util::WriteData (filename.c_str (), "Matched subs list: \n" + matchList);
	Util::WriteData(filename.c_str(), "\n");

}




void Util::OutputStatistics(string method, int pubs, int subs, int atts, int comAtts, int cons, double wid, int subDis, int pubDis, double zipf, int testId, vector<int> matSubList, vector<double> matTimeList, vector< vector<int> > priList, vector< vector<double> > detTimeList, int freq[])
{
	//string detTimeByPubsFile = "scratch/determingtime_test_" + Util::Int2String(testId) + "_" + method + ".txt";
	string statisticsFile = "scratch/statistics_test_" + Util::Int2String(testId) + ".txt";
	//string intervalFile = "scratch/interval_test_" + Util::Int2String(testId) + ".txt";

	//write tilel to statistics file
	string commonTitle = method + "\tParameters: \n  Pubs:\t" + Util::Int2String(pubs)
						 + "\n  Subs:\t" + Util::Int2String(subs) + "\n  Atts:\t" + Util::Int2String(atts)
						 + "\n  ComAtts:\t" + Util::Int2String(comAtts)
						 + "\n  Cons:\t" + Util::Int2String(cons) + "\n  Wid:\t"
						 + Util::RemoveLastZero(Util::Double2String(wid))
						 + "\n  Pubs Dis:\t" + Util::Int2String(pubDis) + "\n  Subs Dis:\t" + Util::Int2String(subDis)
						 + "\n  Zipf:\t" + Util::RemoveLastZero(Util::Double2String(zipf)) + "\n  Time uint:\t(ms) ";

	//Util::WriteData (detTimeByPubsFile.c_str (), commonTitle);
	//Util::WriteData (intervalFile.c_str (), commonTitle);
	Util::WriteData(statisticsFile.c_str(), commonTitle);

	//string staTitle = "PubId\tmatSubs\tpreTime\tfinTime\tmatTime\tLFD\tLFR\tfirDet\tlasDet\tavgDet\tstdDet";
	//NS_LOG_INFO(staTitle);
	//Util::WriteData (statisticsFile.c_str (), staTitle);

	vector<double> lasDetList;
	vector<double> firDetList;
	vector<double> avgDetList;
	vector<double> stdDetList;
	vector<double> lfdList;                //last first difference = (last - first)
	vector<double> lfrList;                //last first ratio = (last - first) / first
	vector<double> intvlList;
	vector<double> finTimeList;

	for (int i = 0; i < pubs; i++)
	{
		string detTimeText;
		string intervalText;
		vector<double> temp = Util::ComputeDoubleStatistics(detTimeList[i]);
		double lfd = temp[1] - temp[0];
		double lfr = (temp[1] - temp[0]) / temp[0] * 100;
		double finTime = matTimeList[i] - temp[1];
		for (unsigned int j = 0; j < detTimeList[i].size(); j++)
		{
			if (j > 0)
			{
				double intvl = detTimeList[i].at(j) - detTimeList[i].at(j - 1);
				intvlList.push_back(intvl);
				//intervalText += Util::RemoveLastZero(Util::Double2String(intvl)) + "\t";
			}
			detTimeText += Util::Int2String(priList[i].at(j)) + "\t" +
						   Util::RemoveLastZero(Util::Double2String(detTimeList[i].at(j))) + "\r";
		}

		firDetList.push_back(temp[0]);
		lasDetList.push_back(temp[1]);
		avgDetList.push_back(temp[2]);
		stdDetList.push_back(temp[3]);
		lfdList.push_back(lfd);
		lfrList.push_back(lfr);
		finTimeList.push_back(finTime);

		//string staText = "pub" + Util::Int2String(i) + "\t"
		//	+ Util::Int2String(matSubList.at(i)) + "\t"
		//	+ Util::RemoveLastZero (Util::Double2String (temp[0])) + "\t"
		//	+ Util::RemoveLastZero (Util::Double2String (finTime)) + "\t"
		//	+ Util::RemoveLastZero (Util::Double2String (matTimeList.at(i))) + "\t"
		//	+ Util::RemoveLastZero (Util::Double2String (lfd)) + "\t"
		//	+ Util::RemoveLastZero (Util::Double2String (lfr)) + "\t"
		//	+ Util::RemoveLastZero (Util::Double2String (temp[0])) + "\t"
		//	+ Util::RemoveLastZero (Util::Double2String (temp[1])) + "\t"
		//	+ Util::RemoveLastZero (Util::Double2String (temp[2])) + "\t"
		//	+ Util::RemoveLastZero (Util::Double2String (temp[3])) + "\t";

		//Util::WriteData (detTimeByPubsFile.c_str (), detTimeText);
		//Util::WriteData (intervalFile.c_str (), intervalText);
		//Util::WriteData (statisticsFile.c_str (), staText);

		//NS_LOG_INFO ("Pub" << i << "\t" << matSubList.at(i) << "\t" << temp[0] << "\t" << finTime
		//		<< "\t" <<matTimeList.at(i) << "\t" << lfd << "\t"<< lfr << "\t" << temp[0] << "\t" << temp[1] << "\t" << temp[2] << "\t" << temp[3]);
	}

	Util::WriteData(statisticsFile.c_str(), "");

	vector<double> matSubSta = Util::ComputeIntStatistics(matSubList);
	vector<double> matTimeSta = Util::ComputeDoubleStatistics(matTimeList);
	vector<double> finTimeSta = Util::ComputeDoubleStatistics(finTimeList);
	vector<double> firDetSta = Util::ComputeDoubleStatistics(firDetList);
	vector<double> lasDetSta = Util::ComputeDoubleStatistics(lasDetList);
	vector<double> avgDetSta = Util::ComputeDoubleStatistics(avgDetList);
	vector<double> stdDetSta = Util::ComputeDoubleStatistics(stdDetList);
	vector<double> lfdSta = Util::ComputeDoubleStatistics(lfdList);
	vector<double> lfrSta = Util::ComputeDoubleStatistics(lfrList);
	vector<double> intvlSta = Util::ComputeDoubleStatistics(intvlList);

	string staResultTitle = method +
							"\tStatistics:\nType\tmatSubs\tpreTime\tfinTime\tmatTime\tlfd\tlfr\tIntvl\tfirDet\tlasDet\tavgDet\tstdDet";

	Util::WriteData(statisticsFile.c_str(), staResultTitle);
	string minText = "Min\t"
					 + Util::RemoveLastZero(Util::Double2String(matSubSta[0])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(firDetSta[0])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(finTimeSta[0])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(matTimeSta[0])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(lfdSta[0])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(lfrSta[0])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(intvlSta[0])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(firDetSta[0])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(lasDetSta[0])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(avgDetSta[0])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(stdDetSta[0])) + "\t";

	string maxText = "Max\t"
					 + Util::RemoveLastZero(Util::Double2String(matSubSta[1])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(firDetSta[1])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(finTimeSta[1])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(matTimeSta[1])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(lfdSta[1])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(lfrSta[1])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(intvlSta[1])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(firDetSta[1])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(lasDetSta[1])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(avgDetSta[1])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(stdDetSta[1])) + "\t";

	string avgText = "Avg\t"
					 + Util::RemoveLastZero(Util::Double2String(matSubSta[2])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(firDetSta[2])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(finTimeSta[2])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(matTimeSta[2])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(lfdSta[2])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(lfrSta[2])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(intvlSta[2])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(firDetSta[2])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(lasDetSta[2])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(avgDetSta[2])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(stdDetSta[2])) + "\t";

	string stdText = "Std\t"
					 + Util::RemoveLastZero(Util::Double2String(matSubSta[3])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(firDetSta[3])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(finTimeSta[3])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(matTimeSta[3])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(lfdSta[3])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(lfrSta[3])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(intvlSta[3])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(firDetSta[3])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(lasDetSta[3])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(avgDetSta[3])) + "\t"
					 + Util::RemoveLastZero(Util::Double2String(stdDetSta[3])) + "\t";
	string freqText = "";
	for (int i = 0; i < atts; i++)
		freqText += Util::Int2String(freq[i]) + "\t";

	Util::WriteData(statisticsFile.c_str(), minText);
	Util::WriteData(statisticsFile.c_str(), maxText);
	Util::WriteData(statisticsFile.c_str(), stdText);
	Util::WriteData(statisticsFile.c_str(), avgText);
	Util::WriteData(statisticsFile.c_str(), freqText);
	Util::WriteData(statisticsFile.c_str(), "");
	Util::WriteData(statisticsFile.c_str(), "");
	Util::WriteData(statisticsFile.c_str(), "");
}

//compute min, max, avg, and std of data
vector<double> Util::ComputeDoubleStatistics(vector<double> data)
{
	double minValue = 999999;
	double maxValue = 0;
	double avgValue;
	double stdValue = 0;
	double totValue = 0;
	vector<double> result;

	for (unsigned int i = 0; i < data.size(); i++)
	{
		if (data.at(i) > maxValue)
			maxValue = data.at(i);
		if (data.at(i) < minValue)
			minValue = data.at(i);
		totValue += data.at(i);
	}
	avgValue = totValue / data.size();
	for (unsigned int i = 0; i < data.size(); i++)
	{
		stdValue += pow((data.at(i) - avgValue), 2);
	}
	stdValue = sqrt(stdValue / (data.size() - 1));
	result.push_back(avgValue);
	result.push_back(minValue);
	result.push_back(maxValue);
	result.push_back(stdValue);
	return result;
}

vector<double> Util::ComputeIntStatistics(vector<int> data)
{
	double minValue = 999999;
	double maxValue = 0;
	double avgValue;
	double stdValue = 0;
	double totValue = 0;
	vector<double> result;

	for (unsigned int i = 0; i < data.size(); i++)
	{
		if (data.at(i) > maxValue)
			maxValue = data.at(i);
		if (data.at(i) < minValue)
			minValue = data.at(i);
		totValue += data.at(i);
	}

	avgValue = totValue / data.size();

	for (unsigned int i = 0; i < data.size(); i++)
	{
		stdValue += pow((data.at(i) - avgValue), 2);
	}
	stdValue = sqrt(stdValue / (data.size() - 1));
	result.push_back(avgValue);
	result.push_back(minValue);
	result.push_back(maxValue);
	result.push_back(stdValue);
	return result;
}



