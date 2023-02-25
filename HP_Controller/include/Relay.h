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
	void UpdateRelay(const char* line);
	bool IsOk();

	Relay(const char* nm);
private:
	bool relaySet(bool highLow);
	bool relaySwitch();
	bool relayOn();
	bool relayOff();
	unsigned long delayStart;

	unsigned int minTimeOn = NAN;
	unsigned int maxTimeOn = NAN;
	unsigned int minTimeOff = NAN;
	unsigned int maxTimeOff = NAN;

	unsigned long lastOn = 0;
	unsigned long lastOff = 0;

};
