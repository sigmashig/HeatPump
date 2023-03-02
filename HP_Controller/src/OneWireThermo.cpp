#include "OneWireThermo.h"
#include "Loger.h"
#include "OneWireBus.h"
#include "Loger.h"
#include "Configuration.h"

extern Configuration Config;


void OneWireThermo::InitUnit() {
	parent = &(Config.DevMgr->Bus);
	Config.Log->Debug("POINT2.5");
	
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
	Config.Log->Debug("POINT2.6");
	PublishDeviceAlert(ALERT_EMPTY, true);
	Config.Log->Debug("POINT2.7");

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
				PublishDeviceAlert(ALERT_TEMP_IS_OUT_OF_RANGE);
			} else {
				PublishDeviceAlert(ALERT_EMPTY);
			}
		}
	}
}

