#include "DeviceManager.h"

#include "EEPROM_Structure.h"
#include "SigmaEEPROM.h"
#include "Configuration.h"

extern Configuration Config;

void DeviceManager::UnitLoop(unsigned long timeperiod)
{
	PressureSwitch.UnitLoop(timeperiod);
	VoltageSwitch.UnitLoop(timeperiod);
	/* //# check contactor simulator
		Bus.UnitLoop(timeperiod);

		for (int i = 0; i < CONFIG_NUMBER_THERMO; i++) {
			AllThermo[i]->UnitLoop(timeperiod);
		}
		*/
}

void DeviceManager::UpdateRelayEquipment(const char* name, const char* payload)
{
	for (int i = 0; i < CONFIG_NUMBER_RELAYS; i++) {
		if (strcmp(AllRelays[i]->Name,name)==0) {
			byte pin = AllRelays[i]->Pin;
			byte lhOn = AllRelays[i]->lhOn;
			AllRelays[i]->UpdateRelay(payload);

			if (AllRelays[i]->Pin != pin || AllRelays[i]->lhOn != lhOn) {
				Config.Log->append("EEPROM RELAY:").append("pin=").append(AllRelays[i]->Pin).append("; lhOn=").append(AllRelays[i]->lhOn).Debug();
				SigmaEEPROM::Write8(EEPROM_ADDR_RELAY + i * EEPROM_LEN_RELAY, AllRelays[i]->Pin);
				SigmaEEPROM::Write8(EEPROM_ADDR_RELAY + i * EEPROM_LEN_RELAY + 1, AllRelays[i]->lhOn);
			}
			AllRelays[i]->InitUnit();
			break;

		}
	}
}

void DeviceManager::UpdateThermoEquipment(const char* name, const char* payload)
{
	if (strcmp(name, Bus.Name) == 0) {
		byte p = atoi(payload);
		if (Bus.Pin != p) {
			Config.Log->Debug("EEPROM BUS");
			SigmaEEPROM::Write8(EEPROM_ADDR_TBUS_PIN, p);
		}
		Bus.Pin = p;
		Bus.InitUnit();
	}
	else {
		for (int i = 0; i < CONFIG_NUMBER_THERMO; i++) {
			if (strcmp(AllThermo[i]->Name, name) == 0) {
				DeviceAddress da;
				OneWireBus::CopyDeviceAddress(da, AllThermo[i]->Address);
				double minT = AllThermo[i]->MinTemp;
				double maxT = AllThermo[i]->MaxTemp;

				AllThermo[i]->UpdateThermo(payload);
				/*
				Config.Log->append("Thermo:").append(OneWireBus::CompareDeviceAddress(AllThermo[i]->Address, da))
					.append("; min=").append(AllThermo[i]->MinTemp).append("#").append(minT)
					.append("; max=").append(AllThermo[i]->MaxTemp).append("#").append(maxT).Debug();
					*/
				if (OneWireBus::CompareDeviceAddress(AllThermo[i]->Address, da) != 0
					|| minT != AllThermo[i]->MinTemp
					|| maxT != AllThermo[i]->MaxTemp) {
					Config.Log->Debug("EEPROM THERMO");

					for (int j = 0; j < 8; j++) {
						SigmaEEPROM::Write8(EEPROM_ADDR_THERM + i * EEPROM_LEN_THERM + j, AllThermo[i]->Address[j]);
					}
					SigmaEEPROM::Write16(EEPROM_ADDR_THERM + i * EEPROM_LEN_THERM + 8, (int)(AllThermo[i]->MinTemp * 2));
					SigmaEEPROM::Write16(EEPROM_ADDR_THERM + i * EEPROM_LEN_THERM + 8 + 2, (int)(AllThermo[i]->MaxTemp * 2));
				}
				AllThermo[i]->InitUnit();
				break;
			}
		}
	}
}


void DeviceManager::UpdateContactorEquipment(const char* name, const char* payload)
{
	Contactor* cont = NULL;
	int n = 0;
	if (strcmp(name, PressureSwitch.Name) == 0) {
		cont = &PressureSwitch;
	} else if (strcmp(name, VoltageSwitch.Name) == 0) {
		cont = &VoltageSwitch;
		n = 1;
	}

	if (cont != NULL) {
		byte pin = cont->Pin;
		byte lhOn = cont->lhOn;
		cont->UpdateContactor(payload);
		//Config.Log->append("Pin:").append(cont->Pin).append("#").append(pin).append(";lh:").append(cont->lhOn).append("#").append(lhOn).Debug();
		if (cont->Pin != pin || cont->lhOn != lhOn) {
			Config.Log->Debug("EEPROM CONTACTOR");
			SigmaEEPROM::Write8(EEPROM_ADDR_CONTACTOR + n * 2, cont->Pin);
			SigmaEEPROM::Write8(EEPROM_ADDR_CONTACTOR + n * 2 + 1, cont->lhOn);
		}
		cont->InitUnit();
	//	cont->print("", D_DEBUG);
	}
}


void DeviceManager::UpdateRelayStatus(const char* name, const char* payload)
{
	for (int i = 0; i < CONFIG_NUMBER_RELAYS; i++) {
		if (strcmp(AllRelays[i]->Name,name)==0) {
			byte p = atoi(payload);
			AllRelays[i]->ProcessUnit((ActionType)p);
			break;
		}
	}
}

void DeviceManager::UpdateThermoStatus(const char* name, const char* payload)
{
	for (int i = 0; i < CONFIG_NUMBER_THERMO; i++) {
		if (strcmp(AllThermo[i]->Name, name) == 0) {
			double t = atof(payload);
			AllThermo[i]->SetTemp(t);
			break;
		}
	}
}


void DeviceManager::UpdateContactorStatus(const char* name, const char* payLoad)
{
	ActionType p = (ActionType)(atoi(payLoad));
	if (strcmp(PressureSwitch.Name, name) == 0) {
		PressureSwitch.ProcessUnit(p);
	} else if (strcmp(VoltageSwitch.Name, name) == 0) {
		VoltageSwitch.ProcessUnit(p);
	}
}


DeviceManager::DeviceManager(){
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
		AllThermo[i]->MinTemp = ((int16_t)(SigmaEEPROM::Read16(EEPROM_ADDR_THERM + i * EEPROM_LEN_THERM + 8))) / 2.0;
		AllThermo[i]->MaxTemp = ((int16_t)(SigmaEEPROM::Read16(EEPROM_ADDR_THERM + i * EEPROM_LEN_THERM + 8 + 2))) / 2.0;
	}
	return res;
}

bool DeviceManager::Init(){
	bool res = true;
	return res;
}

bool DeviceManager::FinalInit()
{
	/*
	for (int i = 0; i < CONFIG_NUMBER_OF_DEVICES; i++) {
		AllDevices[i]->InitUnit();
	}
	*/

	for (int i = 0; i < CONFIG_NUMBER_RELAYS; i++) {
		AllRelays[i]->InitUnit();
	}

	PressureSwitch.InitUnit();
	VoltageSwitch.InitUnit();
	Bus.InitUnit();

	for (int i = 0; i < CONFIG_NUMBER_THERMO; i++) {
		AllThermo[i]->InitUnit();
	}

	PublishEquipment();
	return true;

}

void DeviceManager::PublishEquipment()
{
	//Nothing todo
}

void DeviceManager::SubscribeEquipment() {
	int n = 0;

	for (int i = 0; i < CONFIG_NUMBER_RELAYS; i++) {
		sprintf(Config.TopicBuff, MQTT_EQUIPMENT_RELAY, Config.BoardId(), AllRelays[i]->Name);
		Config.Subscribe(Config.TopicBuff);
		n++;
	}

	sprintf(Config.TopicBuff, MQTT_EQUIPMENT_CONTACTOR, Config.BoardId(), PressureSwitch.Name);
	Config.Subscribe(Config.TopicBuff);
	n++;
	sprintf(Config.TopicBuff, MQTT_EQUIPMENT_CONTACTOR, Config.BoardId(), VoltageSwitch.Name);
	Config.Subscribe(Config.TopicBuff);
	n++;

	sprintf(Config.TopicBuff, MQTT_EQUIPMENT_THERMOMETER, Config.BoardId(), Bus.Name);
	Config.Subscribe(Config.TopicBuff);
	n++;
	for (int i = 0; i < CONFIG_NUMBER_THERMO; i++) {
		sprintf(Config.TopicBuff, MQTT_EQUIPMENT_THERMOMETER, Config.BoardId(), AllThermo[i]->Name);
		Config.Subscribe(Config.TopicBuff);
		n++;
	}
	Config.Transfer(n);
}



void DeviceManager::SubscribeStatuses() {

	int n = 0;
	for (int i = 0; i < CONFIG_NUMBER_RELAYS; i++) {
		sprintf(Config.TopicBuff, MQTT_STATUS_RELAY, Config.BoardId(), AllRelays[i]->Name);
		Config.Subscribe(Config.TopicBuff);
		n++;
	}
	sprintf(Config.TopicBuff, MQTT_STATUS_CONTACTOR, Config.BoardId(), PressureSwitch.Name);
	Config.Subscribe(Config.TopicBuff);
	n++;
	sprintf(Config.TopicBuff, MQTT_STATUS_CONTACTOR, Config.BoardId(), VoltageSwitch.Name);
	Config.Subscribe(Config.TopicBuff);
	n++;
	sprintf(Config.TopicBuff, MQTT_STATUS_THERMOMETER, Config.BoardId(), Bus.Name);
	Config.Subscribe(Config.TopicBuff);
	n++;
	for (int i = 0; i < CONFIG_NUMBER_THERMO; i++) {
		sprintf(Config.TopicBuff, MQTT_STATUS_THERMOMETER, Config.BoardId(), AllThermo[i]->Name);
		Config.Subscribe(Config.TopicBuff);
		n++;
	}
	Config.Transfer(n);

}

void DeviceManager::UpdateEquipment(const char* topic, const char* payload) {

	int len;
	sprintf(Config.TopicBuff, MQTT_EQUIPMENT_RELAYS, Config.BoardId());
	len = strlen(Config.TopicBuff);
	if (strncmp(Config.TopicBuff, topic, len) == 0) {
		strncpy(Config.TopicBuff, topic + len + 1, strlen(topic));
		UpdateRelayEquipment(Config.TopicBuff, payload);
	}
	else {
		sprintf(Config.TopicBuff, MQTT_EQUIPMENT_CONTACTORS, Config.BoardId());
		len = strlen(Config.TopicBuff);
		if (strncmp(Config.TopicBuff, topic, len) == 0) {
			strncpy(Config.TopicBuff, topic + len + 1, strlen(topic));
			UpdateContactorEquipment(Config.TopicBuff, payload);
		}
		else {
			sprintf(Config.TopicBuff, MQTT_EQUIPMENT_THERMOMETERS, Config.BoardId());
			len = strlen(Config.TopicBuff);
			if (strncmp(Config.TopicBuff, topic, len) == 0) {
				strncpy(Config.TopicBuff, topic + len + 1, strlen(topic));
				UpdateThermoEquipment(Config.TopicBuff, payload);
			}
		}
	}
}

void DeviceManager::UpdateStatuses(const char* topic, const char* payload)
{
	int len;
	sprintf(Config.TopicBuff, MQTT_STATUS_RELAYS, Config.BoardId());
	len = strlen(Config.TopicBuff);
	if (strncmp(Config.TopicBuff, topic, len) == 0) {
		strncpy(Config.TopicBuff, topic + len + 1, strlen(topic));
		UpdateRelayStatus(Config.TopicBuff, payload);
	}
	else {
		sprintf(Config.TopicBuff, MQTT_STATUS_CONTACTORS, Config.BoardId());
		len = strlen(Config.TopicBuff);
		if (strncmp(Config.TopicBuff, topic, len) == 0) {
			strncpy(Config.TopicBuff, topic + len + 1, strlen(topic));
			UpdateContactorStatus(Config.TopicBuff, payload);
		}
		else {
			sprintf(Config.TopicBuff, MQTT_STATUS_THERMOMETERS, Config.BoardId());
			len = strlen(Config.TopicBuff);
			if (strncmp(Config.TopicBuff, topic, len) == 0) {
				strncpy(Config.TopicBuff, topic + len + 1, strlen(topic));
				UpdateThermoStatus(Config.TopicBuff, payload);
			}
		}

	}
}
