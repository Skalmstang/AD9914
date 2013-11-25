#pragma once

#include "AD9914Interface.h"
#include <iostream>

using namespace std;

class AD9914UuT : public AD9914Interface {
public:
	AD9914UuT(void);
	~AD9914UuT(void);

	bool IsConnected() {
		return true;
	}

	void WriteRegister(int addr, nullByte* Data[], bool update) {
		for (int i = 0 ; i < 4 ; i++) {
			if (Data[i]->used) {
		cout << hex << addr << "\t" << i << "\t0x" << hex << int(Data[i]->value) << endl;
			}
		}
	}
};

