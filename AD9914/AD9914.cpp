#include "stdafx.h"

#include <windows.h>
#include <iostream>
#include <exception>
#include <string>
#include <iomanip>
#include <fstream>

#include "AD9914Interface.h"
#include "ADController.h"
#include "FileInterpreter.h"
#include "AD9914UuT.h"
#include <math.h>

using namespace std;

BYTE frequencyAddresses[]	= {0x0B, 0x0D, 0x0F, 0x11, 0x13, 0x15, 0x17, 0x19};
BYTE phaseAmpAddresses[]	= {0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A};
BYTE sweepAddresses[]		= {0x04, 0x05, 0x06, 0x07, 0x08};

void testingProfile0(ADController a) {
	nullByte* writing[4];
	for (int i = 0 ; i < sizeof(*writing) ; i++) {
		writing[i] = new nullByte(false);
	}
	BYTE* test	= a.ReadRegister(0x0B); //TODO make exception specific for project;
	if (test[3] == 0) {
		writing[3] = new nullByte(1);
	} else {
		writing[3] = new nullByte(0);
	}

	a.WriteRegister(0x0B, writing);
	test	= a.ReadRegister(0x0B); //TODO make exception specific for project;
}


void writeIntToAdress( int setup, int adress, AD9914Interface& ad9914) {
	nullByte* bytes[4];

	bytes[3] = new nullByte((setup >> 24) & 0xFF);
	bytes[2] = new nullByte((setup >> 16) & 0xFF);
	bytes[1] = new nullByte((setup >> 8) & 0xFF);
	bytes[0] = new nullByte(setup & 0xFF);

	ad9914.WriteRegister(adress, bytes, true);
}


void helpCout() {
	cout
		<< endl
		<< "-backup <filepath> writes a file at destination <filepath> with the values of the registers in integers, these can be reprogrammed to the DDS by using -setup <filepath>." << endl
		<< endl
		<< "-backuphex <filepath> writes a file at destination <filepath> with the values of the DDS registers in hexidecimals, this cannot be reprogrammed to the DDS using -setup." << endl
		<< endl
		<< "-setup <filepath> takes the file at destination <filepath> and programmes this to the DDS-chip. These has to be in integers, see an example by generating an file using the command: -backup <filepath>" << endl
		<< endl
		<< "-uut is a unit-under-test mode, where everything interpreted from the file, which should be programmed to the DDS is instead printed to the console (can be used with the DDS unconnected)." << endl
		<< endl
		<< endl
		<< "Besides this the 'labSetup.txt' file has the initial setup of the DDS-chip which is programmed every time." << endl; //This reprogramming could be removed for reprogramming optimization.
}


void programSweep(AD9914Interface& ad9914, FileInterpreter& fileInt) {
	cout << "Programming sweep..." << endl;
	SweepSetup* sweep = fileInt.getSweep();

	int setupInt = FileInterpreter::calculateFTW(sweep->start());
	writeIntToAdress(setupInt, sweepAddresses[0], ad9914); // The Adress could be carried in an enum for clarity!

	setupInt = FileInterpreter::calculateFTW(sweep->stop());
	writeIntToAdress(setupInt, sweepAddresses[1], ad9914);

	setupInt = sweep->calculateSweepSize();
	writeIntToAdress(setupInt, sweepAddresses[2], ad9914);
	writeIntToAdress(setupInt, sweepAddresses[3], ad9914);

	// Quick and dirty special case:
	setupInt = sweep->sweepTimeWord;
	cout << "SweepTimeWord: " << setupInt << endl;
	nullByte* setupBytes[4];
	setupBytes[3] = new nullByte((setupInt >> 8) & 0xFF);
	setupBytes[2] = new nullByte((setupInt >> 0) & 0xFF);

	setupInt = sweep->sweepTimeWord;
	setupBytes[1] = new nullByte((setupInt >> 8) & 0xFF);
	setupBytes[0] = new nullByte(setupInt & 0xFF);

	ad9914.WriteRegister(sweepAddresses[4], setupBytes, true);



	// For now write same sweep amplitude to all registers (A check should be made whether the sweep amp is related to profile 0 amplitude, theory).
	setupInt = sweep->sweepAmpWord;

	setupBytes[3] = new nullByte((setupInt >> 8) & 0xFF);
	setupBytes[2] = new nullByte(setupInt & 0xFF);
	setupBytes[1] = new nullByte(false);
	setupBytes[0] = new nullByte(false);

	for (int i = 0 ; i < 8 ; i++) {
		ad9914.WriteRegister(phaseAmpAddresses[i], setupBytes, true);
	}

		// Set control function register 2 to enable sweep!
	setupBytes[0] = new nullByte(false);
	setupBytes[1] = new nullByte(false);
	setupBytes[2] = new nullByte(136);
	setupBytes[3] = new nullByte(false);

	ad9914.WriteRegister(0x01, setupBytes, true);

	cout << "Done programming sweep!" << endl;

}


void programProfiles(AD9914Interface& ad9914, FileInterpreter& fileInt) {
	cout << "Programming profiles..." << endl;
	map<int, freqPhAmp> profiles= fileInt.getProfiles();

	nullByte* freqBytes[4];
	nullByte* phAmpBytes[4];
	nullByte* setupBytes[4];

	for (int i = 0; i < 8; i = i+ 1) {
		int freq = profiles[i].FTW;
		int phase = profiles[i].POW;
		int amp = profiles[i].ASF;
		//cout << "\nPr ofile :" << i <<  "\n";
		//cout << "FTW = " << freq << "\tPhAmp = " << phAmp << endl;

		// Shift the byte for frequency to be written
		freqBytes[3] = new nullByte((freq >> 24) & 0xFF); // Make translate to byte refactor
		freqBytes[2] = new nullByte((freq >> 16) & 0xFF);
		freqBytes[1] = new nullByte((freq >> 8) & 0xFF);
		freqBytes[0] = new nullByte(freq & 0xFF);

		// Shift the byte for phase and amplitude to be written
		phAmpBytes[3] = new nullByte((amp >> 8) & 0xFF);
		phAmpBytes[2] = new nullByte((amp >> 0) & 0xFF);
		phAmpBytes[1] = new nullByte((phase >> 8) & 0xFF);
		phAmpBytes[0] = new nullByte(phase & 0xFF);


		////cout << "Data writing reg <0x" << hex << (int) frequencyAddresses[i] << ">\t{ ";
		//for (int j = 0; j < 4; j++) {
		//	//cout << (int) freqBytes[j]->value << "\t"; 
		//} //cout << "}\n";

		ad9914.WriteRegister(frequencyAddresses[i], freqBytes,true);
		ad9914.WriteRegister(phaseAmpAddresses[i], phAmpBytes, true);
	}

	// Set control function register 2 to enable sweep!
	setupBytes[0] = new nullByte(false);
	setupBytes[1] = new nullByte(false);
	setupBytes[2] = new nullByte(128);
	setupBytes[3] = new nullByte(false);

	ad9914.WriteRegister(0x01, setupBytes, true);

	cout << "Done programming profiles!" << endl;
}


int main(int argc, CHAR* argv[]) {
	//Handle arguments backup and setup.
	if (argc == 2 && (string(argv[1]) == "-help" || string(argv[1]) == "help")) {
		helpCout();
	} else if (argc == 3) {
		if (string(argv[1]) == "-backup") {
			try {
				ADController ad9914;
				ad9914.getBackup(argv[2]);
			} catch (exception& e) {
				cout << "EXCEPTION: " << e.what() << endl;
			}
		} else if (string(argv[1]) == "-backuphex") {
			try {
				ADController ad9914;
				ad9914.getBackup(argv[2], true);
			} catch (exception& e) {
				cout << "EXCEPTION: " << e.what() << endl;
			}
		} else if (string(argv[1]) == "-uut") {
			char* filePath = argv[2];	

			try {
				FileInterpreter fileInt(filePath);
				AD9914UuT ad9914;
				string temp(argv[0]);
				int indexToStrip = temp.rfind("\\");
				temp = temp.substr(0,indexToStrip+1);
				temp.append("labSetup.txt");
				ad9914.resetAllRegisters(temp.c_str());

				if (fileInt.isSweep()) {
					programSweep(ad9914, fileInt);
				} else {
					programProfiles(ad9914, fileInt);
				}
			} catch (exception& e) {
				cout << "EXCEPTION: " << e.what() << endl;
				return 1;
			}
			cout << "DONE!" << endl;
		}else if (string(argv[1]) == "-setup") {
			char* filePath = argv[2];	

			try {
				FileInterpreter fileInt(filePath);
				ADController ad9914;
				string temp(argv[0]);
				int indexToStrip = temp.rfind("\\");
				temp = temp.substr(0,indexToStrip+1);
				temp.append("labSetup.txt");
				ad9914.resetAllRegisters(temp.c_str());

				if (fileInt.isSweep()) {
					programSweep(ad9914, fileInt);
				} else {
					programProfiles(ad9914, fileInt);
				}
			} catch (exception& e) {
				cout << "EXCEPTION: " << e.what() << endl;
				return 1;
			}
			cout << "DONE!" << endl;
		} else {
			cout << "Something went wront try with argument -help";
		}
	}
	//string pause;
	//getline(cin, pause);
	return 0;
}