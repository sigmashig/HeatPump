#include "OneWireBus.h"
#include <OneWire.h>
#include "definitions.h"
#include "Unit.h"
#include "Loger.h"
#include <DallasTemperature.h>
#include "Configuration.h"

extern Configuration Config;



bool OneWireBus::CheckAddress(const DeviceAddress address) {
	return sensors->isConnected(address);
}

double OneWireBus::GetTemperature(const DeviceAddress address) {
	if (CheckAddress(address)) {
		return sensors->getTempC(address);
	} else {
		Config.Log->append(F("Temp is failed: ")).append(Name).Error();
		return -999;
	}
}


void OneWireBus::InitUnit() {
	releaseResources();
	oneWire = new OneWire(Pin);
	PublishDeviceAlert(ALERT_EMPTY, true);
	sensors = new DallasTemperature(oneWire);
	sensors->begin();
	printDevices();
}

int OneWireBus::GetDeviceCount() {
	return sensors->getDeviceCount();
}

void OneWireBus::printDevices() {
	Config.Log->append(F("Found ")).append(GetDeviceCount()).append(F(" devices on the bus")).Info();
	for (int i = 0; i < GetDeviceCount(); i++) {
		DeviceAddress addr;
		sensors->getAddress(addr, i);

		Config.Log->append(F("Device ")).append(i).append(F(": ")).append(ConvertAddressToString(addr).c_str()).append(F(" ")).Info();
	}
}
void OneWireBus::ProcessUnit(ActionType action) {
	//	status = action;
}

void OneWireBus::UnitLoop(unsigned long timeperiod) {
	if (timeperiod == 10000) {
		if (Config.Counter10 % 3 == 0) { //request on 20 and 50 sec
			//Config.Log->append(F("Request temperature")).Debug();
			RequestTemperature();
		}
	}
}

void OneWireBus::SetResolution(const DeviceAddress address) {
	sensors->setResolution(address, BUS_RESOLUTION);
}

void OneWireBus::RequestTemperature() {
	if (!IsSimulator()) {
		sensors->requestTemperatures();
	}
}

byte OneWireBus::hexToDec(char c1) {
	switch (c1) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return (byte)(c1 - '0');
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		return (byte)(c1 - 'a' + 10);
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
		return (byte)(c1 - 'A' + 10);
	default:
		return 0;
	}
}


byte OneWireBus::hexToDec(char c1, char c2) {
	return (hexToDec(c1) * 16 + hexToDec(c2));
}

void OneWireBus::ConvertStringToAddress(DeviceAddress address, const char* addrStr) {
	for (int i = 0, j = 0; i < 16; i += 2, j++) {
		address[j] = hexToDec(addrStr[i], addrStr[i + 1]);
	}
}


bool OneWireBus::CompareDeviceAddress(DeviceAddress a0, DeviceAddress a1) {
	bool res = false;

	for (int i = 0; !res && i < 8; i++) {
		res &= (a0[i] == a1[i]);
	}
	return res;
}

void OneWireBus::CopyDeviceAddress(DeviceAddress dest, DeviceAddress src) {
	for (int i = 0; i < 8; i++) {
		dest[i] = src[i];
	}
}


void const OneWireBus::print(const char* header, DebugLevel level) {

	if (header != NULL) {
		Config.Log->append(header);
	}
	Config.Log->append(F("Name:")).append(Name);
	Config.Log->append(F(";Pin:")).append(Pin);
	Config.Log->append(F(" @"));
	Config.Log->Log(level);
}


String OneWireBus::ConvertAddressToString(const DeviceAddress address) {
	String str0 = "";

	for (int i = 0; i < 8; i++) {
		str0 += String(address[i], HEX);
		if (i != 7) {
			str0 += ':';
		}
	}
	return str0;
}

bool OneWireBus::IsZeroAddress(DeviceAddress address) {

	bool res = true;
	for (int i = 0; res && i < 8; i++) {
		res &= address[i] == 0;
	}
	return res;
}

OneWireBus::OneWireBus(const char* nm): Unit(DEVTYPE_BUS, nm) {
}

bool OneWireBus::IsSimulator() {
	return (Config.IsSimulator() || Pin == 0);
}

void OneWireBus::UpdateStatus(const char* payload) {
	// Nothing to do
}

bool OneWireBus::UpdateEquipment(const char* line) {

	if (strlen(line) == 0) {
		Config.Log->Debug("No equipment data. Create default equipment.");
		PublishDefaultEquipment();
		return true;
	}

	bool res = false;
	const size_t CAPACITY = JSON_OBJECT_SIZE(5);
	StaticJsonDocument<CAPACITY> doc;
	//deserializeJson(doc, payload);
	DeserializationError error = deserializeJson(doc, line);
	if (error) {
		Config.Log->append("JSON Error=").append(error.f_str()).Error();
		return false;
	}

	// extract the data
	JsonObject json = doc.as<JsonObject>();

	if (json.containsKey("pin")) {
		byte pin = json["pin"];
		res |= (pin != Pin);
		Pin = pin;
	}
	return res;
}

void OneWireBus::releaseResources() {
	if (oneWire != NULL) {
		delete oneWire;
		oneWire = NULL;
	}
	if (sensors != NULL) {
		delete sensors;
		sensors = NULL;
	}
}

void OneWireBus::PublishDefaultEquipment() {
	// {'pin':30}
	StaticJsonDocument<100> doc;
	doc["pin"] = Pin;
	char str[20];
	serializeJson(doc, str);
	PublishEquipment(str);	
}