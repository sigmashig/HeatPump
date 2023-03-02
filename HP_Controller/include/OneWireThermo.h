#pragma once
#include <DallasTemperature.h>
#include "Unit.h"
#include "OneWire.h"
#include "OneWireBus.h"

class OneWireThermo :
	public Unit
{
public:
	double Temperature;
	DeviceAddress Address;
	double ErrorLow;
	double ErrorHigh;
	double WarningLow;
	double WarningHigh;

	void const print(const char* header, DebugLevel level) override;
	OneWireThermo(const char* nm);
	void SetTemp(float temp) { Temperature = temp; }
	bool IsSimulator(){ return isSimulator; };

	void UnitLoop(unsigned long timeperiod);
	void InitUnit();
	float GetTemperature();
	void UpdateThermo(const char* line);
	bool IsOk();
private:
	OneWireBus* parent;
	bool isSimulator = true;
	bool checkSimulator();
};

