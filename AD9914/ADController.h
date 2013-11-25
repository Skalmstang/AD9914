#pragma once

#include "AD9914Interface.h"

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

class ADController : public AD9914Interface{
public:
	ADController(void);
	~ADController(void);

	BYTE	GetPortValue(int port);
	void	SetPortValue(int port, BYTE Data);
	BYTE	GetPortConfig(int port);
	void	GetSpiInstruction(int rw, int addr,BYTE &instruction);
	BYTE*	ReadRegister(int addr);
	void	WriteRegister(int addr, nullByte* value[], bool update = true);
	void	getBackup(char* rangePath, bool hexed = false);
	void	resetAllRegisters(const char* rangePath);


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
	void	Sync();
	void	IOUpdate();

	int checkSpecialReg(int reg);


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

