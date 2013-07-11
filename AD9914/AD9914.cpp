#include "stdafx.h"

#include <windows.h>
#include <iostream>
#include <exception>
#include <string>
#include <iomanip>
#include <fstream>
#include "ADController.h"
#include "FileInterpreter.h"
#include <math.h>



using namespace std;


BYTE frequencyAddresses[]	= {0x0B, 0x0D, 0x0F, 0x11, 0x13, 0x15, 0x17, 0x19};
BYTE phaseAmpAddresses[]	= {0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A};

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

	//cout << "{ ";
	for (int i = 0 ; i < sizeof(test) ; i++) {
		//cout << (int) test[i] << " ";
	} //cout << "}\n";
}

int main(int argc, CHAR* argv[]) {
	char* filePath;	

	if (argc > 1) {
		filePath = argv[1];
	} else {
		cout << "No file was specified for program";
		return 1;
	}

	FileInterpreter fileInt(filePath);
	if (false) {

	} else {

		map<int, freqPhAmp> profiles= fileInt.getProfiles();
		try {
			ADController ad9914;
			//ad9914.getBackup("labSetup.txt");
			ad9914.resetAllRegisters("labSetup.txt");

			nullByte* freqBytes[4];
			nullByte* phAmpBytes[4];
			for (int i = 0; i < 8; i = i+ 1)
			{
				int freq = profiles[i].FTW;
				int phase = profiles[i].POW;
				int amp = profiles[i].ASF;
				//cout << "\nPr ofile :" << i <<  "\n";
				//cout << "FTW = " << freq << "\tPhAmp = " << phAmp << endl;

				// Shift the byte for frequency to be written
				freqBytes[3] = new nullByte((freq >> 24) & 0xFF);
				freqBytes[2] = new nullByte((freq >> 16) & 0xFF);
				freqBytes[1] = new nullByte((freq >> 8) & 0xFF);
				freqBytes[0] = new nullByte(freq & 0xFF);

				// Shift the byte for phase and amplitude to be written
				phAmpBytes[3] = new nullByte((phase >> 8) & 0xFF); // Make translate to byte refactor
				phAmpBytes[2] = new nullByte((phase >> 0) & 0xFF);
				phAmpBytes[1] = new nullByte((amp >> 8) & 0xFF);
				phAmpBytes[0] = new nullByte(amp & 0xFF);


				////cout << "Data writing reg <0x" << hex << (int) frequencyAddresses[i] << ">\t{ ";
				//for (int j = 0; j < 4; j++) {
				//	//cout << (int) freqBytes[j]->value << "\t"; 
				//} //cout << "}\n";

				ad9914.WriteRegister(frequencyAddresses[i], freqBytes,true);
				//ad9914.WriteRegister(phaseAddresses[i], phAmpBytes, true);
			}
		} catch (exception& e) {
			cout << "EXCEPTION: " << e.what() << endl;
			return 1;
		}
	}
	cout << "DONE!" << endl;


	//ad9914.getBackup("testReset.txt");
	//ad9914.WriteRegister(0x0F, nullByte(profiles[1].longs[1]),true);



	//string pause;
	//getline(cin, pause);
	return 0;

}

