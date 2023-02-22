
#include "Relay.h"

#include "Configuration.h"
#include "SigmaEEPROM.h"

extern Configuration Config;

void Relay::InitUnit() {
	if (Pin != 0) {
		pinMode(Pin, OUTPUT);
		IsAvailable = true;
	}
	else {
		IsAvailable = false;
	}
}

void Relay::RelaySet(bool newStatus)
{
	if (IsAvailable) {
		digitalWrite(Pin, (newStatus == HIGH ? lhOn : !lhOn));
	}
	status = newStatus;
	Publish(MQTT_RELAYS);
}

void Relay::RelaySwitch() {
	RelaySet(!status);
}

void Relay::ProcessUnit(ActionType event) {
	switch (event) {
	case ACT_OFF: {
		RelayOff();
		break;
	}
	case ACT_ON: {
		RelayOn();
		break;
	}
	case ACT_SWITCH: {
		RelaySwitch();
		break;
	}
	default:
		break;
	}
}

void Relay::UnitLoop(unsigned long timePeriod) {
	//nothing todo
}

void const Relay::print(const char* header, DebugLevel level) {
	if (header != NULL) {
		Config.Log->append(header);
	}
	Config.Log->append(F("Name:")).append(Name);
	Config.Log->append(F(";Pin:")).append((unsigned int)Pin);
	Config.Log->append(F(";lhOn:")).append((unsigned int)lhOn);
	Config.Log->append(F(";status:")).append((unsigned int)status);
	Config.Log->append(F(" @"));
	Config.Log->Log(level);
}

Relay::Relay(const char* nm) : Unit(nm)
{

}


void Relay::UpdateRelay(const char* line)
{
	const size_t CAPACITY = JSON_OBJECT_SIZE(JSON_SIZE);
	StaticJsonDocument<CAPACITY> doc;
	deserializeJson(doc, line);
	// extract the data
	JsonObject json = doc.as<JsonObject>();

	if (json.containsKey("pin")) {
		Pin = json["pin"];
	}
	if (json.containsKey("lhOn")) {
		lhOn = json["lhOn"];
	}
	//if (!IsReady) {
	//	Config.DevMgr->IncreaseNumberOfDevices();
	//}
	IsReady = true;

}
