#pragma once

#include <Arduino.h>
#include "definitions.h"
#include "Loger.h"
#include <ArduinoJson.h>



class Unit
{
public:

	byte Status = 0xFF;
	byte Pin;
	const char* Name;
	DeviceType DevType;
	virtual bool IsSimulator() = 0;
	void PublishDeviceAlert(ALERTCODE code, bool force=false);
	void Publish();
	void SubscribeEquipment();
	void SubscribeStatus();
	virtual void UpdateEquipment(const char* payload) = 0;
	virtual void UpdateStatus(const char* payload) = 0;
	virtual void UnitLoop(unsigned long timePeriod) = 0;
	virtual void InitUnit() = 0;
	
protected:

	ALERTCODE alertCode = ALERT_EMPTY;
	virtual ~Unit() {};
	virtual void const print(const char* header, DebugLevel level);
	Unit(DeviceType dType, const char* nm) { Name = nm; DevType = dType; }
};

