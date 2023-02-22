
#include "Contactor.h"

#include "Configuration.h"
#include "Loger.h"
#include "SigmaEEPROM.h"

extern Configuration Config;



void Contactor::InitUnit() {
	if (Pin != 0) {
		pinMode(Pin, INPUT);
		digitalWrite(Pin, lhOn ? LOW : HIGH);
		IsAvailable = true;
	}
	else {
		IsAvailable = false;
	}
	prevValue = 0xff;
	startContact = 0;
	status = !lhOn;
}


void Contactor::handleContactor(unsigned long timePeriod) {

	byte cntValue;
	if (IsAvailable) {
		cntValue = digitalRead(Pin);
		if (prevValue != cntValue) { // contactor is starting switch
			unsigned long now = millis();
			if (startContact == 0) {
				startContact = now;
			}
			else {

				if (startContact + CONTACTOR_SWITCHED_TIME <= now) {//contact is long enough
					handleFinish(cntValue == lhOn ? ACT_ON : ACT_OFF);
					startContact = 0;
					prevValue = cntValue;
				}
			}
		}
	}
}

void Contactor::handleFinish(int newStatus) {
	status = newStatus;
	Publish(MQTT_CONTACTORS);
}


void Contactor::UnitLoop(unsigned long timePeriod) {
	handleContactor(timePeriod);
	if (IsReady) {
		if (!IsOk()) {
			if (!IsAlert) {
				publishDeviceAlert("Contactor is in alert mode");
				IsAlert = true;
			}
		}
		else if (IsAlert) {
			publishDeviceAlert("");
			IsAlert = false;
		}
	}
}

void Contactor::FinalInitUnit()
{
	// No Subscription
	// No Values request
}



void Contactor::ProcessUnit(ActionType event)
{
	status = event;
}

void const Contactor::print(const char* header, DebugLevel level) {
	if (header != NULL) {
		Config.Log->append(header);
	}
	Config.Log->append(F("Name:")).append(Name);
	Config.Log->append(F(";Pin:")).append((unsigned int)Pin);
	Config.Log->append(F(";lhOn:")).append((unsigned int)lhOn);
	Config.Log->append(F(" @"));
	Config.Log->Log(level);
}

Contactor::Contactor(const char* nm) : Unit(nm)
{
}

void Contactor::UpdateContactor(const char* line)
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
//	if (!IsReady) {
//		Config.DevMgr->IncreaseNumberOfDevices();
//	}
	IsReady = true;
}
