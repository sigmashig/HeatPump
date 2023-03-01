#include "Unit.h"
#include "Configuration.h"

extern Configuration Config;


void const Unit::print(const char* header, DebugLevel level){
	
	if (header != NULL) {
		Config.Log->append(header);
	}
	Config.Log->append(F("Name:")).append(Name);
	Config.Log->append(F(";status:")).append(status);
	Config.Log->append(F(" @"));
	Config.Log->Log(level);
}

void Unit::Publish(const char* uPrefix)
{
	sprintf(Config.TopicBuff, MQTT_STATUS "%s%s%s%s", Config.BoardId(), MQTT_SEPARATOR, uPrefix, MQTT_SEPARATOR, Name);
	sprintf(Config.PayloadBuff, "%u", status);
	Config.Publish();
}

void Unit::PublishDeviceAlert(ALERTCODE code) {
	if (alertCode != code) {
		Config.PublishAlert(code, Name);
		alertCode = code;
	}
}



