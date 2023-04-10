#include "OneWireThermo.h"
#include "Loger.h"
#include "OneWireBus.h"
#include "Loger.h"
#include "Configuration.h"
#include "MemoryExplorer.h"

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
bool OneWireThermo::UpdateEquipment(const char* line) {

	bool res = false;
	if (strlen(line) < 10) {
		Config.Log->Debug("No equipment data. Create default equipment.");
		PublishDefaultEquipment();
		return true;
	}
	DynamicJsonDocument doc(400);
	DeserializationError error = deserializeJson(doc, line);
	if (error) {
		if (error == error.EmptyInput) {
			Config.Log->append(F("Empty equipment data. Create default equipment.")).Debug();
			PublishDefaultEquipment();
			return true;
		}

		Config.Log->append("JSON Error=").append(error.f_str()).Error();
		return false;
	}
	JsonObject json = doc.as<JsonObject>();

	if (json.containsKey("address")) {	
		const char* s = json["address"];
		String s1 = OneWireBus::ConvertAddressToString(Address);
		OneWireBus::ConvertStringToAddress(Address, s);
		res |= (strcmp(s1.c_str(), s) != 0);
	}
	if (json.containsKey("errorLow")) {
		double e = json["errorLow"];
		res |= (e != ErrorLow);
		ErrorLow = e;
	}
	if (json.containsKey("errorHigh")) {
		double e = json["errorHigh"];
		res |= (e != ErrorHigh);
		ErrorHigh = e;
	}
	if (json.containsKey("warningHigh")) {
		double e = json["warningHigh"];
		res |= (e != WarningHigh);
		WarningHigh = e;
	}
	if (json.containsKey("warningLow")) {
		double e = json["warningLow"];
		res |= (e != WarningLow);
		WarningLow = e;
	}
	return res;
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

void OneWireThermo::PublishDefaultEquipment() {
	//{'address':'28FFB804B416056C',  'errorLow':'-30', 'warningLow':-20, 'warningHigh':100, 'errorHigh':120}
	memoryReport("OneWireThermo start");
	DynamicJsonDocument doc(400);
	doc["address"] = OneWireBus::ConvertAddressToString(Address).c_str();
	doc["errorLow"] = ErrorLow;
	doc["errorHigh"] = ErrorHigh;
	doc["warningLow"] = WarningLow;
	doc["warningHigh"] = WarningHigh;
	memoryReport("OneWireThermo point 1");
	char buffer[150];
	int l = serializeJson(doc, buffer);
	Config.Log->append("Publish equipment:(").append(l).append(") ").append(buffer).Debug();
	PublishEquipment(buffer);
	memoryReport("OneWireThermo end");
}