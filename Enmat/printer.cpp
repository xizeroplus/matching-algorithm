#include "printer.h"
#include <sstream>
#include "data_structure.h"

using namespace std;


/*
	void Printer::PrintProgress(string name, int num, int unit, int index)
	{
		if (num >= unit)
		{
			if (index % (num / unit) == 0)
				NS_LOG_INFO(name << "No:" << "\t" << index);
		}
		else
			NS_LOG_INFO(name << "No:" << "\t" << index);
	}
*/


	string Printer::PrintParameters(int argc, char *argv[])
	{
		string result = "";
		string str = argv[0];
		int loc = str.rfind('/');
		result += "\n**************  " + str.substr(loc + 1) + "  **************\n";
		result += "Parameters:\n";

		for (int i = 1; i<argc; i++)
		{
			str = argv[i];
			str = str.substr(2);
			loc = str.find('=');
			result += Util::Int2String(i) + ")\t";
			result += str.substr(0, loc) + "\t";
			result += str.substr(loc + 1) + "\n";
		}
		return result;
	}

	string Printer::FormatStatistics(vector <double> valueList)
	{
		string text = "";
		text += "1)\tAvg\t" + Util::RemoveLastZero(Util::Double2String(valueList.at(0))) + "\n";
		text += "2)\tMin\t" + Util::RemoveLastZero(Util::Double2String(valueList.at(1))) + "\n";
		text += "3)\tMax\t" + Util::RemoveLastZero(Util::Double2String(valueList.at(2))) + "\n";
		text += "4)\tStd\t" + Util::RemoveLastZero(Util::Double2String(valueList.at(3))) + "\n";
		return text;
	}


	string Printer::PrintMatchSubStatistics(vector<int> matchSubList)
	{
		string text = "\n";
		vector<double> result = Util::ComputeIntStatistics(matchSubList);
		text += "Statistics of Matching Subs:\n";
		text += FormatStatistics(result);
		return text;
	}



	string Printer::PrintTimeStatistics(vector<double> timeList)
	{
		if (timeList.size() < 1)
			return "";
		string text = "\n";
		vector<double> result = Util::ComputeDoubleStatistics(timeList);
		//text += "Statistics of Matching Times (ms):\n";
		text += FormatStatistics(result);
		return text;
	}


	string Printer::PrintFileName(string method)
	{
		string fileName = method + ".txt";
		return fileName;
	}


	void Printer::SaveResults(string fileName, vector<double> insertTimeList,
		vector<double> matchTimeList, vector<int> matchSubList,int subs,int atts,int cons,int m,int attDis,int valDis,double alpha)
	{
		string paras=string("subs=")+Util::Int2String(subs);
		paras+=string("  atts=")+Util::Int2String(atts);
		paras+=string("  cons=")+Util::Int2String(cons);
		paras+=string("  m=")+Util::Int2String(m);
		paras+=string("  attDis=")+Util::Int2String(attDis);
		paras+=string("  valDis=")+Util::Int2String(valDis);
		paras+=string("  alpha=")+Util::Double2String(alpha)+string("\n");
		Util::WriteData(fileName.c_str(),paras);
		//Util::WriteData(fileName.c_str(), PrintParameters(argc, argv));
		Util::WriteData(fileName.c_str(), "Statistics of Insert Times (ms):\n" + PrintTimeStatistics(insertTimeList));
		Util::WriteData(fileName.c_str(), "Statistics of Match Times (ms):\n" + PrintTimeStatistics(matchTimeList));
		//Util::WriteData(fileName.c_str(), "Statistics of Delete Times (ms):\n" + PrintTimeStatistics(deleteTimeList));
		Util::WriteData(fileName.c_str(), PrintMatchSubStatistics(matchSubList));
		Util::WriteData(fileName.c_str(),"-----------------------------------------------------------------------\n");
		string text = "matchSubs\t matchTime\n";
		for (unsigned int i = 0; i<matchSubList.size(); i++)
		{
			text += Util::Int2String(matchSubList.at(i)) + "\t" + Util::RemoveLastZero(Util::Double2String(matchTimeList.at(i))) + "\n";
		}
		//fileName = fileName.substr(0, fileName.size() - 4) + "_details.txt";
		//Util::WriteData(fileName.c_str(), PrintParameters(argc, argv));
		//Util::WriteData(fileName.c_str(), text);
	}


	/*
	void Printer::PrintSubscription(Sub sub, bool showDetail)
	{
		LogComponentEnable("printer", LOG_LEVEL_INFO);

		NS_LOG_INFO("\nID:" << sub.id);
		NS_LOG_INFO("Pri:" << sub.pri);
		NS_LOG_INFO("Sel:" << sub.sel);
		NS_LOG_INFO("Size:" << sub.size);

		if (showDetail)
		{
			NS_LOG_INFO("Att" << "\t" << "lowVal" << "\t" << "highVal");
			for (uint32_t i = 0; i<sub.constraints.size(); i++)
			{
				NS_LOG_INFO(sub.constraints.at(i).att << "\t" <<
					sub.constraints.at(i).lowValue << "\t" <<
					sub.constraints.at(i).highValue);
			}
		}
	}


	void Printer::PrintPublication(Pub pub)
	{
		for (uint32_t i = 0; i<pub.pairs.size(); i++)
		{
			NS_LOG_INFO(pub.pairs.at(i).att << "\t" << pub.pairs.at(i).val);
		}
	}
	*/




