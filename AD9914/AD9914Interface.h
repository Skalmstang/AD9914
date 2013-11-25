#pragma once

#include <windows.h>
#include <vector>
#include <iomanip>
//WinDef.h for BYTE

using namespace std;

struct nullByte
{
	nullByte(void){used = false; value = 0;}
	nullByte(int val){value = val; used = true;}
	nullByte(unsigned long val){value = (BYTE) val; used = true;}
	nullByte(bool use){used = use; value = 0; }
	BYTE value;
	bool used;
};

class AD9914Interface
{
public:
	/*AD9914Interface(void); */
	~AD9914Interface(void) {};
	virtual void findHardware() {};
	virtual bool IsConnected() {return false;};
	virtual void WriteRegister(int addr, nullByte* Data[], bool update) {};
	virtual void resetAllRegisters(const char* rangePath) {};
	virtual void getBackup(const char* backupPath, bool hex = false) {};
};

