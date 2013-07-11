#pragma once
#include <windows.h>
#include <vector>
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

#ifndef _ADController_TYPEDEF
#define _ADController_TYPEDEF
typedef int (_stdcall *_FindHardware)(int&, int&, int);
typedef int (_stdcall *_GetHardwareHandles)(int&);
typedef int (_stdcall *_IsConnected)(int);
typedef int (_stdcall *_GetPortConfig)(int, int, BYTE&);
typedef int (_stdcall *_SetPortConfig)(int, int, BYTE);
typedef int (_stdcall *_GetPortValue)(int, int, BYTE&);
typedef int (_stdcall *_SetPortValue)(int, int, BYTE);
typedef int (_stdcall *_GetSpiInstruction)(int, int, BYTE&, int);
typedef int (_stdcall *_SpiRead)(int, BYTE&, int, BYTE&, int, int);
typedef int (_stdcall *_SpiWrite)(int, BYTE&, int, int);
#endif

class ADController {
public:
	ADController(void);
	~ADController(void);

	BYTE	GetPortValue(int port);
	void	SetPortValue(int port, BYTE Data);
	BYTE	GetPortConfig(int port);
	void	GetSpiInstruction(int rw, int addr,BYTE &instruction);
	BYTE*	ReadRegister(int addr);
	void	WriteRegister(int addr, nullByte* value[], bool update = true);
	void	getBackup(char* rangePath);
	void	resetAllRegisters(char* rangePath);



private:
	static const int PID = 0xEE1F;
	static const int VID = 0x0456;
	static BYTE portConfig[4];
	static BYTE portInitValues[4];
	HMODULE hUser32;

	int handle;
	static const int fourWire = 0;



	void	FindHardware();
	bool	IsConnected ();
	void	SetPortConfig();


	int checkSpecialReg(int reg);
	void IOUpdate();

	_FindHardware _fFindHardware;
	_GetHardwareHandles _fGetHardwareHandles;
	_IsConnected _fIsConnected;
	_GetPortConfig _fGetPortConfig;
	_SetPortConfig _fSetPortConfig;
	_GetPortValue _fGetPortValue;
	_SetPortValue _fSetPortValue;
	_GetSpiInstruction _fGetSpiInstruction;
	_SpiRead _fSpiRead;
	_SpiWrite _fSpiWrite;
};

