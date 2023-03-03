#include "Unit.h"
#include "Configuration.h"

extern Configuration Config;


void const Unit::print(const char* header, DebugLevel level) {

	if (header != NULL) {
		Config.Log->append(header);
	}
	Config.Log->append(F("Name:")).append(Name);
	Config.Log->append(F(";status:")).append(status);
	Config.Log->append(F(" @"));
	Config.Log->Log(level);
}

void Unit::Publish() {
	Config.Publish(DevType, Name, status);
}

void Unit::SubscribeEquipment() {
	Config.SubscribeEquipment(DevType, Name);
}

void Unit::SubscribeStatus() {
	Config.SubscribeStatus(DevType, Name);
}

void Unit::PublishDeviceAlert(ALERTCODE code, bool force) {
	if (alertCode != code || force) {
		Config.PublishAlert(code, Name);
		alertCode = code;
	}
}



