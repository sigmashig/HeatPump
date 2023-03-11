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

	Contactor* AllContactor[CONFIG_NUMBER_CONTACTORS] = { &PressureSwitch, &VoltageSwitch };
	OneWireBus Bus = OneWireBus("Bus");

	OneWireThermo TGndIn = OneWireThermo("TGndIn");
	OneWireThermo TGndOut = OneWireThermo("TGndOut");
	OneWireThermo TCompressor = OneWireThermo("TCompressor");
	OneWireThermo TCondIn = OneWireThermo("TCondIn");
	OneWireThermo TCondOut = OneWireThermo("TCondOut");
	OneWireThermo TVapIn = OneWireThermo("TVapIn");
	OneWireThermo TVapOut = OneWireThermo("TVapOut");
	OneWireThermo TOut = OneWireThermo("TOut");
	OneWireThermo TIn = OneWireThermo("TIn");
	OneWireThermo TTankOut = OneWireThermo("TTankOut");
	OneWireThermo TTankIn = OneWireThermo("TTankIn");
	OneWireThermo TInside = OneWireThermo("TInside");
	OneWireThermo TOutside = OneWireThermo("TOutside");

	OneWireThermo* AllThermo[CONFIG_NUMBER_THERMO] = { &TGndIn, &TGndOut, &TCompressor, &TVapOut, &TCondIn, &TCondOut, &TVapIn, &TOut, &TIn, &TTankOut, &TTankIn, &TInside, &TOutside };
	
	Unit* AllDevices[CONFIG_NUMBER_OF_DEVICES];


	void UnitLoop(unsigned long timeperiod);
	DeviceManager();
	bool Init();
	bool FinalInit();

//	void PublishEquipment();

	void SubscribeEquipment();

	void SubscribeStatuses();
	void UpdateEquipment(DeviceType dType, const char* name, const char* payload);
    void UpdateStatus(DeviceType dType, const char* name, const char* payload);

private:
//	int numbInitialized = 0;
	void setDefaultStates();
	void updateRelayEquipment(int number, const char* payload);
	void updateBusEquipment(const char* payload);
	void updateThermoEquipment(int number, const char* payload);
	void updateContactorEquipment(int number, const char* payload);
	void updateRelayStatus(const char* name, const char* payLoad);
	void updateContactorStatus(const char* name, const char* payLoad);
	void updateThermoStatus(const char* name, const char* payLoad);
	bool readFromEEPROM();

};

