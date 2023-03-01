
#include "Relay.h"

#include "Configuration.h"
#include "SigmaEEPROM.h"

extern Configuration Config;

void Relay::InitUnit() {
	if (Pin != 0) {
		pinMode(Pin, OUTPUT);
	}
}

bool Relay::relaySet(bool newStatus) {
	bool res = false;
	if (status != newStatus) {
		if (!IsSimulator()) {
			digitalWrite(Pin, (newStatus == HIGH ? lhOn : !lhOn));
		}
		status = newStatus;
		Publish(MQTT_RELAYS);
	}
	return res;
}

bool Relay::relaySwitch() {
	return relaySet(!status);
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
	Config.Log->append(F(";status:")).append(status);
	Config.Log->append(F(" @"));
	Config.Log->Log(level);
}

Relay::Relay(const char* nm): Unit(nm) {
}

void Relay::UpdateRelay(const char* line) {
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
}

bool Relay::IsOk() {
	bool res = true;

	res = status == lhOn;
	if (res) {
		PublishDeviceAlert(ALERT_EMPTY);
	}
	return res;
}
