#pragma once

#include "Relay.h"
#include "Contactor.h"
#include "OneWireBus.h"
#include "OneWireThermo.h"

class DeviceManager
{
public:

	Relay PumpGnd = Relay("PumpGnd");
	Relay Compressor = Relay("Compressor");
	Relay PumpTankIn = Relay("PumpTankIn");
	Relay PumpTankOut = Relay("PumpTankOut");
	Relay PumpFloor1 = Relay("PumpFloor1");
	Relay PumpFloor2 = Relay("PumpFloor2");

	Relay* AllRelays[CONFIG_NUMBER_RELAYS] = { &PumpGnd, &Compressor, &PumpTankIn, &PumpTankOut, &PumpFloor1, &PumpFloor2 };

	Contactor PressureSwitch = Contactor("PressureSwitch");
	Contactor VoltageSwitch = Contactor("VoltageSwitch");

	OneWireBus Bus = OneWireBus("Bus");

	OneWireThermo TGndIn = OneWireThermo("TGndIn");
	OneWireThermo TGndOut = OneWireThermo("TGndOut");
	OneWireThermo TCompressor = OneWireThermo("TCompressor");
	OneWireThermo TVapOut = OneWireThermo("TVapOut");
	OneWireThermo TCondIn = OneWireThermo("TCondIn");
	OneWireThermo TCondVap = OneWireThermo("TCondVap");
	OneWireThermo TOut = OneWireThermo("TOut");
	OneWireThermo TIn = OneWireThermo("TIn");
	OneWireThermo TTankOut = OneWireThermo("TTankOut");
	OneWireThermo TTankIn = OneWireThermo("TTankIn");
	OneWireThermo TInside = OneWireThermo("TInside");
	OneWireThermo TOutside = OneWireThermo("TOutside");

	OneWireThermo* AllThermo[CONFIG_NUMBER_THERMO] = {&TGndIn, &TGndOut, &TCompressor, &TVapOut, &TCondIn, &TCondVap, &TOut, &TIn, &TTankOut, &TTankIn, &TInside, &TOutside};
	


	void UnitLoop(unsigned long timeperiod);
	void UpdateRelayEquipment(const char* name, const char* payLoad);
	void UpdateRelayStatus(const char* name, const char* payLoad);
	void UpdateContactorEquipment(const char* name, const char* payLoad);
	void UpdateContactorStatus(const char* name, const char* payLoad);
	void UpdateThermoEquipment(const char* name, const char* payLoad);
	void UpdateThermoStatus(const char* name, const char* payLoad);
	DeviceManager();
	bool readFromEEPROM();
	bool Init();
	bool FinalInit();

	void PublishEquipment();

	void SubscribeEquipment();

	void SubscribeStatuses();

	void UpdateEquipment(const char* topic, const char* payload);

	void UpdateStatuses(const char* topic, const char* payload);

//	int IncreaseNumberOfDevices() { return ++numbInitialized; };

private:
//	int numbInitialized = 0;
//	Unit* AllDevices[CONFIG_NUMBER_OF_DEVICES];

};

