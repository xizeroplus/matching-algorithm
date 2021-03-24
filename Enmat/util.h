#ifndef _UTIL_H
#define _UTIL_H

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <cmath>
#include <vector>
#include <cstring>
#include "data_structure.h"
#include "generator.h"
#include "chrono_time.h"
#include "printer.h"
using namespace std;
									 

class Util {
public:

	static void Check(intervalGenerator gen, string filename);

	static double Average(const vector<double> &a);
	
	static double Average(const vector<int> &a);
	
	static double Variance(const vector<double> &a);
	
	static pair<double, double> linfit(const vector<double> &x, const vector<double> &y);

	static std::string RemoveBlank(std::string str);

	static std::string RemoveLastZero(std::string str);

	static void WriteData(string fileName, string text);

	static string Double2String(double value);

	static double String2Double(string str);

	static string Int2String(int value);

	static int String2Int(string str);

	static bool CheckExist(int value, vector<int> array);

	static bool IsPrime(int number);

	static int Fac(int n);

	static double N2U(double value, double mean, double var);     //normal distribution to uniform distribution
	static double E2U(double value, double mean);     //exponential distribution to uniform distribution
	static double P2U(double value, double mean, double scale);     //pareto distribution to uniform distribution

	static void
	OutputResult(string filename, int ope, int att, int ite, int subDis, int pubDis, double wid, int sub, int pub,
				 double zip, double total, double avg, double max, double min, double std, int matchNo,
				 string matchList);

	static void
	OutputStatistics(string filename, int pubs, int subs, int atts, int comAtts, int cons, double wid, int subDis,
					 int pubDis, double zipf, int testId, vector<int> matSubList, vector<double> matTimeList,
					 vector<vector<int> > priList, vector<vector<double> > detTimeList, int freq[]);

	static vector<double> ComputeDoubleStatistics(vector<double> data);

	static vector<double> ComputeIntStatistics(vector<int> data);

};

#endif
