#include "Loger.h"
#include "definitions.h"
#include "Configuration.h"

extern Configuration Config;
extern DebugLevel DLevel;

void Loger::Log(DebugLevel level) {
	if (level <= DLevel) {
		Serial.println(loger->c_str());
		if (level != D_INTERNAL) {
			//Config.MqttClient->PublishLog(level, loger->c_str());
		}
	}

	loger->clear();
}

Loger::Loger(int size)
{
	this->size = size;

	loger_SAFEBUFFER = new char[size + 1];
	loger = new SafeString(size, loger_SAFEBUFFER, "");
}