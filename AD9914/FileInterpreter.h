#ifndef FileInterpreter_h
#define FileInterpreter_h

#include <fstream>
#include <iostream>
//#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <assert.h>
#include "Utils.h"

#define _USE_MATH_DEFINES
#include <math.h>

////To trims
//#include <algorithm> 
//#include <functional> 
//#include <cctype>
//#include <locale>

using namespace std;

struct freqPhAmp{
	int FTW;
	int POW;
	int ASF;
};

class SweepSetup;

class FileInterpreter{
private:
	map<int, freqPhAmp> profiles;
	void cleanProfiles();
	bool sweepOn;
	SweepSetup* sweep;

public:
	// Constructors:
	FileInterpreter(char* filePath);
	~FileInterpreter();

	// Public member variables
	//const double fsysclk;  //double fsysclk = 2.4*pow(10,9); should be constexpr
	static const int fsysclk = 1750000000;

	// Public methods:
	map<int, freqPhAmp> getProfiles();
	static int calculateFTW(double fout);
	static int calculateSweepRiseSize( double subValue );
	void initializeProfiles();
	static int calculatePOW( double phase );
	static int calculateASF( double subValue );
	static int calculateSweepRiseTime( double subValue );
	bool isSweep(void);
	SweepSetup* getSweep();
};


class SweepSetup {
public:
	double Start;
	double Stop;
	double sweepDuration;
	int sweepAmpWord;
	int sweepTimeWord;
	//const double frequencyPrecision = 0.558793544769287; // constexpr test = 2.4*pow(10,9)/pow(2,32); not working in visual studio
	enum sweepType {frequency = 8, phase = 4, amplitude = 3}; // Represent the number of hexidecimals thats available for the specific step type

	sweepType type;
	double risingSize;
	double fallingSize;

	// Constructors
	SweepSetup(sweepType typ = frequency) {
		type = typ;
		sweepAmpWord = FileInterpreter::calculateASF(0.5);
	}

	/* Calculates the integer representation of the rising size.
	Requirements:
	- The end of the ramp should be higher than the start.
	- The riseSize should not exceed the difference between the start and the end of the ramp.
	*/
	int calculateSweepSize() {
		double delta = fabs(Start - Stop);
		double bestFreq = bestFraction();
		assert(bestFreq < delta);
		return round(pow(2,type * 4) * bestFreq / ((double) FileInterpreter::fsysclk)); //This sysclk should be moved!
	}


	/* Functions that make sure to return the largest value of the frequencies for stop and lowest for start */
	double start() {
		return (Stop > Start)? Start : Stop;
	}

	double stop() {
		return (Start > Stop)? Start : Stop;
	}

private:
	// TODO: Make dynamic for phase and amp sweep!
	double bestFraction() {
		double gradient = fabs(Stop - Start) / sweepDuration;// * 2400000000 / 24;
		// cout << "gadient: " << gradient << endl;
		double freqMax = ((double) FileInterpreter::fsysclk); //Should be constexpr up in private members
		double testFreq = 0;
		double oldError = numeric_limits<double>::infinity();

		double idealGrad;

		double bestFreq;
		double testGrad;
		double gradError = numeric_limits<double>::infinity();
		int deltaT;
		while(testFreq <= freqMax  && fabs(idealGrad/gradient - 1) > 0.001) {
			testFreq += ((double) FileInterpreter::fsysclk)/pow(2.0,32);
			deltaT = round(sweepDuration / fabs(Stop - Start) * testFreq * ((double) FileInterpreter::fsysclk) / 24.0);

			testGrad = testFreq / ((double) deltaT * 24.0 / (double) FileInterpreter::fsysclk);

			gradError = fabs(testGrad - gradient);

			if (gradError < oldError) {
				oldError =  gradError;
				sweepTimeWord = deltaT;
				bestFreq = testFreq;
				idealGrad = testGrad;
			}
		}

		cout << "Ratio of grad - 1: " << idealGrad/gradient - 1 << endl
			<< "Frequency step size: " << testFreq << " Hz" << endl
			<< "Slope rate: " << 24/((double) FileInterpreter::fsysclk) * sweepTimeWord * pow(10.0,6) << " "  << char(230) << "s" << endl; 
		return bestFreq;
	}
};

#endif // !FileInterpreter_h