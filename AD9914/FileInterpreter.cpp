#include "FileInterpreter.h"

FileInterpreter::FileInterpreter(char* filePath) {
	cleanProfiles();

	bool sweepOn = false;
	sweepSetup sw;
	// Initiate profile map


	ifstream file(filePath);
	string line;

	if(file.fail()) {
		file.close();
		throw invalid_argument(Form("Something is wrong with the file %s", filePath));  //TODO: this is not right yet
	}

	int lineNum = 0;

	int profile = 0;
	double frequency = 0;
	int amplitude = 4095; //This number represents the maximum 3 bit amplitude scale
	double phase = 0;


	while(getline(file, line)) {
		lineNum += 1;
		if (line.empty() || line.front() == '#') {
		} else {
			vector<string> tokens = split(line, ':');

			// Check whether there is an subject and a subject value
			int tokenSize = tokens.size();
			if (tokenSize > 2) {
				file.close();
				throw invalid_argument(Form("Too many arguments in the line %s", lineNum));
			} else if (tokenSize < 2) {
				file.close();
				throw invalid_argument(Form("Too few arguments in the line %s", lineNum));
			}
			
			string subject;
			double subValue;

			subject = tokens[0];		
			subject = trim(tokens[0]);
			transform(subject.begin(), subject.end(), subject.begin(), ::tolower);	

			subValue = stod(trim(tokens[1]));
			
			tokens = split(subject,'_');

			// A security for token size like earlier could be implemented here!
			string prefix = tokens[0]; 
			transform(prefix.begin(), prefix.end(), prefix.begin(), ::tolower);	
			string suffix = tokens[1];
			transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);	

			if (prefix.front() == 'p') {
				profile = stoi(prefix.substr(1,1));

				//Different cases:
				if (suffix == "freq") {
					profiles[profile].FTW = calculateFTW(subValue);
				} else if (suffix == "amp") {
					profiles[profile].ASF = calculateASF(subValue);
				} else if (suffix == "phase"){
					profiles[profile].POW = calculatePOW(subValue);
				}
			}


			// Handle the kind of sweep chosen:
			//if (subject == "sweep") {
			//	sweepOn = true;
			//	string type = trim(tokens[1]);
			//	if (type == "f") {
			//		sw.type = sw.frequncy;
			//	} else if (type == "p") {
			//		sw.type = sw.phase;
			//	} else if (type == "a") {
			//		sw.type = sw.amplitude;
			//	}
			//} else {

			//if (subject == "p" || subject == "profile"){

			//	// Remember to write to profiles before changing the profile.
			//	profiles[profile].FTW = calculateFTW(frequency);
			//	profiles[profile].phase = calculatePhase(phase);
			//	profiles[profile].
			//	////cout << "The FTW " << profiles[profile].FTW << " is written to  profile " << profile << " arrayindex" << endl;
			//	profile = (int) subValue;
			//} else if (subject == "a" || subject == "amplitude"){
			//	amplitude = calculateAmp(subValue);
			//	////cout << endl << endl << "Timing changed to: " << subValue << endl << endl;
			//} else if (subject == "f" || subject == "frequency"){
			//	////cout <<  subValue << " is the new freq" << " instead of " << frequency << endl;
			//	frequency = subValue;
			//} else if (subject == "sf1" || subject == "sweep frequency start") {
			//	sw.Start = calculateFTW(subValue);
			//} else if (subject == "sf2" || subject == "sweep frequency end") {
			//	sw.Stop = calculateFTW(subValue);
			//} else if (subject == "srs" || subject == "sweep rising size") {
			//	sw.risingSize = calculateSweepRiseSize(subValue);
			//}
		}
	}
}
//profiles[profile].FTW = calculateFTW(frequency);
//profiles[profile].phase = round(phase);

void FileInterpreter::cleanProfiles(){
	for (int i = 0; i < 8 ; i = i + 1) {
		profiles[i].FTW = 0;
		profiles[i].POW = 0;
		profiles[i].ASF = 4095;
	}
}

int FileInterpreter::round(double number)
{
	return (int) (number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5));
}

int FileInterpreter::calculateFTW(double fout) {
	double fsysclk = 2.4*pow(10,9);
	int FTW = round(pow(2,32) * (fout*pow(10,6)/fsysclk));
	return FTW;
}

/* This function takes an phase offset in radians and returns the phase offset word*/
int FileInterpreter::calculatePOW( double phase ){
	return round(phase / (2.0 * M_PI) * pow(2,14));
}

/* This function takes an decimal representing the relative amplitude from 0 to 1. and returns the amplitude scale factor */
int FileInterpreter::calculateASF( double subValue ){
	return round(subValue * pow(2,14));
}

int FileInterpreter::calculateSweepRiseSize( double subValue )
{
	throw std::exception("The method or operation is not implemented.");
}

vector<string> &FileInterpreter::split(const string &s, char delim, vector<string> &elems) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

vector<string> FileInterpreter::split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}

string FileInterpreter::Form( string str, char* to_Char ) 
{
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

string FileInterpreter::Form( string str, int to_int ) 
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

string& FileInterpreter::ltrim(string &s) {
	s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
	return s;
}

// trim from end
string& FileInterpreter::rtrim(string &s) {
	s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
	return s;
}

// trim from both ends
string& FileInterpreter::trim(string &s) {
	return ltrim(rtrim(s));
}

map<int, freqPhAmp> FileInterpreter::getProfiles() {
	return profiles;
}

void FileInterpreter::initializeProfiles() {
	for (unsigned int i = 0 ; i < profiles.size() ; i++) {

	}
}
