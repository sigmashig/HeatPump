#include "DeviceManager.h"

#include "EEPROM_Structure.h"
#include "SigmaEEPROM.h"
#include "Configuration.h"

extern Configuration Config;

void DeviceManager::UnitLoop(unsigned long timeperiod) {
	PressureSwitch.UnitLoop(timeperiod);
	VoltageSwitch.UnitLoop(timeperiod);
	Bus.UnitLoop(timeperiod);

	for (int i = 0; i < CONFIG_NUMBER_THERMO; i++) {
		AllThermo[i]->UnitLoop(timeperiod);
	}
}

void DeviceManager::updateRelayEquipment(int number, const char* payload) {
	byte pin = AllRelays[number]->Pin;
	byte lhOn = AllRelays[number]->lhOn;
	AllRelays[number]->UpdateEquipment(payload);

	if (AllRelays[number]->Pin != pin || AllRelays[number]->lhOn != lhOn) {
		SigmaEEPROM::Write8(EEPROM_ADDR_RELAY + number * EEPROM_LEN_RELAY, AllRelays[number]->Pin);
		SigmaEEPROM::Write8(EEPROM_ADDR_RELAY + number * EEPROM_LEN_RELAY + 1, AllRelays[number]->lhOn);
	}
	AllRelays[number]->InitUnit();
}

void DeviceManager::updateBusEquipment(const char* payload) {
	byte pin = Bus.Pin;
	Bus.UpdateEquipment(payload);
	if (Bus.Pin != pin) {
		Config.Log->Debug("EEPROM BUS");
		SigmaEEPROM::Write8(EEPROM_ADDR_TBUS_PIN, Bus.Pin);
		Bus.InitUnit();
	}
}

void DeviceManager::updateThermoEquipment(int number, const char* payload) {
	DeviceAddress da;
	OneWireBus::CopyDeviceAddress(da, AllThermo[number]->Address);
	double eLow = AllThermo[number]->ErrorLow;
	double eHigh = AllThermo[number]->ErrorHigh;
	double wHigh = AllThermo[number]->WarningHigh;
	double wLow = AllThermo[number]->WarningLow;
	AllThermo[number]->UpdateEquipment(payload);
	if (OneWireBus::CompareDeviceAddress(AllThermo[number]->Address, da) != 0
		|| eLow != AllThermo[number]->ErrorLow || eHigh != AllThermo[number]->ErrorHigh
		|| wLow != AllThermo[number]->WarningLow || wHigh != AllThermo[number]->WarningHigh
		) {

		for (int j = 0; j < 8; j++) {
			SigmaEEPROM::Write8(EEPROM_ADDR_THERM + number * EEPROM_LEN_THERM + j, AllThermo[number]->Address[j]);
		}
		SigmaEEPROM::Write16(EEPROM_ADDR_THERM + number * EEPROM_LEN_THERM + 8, (int)(AllThermo[number]->ErrorHigh * 2));
		SigmaEEPROM::Write16(EEPROM_ADDR_THERM + number * EEPROM_LEN_THERM + 8 + 2, (int)(AllThermo[number]->ErrorLow * 2));
		SigmaEEPROM::Write16(EEPROM_ADDR_THERM + number * EEPROM_LEN_THERM + 8 + 4, (int)(AllThermo[number]->WarningHigh * 2));
		SigmaEEPROM::Write16(EEPROM_ADDR_THERM + number * EEPROM_LEN_THERM + 8 + 6, (int)(AllThermo[number]->WarningLow * 2));
	}
	AllThermo[number]->InitUnit();
}

void DeviceManager::updateContactorEquipment(int number, const char* payload) {
	byte pin = AllContactor[number]->Pin;
	byte lhOn = AllContactor[number]->lhOn;
	AllContactor[number]->UpdateEquipment(payload);
	if (AllContactor[number]->Pin != pin || AllContactor[number]->lhOn != lhOn) {
		Config.Log->Debug("EEPROM CONTACTOR");
		SigmaEEPROM::Write8(EEPROM_ADDR_CONTACTOR + number * 2, AllContactor[number]->Pin);
		SigmaEEPROM::Write8(EEPROM_ADDR_CONTACTOR + number * 2 + 1, AllContactor[number]->lhOn);
	}
	AllContactor[number]->InitUnit();
}

void DeviceManager::updateRelayStatus(const char* name, const char* payload) {
	for (int i = 0; i < CONFIG_NUMBER_RELAYS; i++) {
		if (strcmp(AllRelays[i]->Name, name) == 0) {
			byte p = atoi(payload);
			AllRelays[i]->ProcessUnit((ActionType)p);
			break;
		}
	}
}

void DeviceManager::updateThermoStatus(const char* name, const char* payload) {
	for (int i = 0; i < CONFIG_NUMBER_THERMO; i++) {
		if (strcmp(AllThermo[i]->Name, name) == 0) {
			double t = atof(payload);
			AllThermo[i]->SetTemp(t);
			break;
		}
	}
}

void DeviceManager::updateContactorStatus(const char* name, const char* payLoad) {
	ActionType p = (ActionType)(atoi(payLoad));
	if (strcmp(PressureSwitch.Name, name) == 0) {
		PressureSwitch.ProcessUnit(p);
	} else if (strcmp(VoltageSwitch.Name, name) == 0) {
		VoltageSwitch.ProcessUnit(p);
	}
}

DeviceManager::DeviceManager() {
	int j = 0;
	for (int i = 0; i < CONFIG_NUMBER_RELAYS; i++) {
		AllDevices[j] = AllRelays[i];
		j++;
	}
	AllDevices[j] = &Bus;
	j++;
	for (int i = 0; i < CONFIG_NUMBER_THERMO; i++) {

		AllDevices[j] = AllThermo[i];
		j++;
	}
	for (int i = 0; i < CONFIG_NUMBER_CONTACTORS; i++) {

		AllDevices[j] = AllContactor[i];
		j++;
	}
	readFromEEPROM();
}

bool DeviceManager::readFromEEPROM() {
	bool res = false;

	for (int i = 0; i < CONFIG_NUMBER_RELAYS; i++) {
		AllRelays[i]->Pin = SigmaEEPROM::Read8(EEPROM_ADDR_RELAY + i * EEPROM_LEN_RELAY);
		AllRelays[i]->lhOn = SigmaEEPROM::Read8(EEPROM_ADDR_RELAY + i * EEPROM_LEN_RELAY + 1);
	}

	PressureSwitch.Pin = SigmaEEPROM::Read8(EEPROM_ADDR_CONTACTOR + 0);
	PressureSwitch.lhOn = SigmaEEPROM::Read8(EEPROM_ADDR_CONTACTOR + 1);

	VoltageSwitch.Pin = SigmaEEPROM::Read8(EEPROM_ADDR_CONTACTOR + 2);
	VoltageSwitch.lhOn = SigmaEEPROM::Read8(EEPROM_ADDR_CONTACTOR + 2 + 1);

	Bus.Pin = SigmaEEPROM::Read8(EEPROM_ADDR_TBUS_PIN);
	
	for (int i = 0; i < CONFIG_NUMBER_THERMO; i++) {
		for (int j = 0; j < 8; j++) {
			AllThermo[i]->Address[j] = SigmaEEPROM::Read8(EEPROM_ADDR_THERM + i * EEPROM_LEN_THERM + j);
		}
		AllThermo[i]->ErrorHigh = ((int16_t)(SigmaEEPROM::Read16(EEPROM_ADDR_THERM + i * EEPROM_LEN_THERM + 8))) / 2.0;
		AllThermo[i]->ErrorLow = ((int16_t)(SigmaEEPROM::Read16(EEPROM_ADDR_THERM + i * EEPROM_LEN_THERM + 8 + 2))) / 2.0;
		AllThermo[i]->WarningHigh = ((int16_t)(SigmaEEPROM::Read16(EEPROM_ADDR_THERM + i * EEPROM_LEN_THERM + 8 + 4))) / 2.0;
		AllThermo[i]->WarningLow = ((int16_t)(SigmaEEPROM::Read16(EEPROM_ADDR_THERM + i * EEPROM_LEN_THERM + 8 + 6))) / 2.0;
	}
	return res;
}

bool DeviceManager::Init() {
	bool res = true;
	return res;
}

void DeviceManager::setDefaultStates() {

	PumpFloor1.ProcessUnit(ACT_OFF);
	PumpFloor2.ProcessUnit(ACT_OFF);
	PumpGnd.ProcessUnit(ACT_OFF);
	PumpTankIn.ProcessUnit(ACT_OFF);
	PumpTankOut.ProcessUnit(ACT_OFF);
	Compressor.ProcessUnit(ACT_OFF);

}

bool DeviceManager::FinalInit() {
	
	for (int i = 0; i < CONFIG_NUMBER_OF_DEVICES; i++) {
		AllDevices[i]->InitUnit();
	}
	setDefaultStates();
/*
	for (int i = 0; i < CONFIG_NUMBER_RELAYS; i++) {
		AllRelays[i]->InitUnit();
		AllRelays[i]->Publish();
	}

	PressureSwitch.InitUnit();
	VoltageSwitch.InitUnit();
	Bus.InitUnit();

	for (int i = 0; i < CONFIG_NUMBER_THERMO; i++) {
		AllThermo[i]->InitUnit();
		AllThermo[i]->Publish();
	}
*/
	return true;

}


void DeviceManager::SubscribeEquipment() {
	for (int j = 0; j < CONFIG_NUMBER_OF_DEVICES; j++) {
		AllDevices[j]->SubscribeEquipment();
	}
	Config.Transfer(CONFIG_NUMBER_OF_DEVICES);
}

void DeviceManager::SubscribeStatuses() {
	for (int j = 0; j < CONFIG_NUMBER_OF_DEVICES; j++) {
		AllDevices[j]->SubscribeStatus();
	}
	Config.Transfer(CONFIG_NUMBER_OF_DEVICES);
}

void DeviceManager::UpdateEquipment(DeviceType dType, const char* name, const char* payload) {

	switch (dType) {
	case DEVTYPE_RELAY:
		for (int i = 0; i < CONFIG_NUMBER_RELAYS; i++) {
			if (strcmp(AllRelays[i]->Name, name) == 0) {
				updateRelayEquipment(i, payload);
				break;
			}
		}
		break;
	case DEVTYPE_BUS:
		if (strcmp(Bus.Name, name) == 0) {
			//Config.Log->Debug("POINT1");
			updateBusEquipment(payload);
		}
		break;
	case DEVTYPE_CONTACTOR:
		for (int i = 0; i < CONFIG_NUMBER_CONTACTORS; i++) {
			if (strcmp(AllContactor[i]->Name, name) == 0) {
				updateContactorEquipment(i, payload);
				break;
			}
		}
		break;
	case DEVTYPE_THERMOMETER:
		for (int i = 0; i < CONFIG_NUMBER_THERMO; i++) {
			if (strcmp(AllThermo[i]->Name, name) == 0) {
				updateThermoEquipment(i, payload);
				break;
			}
		}
		break;
	default:
		// do nothing
		break;
	}
}

void DeviceManager::UpdateStatus(DeviceType dType, const char* name, const char* payload) {
	for (int i = 0; i < CONFIG_NUMBER_OF_DEVICES; i++) {
		if (AllDevices[i]->DevType == dType && strcmp(AllDevices[i]->Name, name) == 0) {
			AllDevices[i]->UpdateStatus(payload);
			break;
		}
	}
}
