#include "ADController.h"

#include <stdexcept>
#include <iostream>
#include <windows.h>
#include <fstream>
#include <sstream>

using namespace std;

BYTE ADController::portConfig[] = {0x7f, 0xff, 0x00, 0xff};//int ADController::portConfig[] = {0x7F,0xFF,0x00,0xFF};
BYTE ADController::portInitValues[] = {0x82, 0xf0, 0xff, 0x01};

ADController::ADController(void)
{
	HMODULE hUser32 = LoadLibrary(L"adiddseval.dll");

	if (!hUser32) throw new runtime_error("Missing dll");

	_fFindHardware = (_FindHardware) GetProcAddress(hUser32, "FindHardware");
	_fGetHardwareHandles = (_GetHardwareHandles) GetProcAddress(hUser32, "GetHardwareHandles");
	_fIsConnected	= (_IsConnected) GetProcAddress(hUser32, "IsConnected");
	_fGetPortConfig = (_GetPortConfig) GetProcAddress(hUser32, "GetPortConfig");
	_fSetPortConfig = (_SetPortConfig) GetProcAddress(hUser32, "SetPortConfig");
	_fGetPortValue	= (_GetPortValue) GetProcAddress(hUser32, "GetPortValue");
	_fSetPortValue	= (_SetPortValue) GetProcAddress(hUser32, "SetPortValue");
	_fGetSpiInstruction	= (_GetSpiInstruction) GetProcAddress(hUser32, "GetSpiInstruction");

	_fSpiRead	= (_SpiRead) GetProcAddress(hUser32, "SpiRead");
	_fSpiWrite	= (_SpiWrite) GetProcAddress(hUser32, "SpiWrite");


	FindHardware();


	if (!IsConnected()) throw runtime_error("Hardware not connected");

	SetPortConfig();
	//resetAllRegisters("backup.txt");
	// TODO: make reset setup! void reset();
}


ADController::~ADController(void) {	
	FreeLibrary(hUser32);
}

void ADController::FindHardware() {
	cout << "Locating hardware..." << endl;
	const int N = 1;

	int vid = ADController::VID;
	int pid = ADController::PID;

	int count = _fFindHardware(vid, pid, N);
	_fGetHardwareHandles(handle);
	cout << "The handle is:" << handle << endl;
}

bool ADController::IsConnected () {
	cout << "Checking connection...\n";
	cout << "The connection was [" << _fIsConnected(handle) << "] established" << endl;
	return (_fIsConnected(handle) == 1);
	cout << "Check done";
}

void ADController::SetPortConfig() {
	cout << "Setting configuration of ports...\n";

	for (int port = 0 ; port < 4; port++ ) {
		_fSetPortConfig(handle,port,portConfig[port]);
	}
	//for (int port = 0 ; port < 4 ; port++) {
	//	SetPortValue(port, portInitValues[port]);
	//}
	//cout << "Config\tValue" << endl;
	//cout << (int) GetPortConfig(0) << "\t" << (int) GetPortValue(0) << endl;
	//cout << (int) GetPortConfig(1) << "\t" << (int) GetPortValue(1) << endl;
	//cout << (int) GetPortConfig(2) << "\t" << (int) GetPortValue(2) << endl;
	//cout << (int) GetPortConfig(3) << "\t" << (int) GetPortValue(3) << endl;
	cout << "Configuration of ports done.\n";
}

BYTE ADController::GetPortConfig(int port) {

	BYTE value = 0;
	_fGetPortConfig(handle, port, value);
	return value;
}

BYTE ADController::GetPortValue(int port) {
	BYTE value = 0;
	_fGetPortValue(handle, port, value);
	return value;
}

void ADController::SetPortValue( int port , BYTE value)
{
	_fSetPortValue(handle, port, value);
}

BYTE* ADController::ReadRegister(int addr)
{
	int readLength = 4;
	int SpecialReadLength = checkSpecialReg(addr);
	if (SpecialReadLength !=0) readLength = SpecialReadLength;
	BYTE* registerValues = new BYTE[readLength];

	BYTE instruction = 0x00;	
	GetSpiInstruction(1, addr, instruction);

	if (_fSpiRead(handle, instruction, sizeof(instruction), *registerValues, readLength, fourWire) == 0){
		return registerValues;
	} else { 
		return registerValues;
	}
}


void ADController::WriteRegister(int addr, nullByte* Data[], bool update) {
	int writeLength = 4;
	if (sizeof(Data) != writeLength) {
		throw runtime_error("More data needed!");
	}

	if (addr == 0x37) {
		update = false;
	}

	BYTE* readData = ReadRegister(addr);

	BYTE* writeData = new BYTE[writeLength+1];

	//cout << "Data writing reg <0x" << hex << addr << ">\t{ ";
	for (int i = 0; i < writeLength; i++) {
		BYTE temp = (Data[i]->used == 0)? readData[i] : Data[i]->value;
		writeData[i] = temp;
		//cout << hex << (int) writeData[i] << "\t"; 
	} //cout << "}\n";

	BYTE instruction = 0x00;	
	GetSpiInstruction(0, addr, instruction);
	writeData[writeLength] = instruction;

	_fSpiWrite(handle, *writeData, writeLength+1, fourWire);

	if (update) {
		IOUpdate();
	}

	readData = ReadRegister(addr);
	
	//cout << "Restult of reg <0x" << hex << addr << ">:\t{ ";
	for (int i = 0 ; i < sizeof(readData) ; i++) {
		//cout << hex << (int) readData[i] << "\t";
	} //cout << "}" << endl;



}

void ADController::IOUpdate() {
	SetPortValue(0x01, 0xF8);
	SetPortValue(0x01, 0xF0);	
}

void ADController::Sync() {
	nullByte* setupBytes[4];
	
	setupBytes[0] = new nullByte(false);
	setupBytes[1] = new nullByte(false);
	setupBytes[2] = new nullByte(false);
	setupBytes[3] = new nullByte(0xE1);

	WriteRegister(0x03, setupBytes, true);
	setupBytes[3] = new nullByte(0xE0);
	WriteRegister(0x03, setupBytes, true);
	Sleep(0.1);
	//SetPortValue(0x02, 0x00);
	//SetPortValue(0x02, 0x01);
}

int ADController::checkSpecialReg(int addr) {
	return 0;
	//{0x37, 0x3D, 0x3E}
}

void ADController::GetSpiInstruction(int rw,int addr,BYTE &instruction) {
	_fGetSpiInstruction(rw, addr, instruction, sizeof(instruction));
}

void ADController::resetAllRegisters(const char* filePath) {
	cout << "Resetting all registers..." << endl;
	for (int i = 0 ; i < 1 ; i++) { // TODO find registry that should ready this for writing!
		SetPortValue(0, 0x7);
		Sleep(10);
		SetPortValue(0, 0x3);

		ifstream file(filePath);
		string   line;

		if(file.fail()) {
			cout << "Filepath: " << filePath << endl;
			throw invalid_argument("Something is wrong with the file!");
		}

		int reg;
		int oldReg = 0;
		int part;
		int value;
		nullByte* valueArray[4];
		while(getline(file, line)) {

			if (line[0] == '-') break;

			stringstream   linestream(line);

			linestream >> reg;
			linestream >> part;
			linestream >> value;

			if (oldReg != reg) {

				/*//cout << "Try to write:\t{ ";
				for (int i = 0 ; i < sizeof(*valueArray) ; i++) {
					//cout << (int) valueArray[i]->value << "\t";
				} //cout << "} \tto register: " << oldReg << "\n";
*/
				WriteRegister(oldReg, valueArray,1);

				BYTE* test = ReadRegister(oldReg);


				oldReg = reg;
				valueArray[part] = new nullByte(value);

			} else {
				valueArray[part] = new nullByte(value);
			}
		}
		WriteRegister(reg,valueArray,1);
	}
	Sync();
	cout << "done" << endl;
}

void ADController::getBackup(char* filePath, bool hexed) {
	cout << "Beginning backup..." << endl;
	ofstream wFile;
	wFile.open(filePath);

	for (int i = 0 ; i <  0x1C ; i++) {
		//cout << "Backing up register " << i << endl;
		BYTE* registerValues = ReadRegister(i);
		for (int j = 0 ; j < sizeof(registerValues); j++) {
			if (hexed) {
			wFile << i << "\t" << j << "\t0x" << hex << int(registerValues[j]) << endl;
			} else {
			wFile<< i << "\t" << j << "\t" << (int) registerValues[j] << endl;				
			}
		}
	}
	wFile.close();
	if (hexed) {
	cout << "Done backing up in hex" << endl;
	} else {
	cout << "Done backing up in integers" << endl;
	}
}