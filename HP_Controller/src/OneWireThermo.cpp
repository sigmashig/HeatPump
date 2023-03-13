#include "OneWireThermo.h"
#include "Loger.h"
#include "OneWireBus.h"
#include "Loger.h"
#include "Configuration.h"

extern Configuration Config;


void OneWireThermo::InitUnit() {
	parent = &(Config.DevMgr->Bus);
	if (OneWireBus::IsZeroAddress(Address)) {
		isSimulator = true;
	} else 	if (!parent->CheckAddress(Address)) {
		Config.Log->append(F("Unit:")).append(Name).append(F(" is absent on the bus")).Error();
		isSimulator = true;
	} else {
		isSimulator = false;
		parent->SetResolution(Address);
	}
	if (!IsSimulator()) {
		publishTemp();
	}
	PublishDeviceAlert(ALERT_EMPTY, true);
}

double OneWireThermo::getTemperature() {
	if (!isSimulator) {
		Config.Log->append(F("Get temperature from ")).append(Name).Debug();
		double t = parent->GetTemperature(Address);
		if (t != Temperature) {
			Temperature = t;
			publishTemp();
		}
	}
	return Temperature;
}

void OneWireThermo::publishTemp() {

	if (Temperature > -50 && Temperature <= 125) {
		int p1 = (int)Temperature;
		int p2 = (int)((Temperature - p1) * 10);
		char p[10];
		sprintf(p, "%d.%d", p1, p2);

		Config.Publish(DevType, Name, p);
	}
}
void OneWireThermo::UpdateEquipment(const char* line) {
	//const size_t CAPACITY = JSON_OBJECT_SIZE(10);
	DynamicJsonDocument doc(400);
	DeserializationError error = deserializeJson(doc, line);
	if (error) {
		Config.Log->append("JSON Error=").append(error.f_str()).Error();
		return;
	}
//StaticJsonDocument<CAPACITY> doc;
	//deserializeJson(doc, line);
	// extract the data
	JsonObject json = doc.as<JsonObject>();

	if (json.containsKey("address")) {
		const char* s = json["address"];
		OneWireBus::ConvertStringToAddress(Address, s);
		//checkSimulator();
	}
	if (json.containsKey("errorLow")) {
		ErrorLow = json["errorLow"];
	}
	if (json.containsKey("errorHigh")) {
		ErrorHigh = json["errorHigh"];
	}
	if (json.containsKey("warningHigh")) {
		WarningHigh = json["warningHigh"];
	}
	if (json.containsKey("warningLow")) {
		WarningLow = json["warningLow"];
	}

}

bool OneWireThermo::IsOk() {
	bool res = Temperature >= ErrorLow && Temperature <= ErrorHigh;
	if (res) {
		PublishDeviceAlert(ALERT_EMPTY);
	}
	return res;
}



void const OneWireThermo::print(const char* header, DebugLevel level) {
	if (header != NULL) {
		Config.Log->append(header);
	}
	Config.Log->append(F("Name:")).append(Name);
	Config.Log->append(F(";Address:")).append(OneWireBus::ConvertAddressToString(Address).c_str());
	//Config.Log->append(F(";Min:")).append(MinTemp);
	//Config.Log->append(F(";Max:")).append(MaxTemp);
	Config.Log->append(F("@"));
	Config.Log->Log(level);

}

OneWireThermo::OneWireThermo(const char* nm): Unit(DEVTYPE_THERMOMETER, nm) {
}

bool OneWireThermo::checkSimulator() {
	isSimulator = Config.IsSimulator()
		&& OneWireBus::IsZeroAddress(Address) &&
		!parent->CheckAddress(Address);
	return isSimulator;
}

void OneWireThermo::UnitLoop(unsigned long timeperiod) {

	if (timeperiod == 10000) {
		if (!isSimulator) {
			if (Config.Counter10 % 3 == 1) { // get temp from sensor every 30 sec: 10 and 40 sec
				getTemperature();
			}
		}
	}
}


void OneWireThermo::UpdateStatus(const char* payload) {
	Temperature =atof(payload);
}