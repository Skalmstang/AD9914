#include "FileInterpreter.h"

FileInterpreter::FileInterpreter(char* filePath) : sweepOn(false){
	sweep = new SweepSetup(SweepSetup::frequency);
	cleanProfiles();
	// Initiate profile map


	ifstream file(filePath);
	string line;

	if(file.fail()) {
		file.close();
		throw invalid_argument(Form("Something is wrong with the file %s", filePath));  //TODO: this is not right yet
	}

	int lineNum = 0;

	int profile = 0;
//	double frequency = 0;
//	int amplitude = 4095; //This number represents the maximum 3 bit amplitude scale
//	double phase = 0;

	cout << "Starting to analyse file..." << endl;
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
					double tempFreq = subValue; // Subtracting refclk!
					profiles[profile].FTW = (0 > tempFreq)? 0 : calculateFTW(tempFreq);
				} else if (suffix == "amp") {
					profiles[profile].ASF = calculateASF(subValue);
				} else if (suffix == "phase"){
					profiles[profile].POW = calculatePOW(subValue);
				}
			} else if (prefix == "sweep") {
				sweepOn = true;
				if (suffix == "start" || suffix == "freqstart" || suffix == "low" || suffix == "freqlow") {
					sweep->Start = (subValue / (double) 3 - ((double) fsysclk) * pow(10,-6)) * pow(10.0,6);
				} else if (suffix == "stop" || suffix == "end" || suffix == "freqstop" || suffix == "high" || suffix == "freqhigh") {
					sweep->Stop = (subValue / (double) 3 - ((double) fsysclk) * pow(10,-6)) * pow(10.0,6);
				} else if (suffix == "risingsize") {
					sweep->risingSize = subValue;
				} else if (suffix == "fallingsize") {
					sweep->fallingSize = subValue;
				} else if (suffix == "duration") {
					sweep->sweepDuration = subValue * pow(10,-6);
				} else if (suffix == "amp") {
					sweep->sweepAmpWord = calculateASF(subValue);
				} else if (suffix == "type") {
					switch (int(subValue)) {
					case 2:
						sweep->type = SweepSetup::phase;
						break;
					case 3:
						sweep->type = SweepSetup::amplitude;
						break;
					default:
						break;
					}
				} else{
					cout << "Argument " << subject << " with value " << subValue << " could not be interpreted" << endl;
				}
			}
		}
	}
	cout << "Done analysing!" << endl;
}

FileInterpreter::~FileInterpreter(){
	delete sweep;
}

void FileInterpreter::cleanProfiles(){
	for (int i = 0; i < 8 ; i = i + 1) {
		profiles[i].FTW = 0;
		profiles[i].POW = 0;
		profiles[i].ASF = calculateASF(0.5);
	}
}

int FileInterpreter::calculateFTW(double fout) {
	return round((pow(2,32)-1) * (fout/((double) fsysclk)));
}

/* This function takes an phase offset in radians and returns the phase offset word*/
int FileInterpreter::calculatePOW( double phase ){
	return round(phase / (2.0 * M_PI) * pow(2,14));
}

/* This function takes an decimal representing the relative amplitude from 0 to 1. and returns the amplitude scale factor */
int FileInterpreter::calculateASF( double subValue ){
	return round(subValue * (pow(2,12)-1));
}

int FileInterpreter::calculateSweepRiseTime( double freq ) {
	return round(freq * ((double) fsysclk) / 24.0);
}

map<int, freqPhAmp> FileInterpreter::getProfiles() {
	return profiles;
}

SweepSetup* FileInterpreter::getSweep() {
	return sweep;
}

bool FileInterpreter::isSweep(){
	return sweepOn;
}

void FileInterpreter::initializeProfiles() {
	for (unsigned int i = 0 ; i < profiles.size() ; i++) {

	}
}
