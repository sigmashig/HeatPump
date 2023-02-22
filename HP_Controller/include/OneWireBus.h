#pragma once
#include "Unit.h"
#include <OneWire.h>
#include "DallasTemperature.h"
#include "SigmaEEPROM.h"

#define BUS_RESOLUTION 9
#define BUS_INTERVAL  2000

class OneWireBus :
	public Unit
{
public:

	void InitUnit();
	void UnitLoop(unsigned long timePeriod);
	void FinalInitUnit() {};

	void ProcessUnit(ActionType action);
	double GetTemperature(const DeviceAddress address);
	bool CheckAddress(const DeviceAddress address);
	void SetResolution(const DeviceAddress address);
	void RequestTemperature();
	static bool CompareDeviceAddress(DeviceAddress a0, DeviceAddress a1);
	static void CopyDeviceAddress(DeviceAddress dest, DeviceAddress src);
	static void ConvertStringToAddress(DeviceAddress address, const char* addrStr);
	static String ConvertAddressToString(const DeviceAddress address);
	static bool IsZeroAddress(DeviceAddress address);
	void const print(const char* header, DebugLevel level);
	OneWireBus(const char* nm);
	bool IsSimulator();
private:
	OneWire* oneWire;
	int numberThermo = 0;
	DallasTemperature *sensors;
	static byte hexToDec(char c1, char c2);
	static byte hexToDec(char c1);

//	unsigned long prevCycle = 0;
};

