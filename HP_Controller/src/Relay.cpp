
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
		unsigned long now = millis();
		if (status == LOW) {
			if (minTimeOff != NAN && now - lastOff < minTimeOff * 1000) {
				res = false;
			} else { // ok, let's switch a relay
				lastOn = now;
				res = true;
			}
		} else {
			if (minTimeOn != NAN && now - lastOn < minTimeOn * 1000) {
				res = false;
			} else { // ok, let's switch a relay
				lastOff = now;
				res = true;
			}
		}

		if (res) {
			if (!IsSimulator()) {
				digitalWrite(Pin, (newStatus == HIGH ? lhOn : !lhOn));
			}
			status = newStatus;
			Publish(MQTT_RELAYS);
		}
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
	if (json.containsKey("minTimeOff")) {
		minTimeOff = json["minTimeOff"];
	}
	if (json.containsKey("minTimeOn")) {
		minTimeOn = json["minTimeOn"];
	}
	if (json.containsKey("maxTimeOff")) {
		maxTimeOff = json["maxTimeOff"];
	}
	if (json.containsKey("maxTimeOn")) {
		maxTimeOn = json["maxTimeOn"];
	}
}

bool Relay::IsOk() {
	unsigned long now = millis();
	bool res = true;

	if (status == LOW) {
		if (maxTimeOff != NAN && now - lastOff > maxTimeOff) {
			res = false;
		} 
	} else {
		if (maxTimeOn != NAN && now - lastOn > maxTimeOn) {
			res = false;
		}		
	}
	return res;
}
