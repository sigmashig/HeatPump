
#include "Relay.h"

#include "Configuration.h"
#include "SigmaEEPROM.h"

extern Configuration Config;

void Relay::InitUnit() {
	if (Pin != 0) {
		pinMode(Pin, OUTPUT);
	}
	PublishDeviceAlert(ALERT_EMPTY, true);
}

bool Relay::relaySet(bool newStatus) {
	bool res = false;
	if (Status != newStatus) {
		if (!IsSimulator()) {
			digitalWrite(Pin, (newStatus == HIGH ? lhOn : !lhOn));
		}
		Status = newStatus;
		Publish();
	}
	return res;
}

bool Relay::relaySwitch() {
	return relaySet(!Status);
}

bool Relay::ProcessUnit(ActionType event) {
	bool res = false;

	switch (event) {
	case ACT_OFF:
	{
		res = relayOff();
		break;
	}
	case ACT_ON:
	{
		res = relayOn();
		break;
	}
	case ACT_SWITCH:
	{
		res = relaySwitch();
		break;
	}
	default:
		break;
	}

	return res;
}

bool Relay::relayOn() {
	return relaySet(HIGH);
};

bool Relay::relayOff() {
	return relaySet(LOW);
};

void Relay::UnitLoop(unsigned long timePeriod) {
	// nothing todo
}

bool Relay::IsSimulator() {
	return (Config.IsSimulator() || Pin == 0);
}

void const Relay::print(const char* header, DebugLevel level) {
	if (header != NULL) {
		Config.Log->append(header);
	}
	Config.Log->append(F("Name:")).append(Name);
	Config.Log->append(F(";Pin:")).append(Pin);
	Config.Log->append(F(";lhOn:")).append(lhOn);
	Config.Log->append(F(";Sim:")).append(IsSimulator());
	Config.Log->append(F(";status:")).append(Status);
	Config.Log->append(F(" @"));
	Config.Log->Log(level);
}

Relay::Relay(const char* nm): Unit(DEVTYPE_RELAY, nm) {
}

void Relay::UpdateEquipment(const char* line) {
	const size_t CAPACITY = JSON_OBJECT_SIZE(10);
	StaticJsonDocument<CAPACITY> doc;
	//deserializeJson(doc, line);
	DeserializationError error = deserializeJson(doc, line);
	if (error) {
		Config.Log->append("JSON Error=").append(error.f_str()).Error();
		return;
	}

	// extract the data
	JsonObject json = doc.as<JsonObject>();

	if (json.containsKey("pin")) {
		Pin = json["pin"];
	}
	if (json.containsKey("lhOn")) {
		lhOn = json["lhOn"];
	}
}

bool Relay::IsOk() {
	bool res = true;

	res = Status == HIGH;
	if (res) {
		PublishDeviceAlert(ALERT_EMPTY);
	}
	return res;
}

void Relay::UpdateStatus(const char* payload) {
//TODO: For some relay, like Compressor it should be prohibited to control remotely
	ActionType a = (ActionType)atoi(payload);
	ProcessUnit(a);
}

