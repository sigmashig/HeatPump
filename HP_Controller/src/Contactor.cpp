
#include "Contactor.h"

#include "Configuration.h"
#include "Loger.h"
#include "SigmaEEPROM.h"

extern Configuration Config;



void Contactor::InitUnit() {
	if (Pin != 0) {
		pinMode(Pin, INPUT);
		digitalWrite(Pin, lhOn ? LOW : HIGH);
	}
	prevValue = 0xff;
	startContact = 0;
	IsOk();
	//status = !lhOn;
	PublishDeviceAlert(ALERT_EMPTY, true);

}

bool Contactor::IsSimulator() {
	return (Config.IsSimulator() || Pin==0);
}

void Contactor::handleContactor(unsigned long timePeriod) {

	byte cntValue;
	if (!IsSimulator()) {
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
	Status = newStatus;
	Publish();
}

void Contactor::UnitLoop(unsigned long timePeriod)
{
	handleContactor(timePeriod);
	/*
	if (!IsOk())
	{
		if (!IsAlert)
		{
			publishDeviceAlert("Contactor is in alert mode");
			IsAlert = true;
		}
	}
	else if (IsAlert)
	{
		publishDeviceAlert("");
		IsAlert = false;
	}
	*/
}

bool Contactor::IsOk() {
	bool res = false;
	//Config.Log->append("ISOK. Contactor ").append(Name).append(" = ").append(status).Debug();
	
	if (Status == lhOn) {
		PublishDeviceAlert(ALERT_EMPTY);
		res = true;
	} 
	return res;
}

void Contactor::FinalInitUnit()
{
}



void Contactor::ProcessUnit(ActionType event) {
	if (event == ACT_OFF) {
		Status = !lhOn;
	} else if (event == ACT_ON) {
		Status = lhOn;
	}
	//Config.Log->append("Contactor ").append(Name).append(" = ").append(status).Debug();
}

void const Contactor::print(const char* header, DebugLevel level) {
	if (header != NULL) {
		Config.Log->append(header);
	}
	Config.Log->append(F("Name:")).append(Name);
	Config.Log->append(F(";Pin:")).append(Pin);
	Config.Log->append(F(";lhOn:")).append(lhOn);
	Config.Log->append(F(" @"));
	Config.Log->Log(level);
}

Contactor::Contactor(const char* nm) : Unit(DEVTYPE_CONTACTOR, nm)
{
}

void Contactor::UpdateEquipment(const char* line)
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
}


void Contactor::UpdateStatus(const char* payload) {
	ActionType a = (ActionType)atoi(payload);
	ProcessUnit(a);
}