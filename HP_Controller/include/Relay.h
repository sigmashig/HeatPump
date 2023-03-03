// relay.h
#pragma once
#include <Arduino.h>
#include "Unit.h"

class Relay : public Unit
{
public:
	bool lhOn;

	void InitUnit();
	void UnitLoop(unsigned long timePeriod);
	bool IsSimulator();

	bool ProcessUnit(ActionType event);
	void const print(const char* header, DebugLevel level);
	void UpdateEquipment(const char* line);
	bool IsOk();
	void UpdateStatus(const char* payload);

	Relay(const char* nm);
private:
	bool relaySet(bool highLow);
	bool relaySwitch();
	bool relayOn();
	bool relayOff();
	//unsigned long delayStart;

	/*
	unsigned int minTimeOn = (unsigned long)NAN;
	unsigned int maxTimeOn = (unsigned long)NAN;
	unsigned int minTimeOff = (unsigned long)NAN;
	unsigned int maxTimeOff = (unsigned long)NAN;

	unsigned long lastOn = 0;
	unsigned long lastOff = 0;
*/
};
