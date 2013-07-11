#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#define _USE_MATH_DEFINES
#include <math.h>

//To trims
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

using namespace std;

struct freqPhAmp{
	int FTW;
	int POW;
	int ASF;
};


class FileInterpreter
{
private:
	map<int, freqPhAmp> profiles;
	string Form( string str, char* to_Char );
	string Form( string str, int to_int );
	static inline string& trim(string &s);
	static inline string& rtrim(string &s);
	static inline string& ltrim(string &s);
	vector<string> split(const string &s, char delim);
	vector<string> &split(const string &s, char delim, vector<string> &elems);
	int round(double number); // TODO: stupid to return double?
	void cleanProfiles();

public:
	struct sweepSetup {
		enum sweepType {frequncy, phase, amplitude};
		sweepType type;
		int Start;
		int Stop;
		int risingSize;
		int fallingSize;
		int risingTime;
		int fallingTime;
	};

	FileInterpreter(char* filePath);
	map<int, freqPhAmp> getProfiles();
	int calculateFTW(double fout);
	int calculateSweepRiseSize( double subValue );
	void initializeProfiles();
	int calculatePOW( double phase );
	int calculateASF( double subValue );
};

