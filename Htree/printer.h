#ifndef _PRINTER_H
#define _PRINTER_H

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "data_structure.h"
#include "util.h"
#include <vector>

using namespace std;

class Printer {

public:

	static string PrintParameters(int argc, char *argv[]);

	//static void PrintProgress (string name, int num, int unit, int index);

	static string FormatStatistics(vector<double> valueList);

	static string PrintMatchSubStatistics(vector<int> matchSubList);

	static string PrintTimeStatistics(vector<double> matchTimeList);

	static string PrintFileName(string method);

	static void SaveResults(string fileName, vector<double> insertTimeList, vector<double> matchTimeList,
							vector<int> matchSubList, int subs, int atts, int cons, int m, int attDis, int valDis,
							double alpha);

	//static void PrintSubscription (Sub sub, bool showDetail=false);
	//static void PrintPublication (Pub pub);
};

#endif
