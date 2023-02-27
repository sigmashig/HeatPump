#pragma once

#include <Arduino.h>
#include "definitions.h"
#include "Loger.h"
#include <ArduinoJson.h>



class Unit
{
public:

	byte status = 0xFF;
	byte Pin;
	const char* Name;
//	bool IsAlert = false;
	virtual bool IsSimulator() = 0;

protected:

	ALERTCODE alertCode = ALERT_EMPTY;
	void Publish(const char* uPrefix);
	virtual void UnitLoop(unsigned long timePeriod) = 0;
	virtual void InitUnit() = 0;
	virtual ~Unit() {};
	virtual void const print(const char* header, DebugLevel level);
	Unit(const char* nm) { Name = nm; }
	void publishDeviceAlert(ALERTCODE code);
};
