//#pragma once
#include "stdafx.h"

#ifndef Utils_h
#define Utils_h


#include <math.h>
#include <string>

// To splits
#include <vector>
#include <sstream>


// To trims
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

using namespace std;

int myRound(double number);

/*	These trim functions was taken from http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring and modified by removing std::
	Thank you Evan Teran :) */
// Trim from start
string& ltrim(string &s);

// trim from end
string& rtrim(string &s);

// trim from both ends
string& trim(string &s);

/*	These split funtions was taken from: http://stackoverflow.com/questions/236129/splitting-a-string-in-c and modified by removing std::
	Thank you Evan Teran :) */
vector<string> &split(const string &s, char delim, vector<string> &elems);

vector<string> split(const string &s, char delim);


string Form( string str, char* to_Char );

string Form( string str, int to_int );

#endif