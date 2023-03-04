#include "Schedule.h"
#include <ArduinoJson.h>

#include "Configuration.h"

extern Configuration Config;

int Schedule::CompareTime(byte hr, byte mn) {
	if (hr < hour) {
		return -1;
	}
	else if (hr > hour) {
		return 1;
	}
	else {
		if (mn < minute) {
			return -1;
		}
		else if (mn > minute) {
			return 1;
		}
		else {
			return 0;
		}
	}
	return 0;
}

void Schedule::UpdateSchedule(const char* line)
{
	const size_t CAPACITY = JSON_OBJECT_SIZE(JSON_SIZE);
	StaticJsonDocument<CAPACITY> doc;
	deserializeJson(doc, line);
	// extract the data
	JsonObject json = doc.as<JsonObject>();

	if (json.containsKey("time")) {
		const char* time = json["time"];
		bool isSepFound = false;
		hour = 0;
		minute = 0;
		for (unsigned int i = 0; i < strlen(time); i++) {
			if (time[i] == ':') {
				isSepFound = true;
			}
			else {
				if (isDigit(time[i])) {
					if (isSepFound) {
						minute = minute * 10 + (time[i] - '0');
					}
					else {
						hour = hour * 10 + (time[i] - '0');
					}
				}
			}
		}
		if (hour >= 24) {
			hour = 0;
		}
		if (minute >= 60) {
			minute = 0;
		}
	}
	if (json.containsKey("temp")) {
		temperature = json["temp"];
	}

	//print("", D_DEBUG);
}

void Schedule::print(const char* header, DebugLevel level)
{
	if (header != NULL) {
		Config.Log->append(header);
	}
	Config.Log->append(F("Time:")).append(hour).append(":").append(minute);
	Config.Log->append(F(";Temp:")).append(temperature);
	Config.Log->append(F(" @"));
	Config.Log->Log(level);

}

void Schedule::Serialize(char* buffer) {
	
	StaticJsonDocument<JSON_SIZE> doc;
	doc["time"] = String(hour) + ":" + String(minute);
	doc["temp"] = temperature;
	serializeJson(doc, buffer, JSON_SIZE);
}

