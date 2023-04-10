// Contactor.h
#pragma once


#include "Unit.h"

#define CONTACTOR_SWITCHED_TIME 100

class Contactor:public Unit
{
public:
	bool lhOn;

	void InitUnit();
	void UnitLoop(unsigned long timePeriod);
	void FinalInitUnit();
	bool IsSimulator();

	void ProcessUnit(ActionType event);
	void const print(const char* header, DebugLevel level);
	Contactor(const char* nm);
    bool UpdateEquipment(const char* line);
    void UpdateStatus(const char* payload);
    void PublishDefaultEquipment();
	bool IsOk();
private:
	unsigned long startContact=0;
	byte prevValue;

	void handleContactor(unsigned long timePeriod);
	void handleFinish(int newStatus);
};
