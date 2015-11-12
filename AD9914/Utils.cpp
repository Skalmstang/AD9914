#include "Utils.h"


int myRound(double number){
	return (int) (number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5));
}


string& ltrim(string &s) {
	s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
	return s;
}

// trim from end
string& rtrim(string &s) {
	s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
	return s;
}

// trim from both ends
string& trim(string &s) {
	return ltrim(rtrim(s));
}


vector<string> &split(const string &s, char delim, vector<string> &elems) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


vector<string> split(const string &s, char delim) {
	std::vector<string> elems;
	split(s, delim, elems);
	return elems;
}


string Form( string str, char* to_Char ) {
	string from = "%s";
	string to	= string(to_Char);
	if (from.empty()) {
		return str;
	} else {
		size_t start_pos = 0;
		while((start_pos = str.find(from, start_pos)) != string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}
	return str;
}

string Form( string str, int to_int ) 
{
	string from = "%s";
	string to	= to_string(to_int);
	if (from.empty()) {
		return str;
	} else {
		size_t start_pos = 0;
		while((start_pos = str.find(from, start_pos)) != string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}
	return str;
}
