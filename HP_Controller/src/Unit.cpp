#include "Unit.h"
#include "Configuration.h"

extern Configuration Config;


void const Unit::print(const char* header, DebugLevel level){
	
	if (header != NULL) {
		Config.Log->append(header);
	}
	Config.Log->append(F("Name:")).append(Name);
	Config.Log->append(F(";status:")).append((unsigned int)status);
	Config.Log->append(F(" @"));
	Config.Log->Log(level);
}

void Unit::Publish(const char* uPrefix)
{
	sprintf(Config.TopicBuff, MQTT_STATUS "%s%s%s%s", Config.BoardId(), MQTT_SEPARATOR, uPrefix, MQTT_SEPARATOR, Name);
	sprintf(Config.PayloadBuff, "%u", status);
	Config.Publish(Config.TopicBuff, Config.PayloadBuff);
}

void Unit::publishDeviceAlert(const char* msg) {
	sprintf(Config.TopicBuff, MQTT_ALERT_DEVICE, Config.BoardId(), Name);
	Config.Publish(Config.TopicBuff, msg);
}



