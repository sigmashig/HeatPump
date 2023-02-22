#include "OneWireThermo.h"
#include "Loger.h"
#include "OneWireBus.h"
#include "Loger.h"
#include "Configuration.h"

extern Configuration Config;


void OneWireThermo::InitUnit() {
	parent = &(Config.DevMgr->Bus);
	if (!parent->CheckAddress(Address)) {
		Config.Log->append(F("Unit:")).append(Name).append(F(" is absent on the bus")).Error();
		IsAvailable = false;
	}
	else {
		IsAvailable = true;
		parent->SetResolution(Address);
	}
}


float OneWireThermo::GetTemperature() {
	//Config.Log->append("Thermometer:").append(Name).append(";avail=").append(IsAvailable).Debug();
	if (IsAvailable) {
		Temperature = parent->GetTemperature(Address);
		sprintf(Config.TopicBuff, MQTT_STATUS_THERMOMETER, Config.BoardId(), Name);
		sprintf(Config.PayloadBuff, "%d.%u", (int)Temperature, (unsigned)((Temperature - (int)Temperature) * 10));
		Config.MqttClient->Publish(Config.TopicBuff, Config.PayloadBuff);
	}
	return Temperature;
}

void OneWireThermo::UpdateThermo(const char* line)
{
	const size_t CAPACITY = JSON_OBJECT_SIZE(JSON_SIZE);
	StaticJsonDocument<CAPACITY> doc;
	deserializeJson(doc, line);
	// extract the data
	JsonObject json = doc.as<JsonObject>();

	if (json.containsKey("address")) {
		const char* s = json["address"];
		OneWireBus::ConvertStringToAddress(Address, s);
		IsAvailable = OneWireBus::IsZeroAddress(Address);
	}
	if (json.containsKey("min")) {
		//Config.Log->append("min = ").append(s).Debug();
		MinTemp = json["min"];
	}
	if (json.containsKey("max")) {
		MaxTemp = json["max"];
	}
	//print("", D_DEBUG);	
//	if (!IsReady) {
//		Config.DevMgr->IncreaseNumberOfDevices();
//	}
	IsReady = true;

}

bool OneWireThermo::IsOk()
{
	return (Temperature>=MinTemp && Temperature<=MaxTemp);
}



void const OneWireThermo::print(const char* header, DebugLevel level)
{
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

OneWireThermo::OneWireThermo(const char* nm) : Unit(nm)
{
}

void OneWireThermo::UnitLoop(unsigned long timeperiod) {
		
	if (timeperiod == 1000) {
		GetTemperature();
		if (IsReady) {
			if (!IsOk()) {
				if (!IsAlert) {
					char tmp[MQTT_PAYLOAD_LENGTH];
					sprintf(tmp, "Temperature is out of range: %d.%d...%d.%d", (int)MinTemp, (int)(10 * (MinTemp - (int)MinTemp))
						, (int)MaxTemp, (int)(10 * (MaxTemp - (int)MaxTemp)));
					publishDeviceAlert(tmp);
					IsAlert = true;
				}
			}
			else {
				if (IsAlert) {
					publishDeviceAlert("");
					IsAlert = false;
				}
			}
		}
	}
}
