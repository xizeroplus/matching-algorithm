#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "data_structure.h"
#include "generator.h"
#include "chrono_time.h"
#include "printer.h"
#include <cmath>
#include <vector>
using namespace std;
									 

class Util
{

public:

	static void check(generator gen, string filename);
	static void check(intervalGenerator gen, string filename);
	static double mean(const vector<double> &a);
	//static vector <Para> ParseParameters (int argc, char *argv[]);
	
	 
	static std::string AddZero (std::string str, int len);
	static std::string RemoveBlank (std::string str);
	static std::string RemoveLastZero(std::string str);
	static void Maopao(vector<int> &vec);
	
	static void WriteData (string fileName, string text);
	
	static string Double2String (double value);
	static double String2Double (string str);
	static string Int2String (int value);
	static int String2Int (string str);
	
	static bool CheckExist (int value, vector<int> array);
	static bool	IsPrime(int number);
	static int Fac(int n);	
	
	
	static bool OptMatching (Sub sub, Pub pub, int cardi, int start=0);	//optimized matched
	static bool SimMatching (Sub sub, Pub pub, int cardi, int start=0);  //simple match


	static double N2U(double value, double mean, double var);	 //normal distribution to uniform distribution
	static double E2U(double value, double mean);	 //exponential distribution to uniform distribution
	static double P2U(double value, double mean, double scale);	 //pareto distribution to uniform distribution
//private:	
	//static uint32_t sat_pubs;
		
	static string PrintHead (int oper);
	
	
	static void ListResult (int matchNo, double total, double avg, double max, double min, double std);
	static void OutputResult (string filename, int ope, int att, int ite, int subDis, int pubDis, double wid, int sub, int pub, double zip, double total, double avg, double max, double min, double std, int matchNo, string  matchList);
	
	static void OutputStatistics(string filename, int pubs, int subs, int atts, int comAtts, int cons, double wid, int subDis, int pubDis, double zipf, int testId, vector<int> matSubList, vector<double> matTimeList, vector< vector<int> > priList, vector< vector<double> > detTimeList, int freq[]);

	
	static vector<double> ComputeDoubleStatistics (vector<double> data);
	static vector<double> ComputeIntStatistics (vector<int> data);
	
	static vector<int> GenerateDelID (int del, int sub);
	
	static int BiSearch(vector<double> array, double target);
	//static vector<IS> ShellSort(vector<IS> data);
	
	static vector<int> SortAtt (int fre[], int atts);
	
};

#endif
