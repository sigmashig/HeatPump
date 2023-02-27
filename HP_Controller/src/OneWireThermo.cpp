#include "OneWireThermo.h"
#include "Loger.h"
#include "OneWireBus.h"
#include "Loger.h"
#include "Configuration.h"

extern Configuration Config;


void OneWireThermo::InitUnit() {
	parent = &(Config.DevMgr->Bus);
	if (OneWireBus::IsZeroAddress(Address)) {
		// Simulator
		isSimulator = true;
	} else 	if (!parent->CheckAddress(Address)) {
		Config.Log->append(F("Unit:")).append(Name).append(F(" is absent on the bus")).Error();
		isSimulator = true;
	} else {
		isSimulator = false;
		parent->SetResolution(Address);
	}
}


float OneWireThermo::GetTemperature() {
	//Config.Log->append("Thermometer:").append(Name).append(";avail=").append(IsAvailable).Debug();
	if (!isSimulator) {
		Temperature = parent->GetTemperature(Address);
		sprintf(Config.TopicBuff, MQTT_STATUS_THERMOMETER, Config.BoardId(), Name);
		sprintf(Config.PayloadBuff, "%d.%u", (int)Temperature, (unsigned)((Temperature - (int)Temperature) * 10));
		Config.Publish();
	}
	return Temperature;
}

void OneWireThermo::UpdateThermo(const char* line) {
	const size_t CAPACITY = JSON_OBJECT_SIZE(JSON_SIZE);
	StaticJsonDocument<CAPACITY> doc;
	deserializeJson(doc, line);
	// extract the data
	JsonObject json = doc.as<JsonObject>();

	if (json.containsKey("address")) {
		const char* s = json["address"];
		OneWireBus::ConvertStringToAddress(Address, s);
		checkSimulator();
	}
	if (json.containsKey("min")) {
		//Config.Log->append("min = ").append(s).Debug();
		MinTemp = json["min"];
	}
	if (json.containsKey("max")) {
		MaxTemp = json["max"];
	}
}

bool OneWireThermo::IsOk() {
	return (Temperature >= MinTemp && Temperature <= MaxTemp);
}



void const OneWireThermo::print(const char* header, DebugLevel level) {
	if (header != NULL) {
		Config.Log->append(header);
	}
	Config.Log->append(F("Name:")).append(Name);
	Config.Log->append(F(";Address:")).append(OneWireBus::ConvertAddressToString(Address).c_str());
	Config.Log->append(F(";Min:")).append(MinTemp);
	Config.Log->append(F(";Max:")).append(MaxTemp);
	Config.Log->append(F("@"));
	Config.Log->Log(level);

}

OneWireThermo::OneWireThermo(const char* nm): Unit(nm) {
}

bool OneWireThermo::checkSimulator() {
	isSimulator = Config.IsSimulator()
		&& OneWireBus::IsZeroAddress(Address) &&
		!parent->CheckAddress(Address);
	return isSimulator;
}

void OneWireThermo::UnitLoop(unsigned long timeperiod) {

	if (timeperiod == 1000) {
		if (!isSimulator) {
			GetTemperature();
			if (!IsOk()) {
				publishDeviceAlert(ALERT_TEMP_IS_OUT_OF_RANGE);
			} else {
				publishDeviceAlert(ALERT_EMPTY);
			}
		}
	}
}

