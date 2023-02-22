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

	void ProcessUnit(ActionType event);
	void const print(const char* header, DebugLevel level);
	void UpdateRelay(const char* line);

	Relay(const char* nm);
private:
	void RelaySet(bool highLow);
	void RelaySwitch();
	void RelayOn() { RelaySet(HIGH); };
	void RelayOff() { RelaySet(LOW); };
	unsigned long delayStart;
};




