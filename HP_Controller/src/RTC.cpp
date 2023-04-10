#include "RTC.hpp"
#include "Configuration.h"

extern Configuration Config;

RTC::RTC(const char* nm) : Unit(DEVTYPE_CLOCK, nm) {
}

void RTC::InitUnit() {
    Config.Log->Debug("Clock Init");
    time_t tm_sec = SigmaClock::SyncClock();
    Config.Log->append("TM:").append(tm_sec).Debug();
    SigmaClock::SetClock(tm_sec, tz, type, pins);
    Config.Log->append("Clock set").Debug();
    tm t = GetTime();
	Config.Log->append("Now:").Info(SigmaClock::PrintClock(t));

}

void RTC::UnitLoop(unsigned long timePeriod) {
    if (timePeriod % 1000 == 0 && Config.Counter60!= 0  && Config.Counter60 %(60*24)==0 ) {
        time_t t = SigmaClock::SyncClock();
        SigmaClock::SetClock(t, tz, type, pins);
    }
}

tm RTC::GetTime() {
    Config.Log->append("Type:").append(type).append(" Pins:").append(pins.datPin).append(":").append(pins.clkPin).append(":").append(pins.cePin).Debug();
    return SigmaClock::GetClock(type, pins);
}

void RTC::PublishDefaultEquipment() {
    // {'type':'DS1302','datPin':14,'clkPin':15,'cePin':16, 'tz':'2'}
    const size_t CAPACITY = JSON_OBJECT_SIZE(10);
    StaticJsonDocument<CAPACITY> doc;
    doc["type"] = (type == RTC_DS1302 ? "DS1302" : "DS3231");
    doc["datPin"] = pins.datPin;
    doc["clkPin"] = pins.clkPin;
    doc["cePin"] = pins.cePin;
    doc["tz"] = tz;
    char buffer[256];
    serializeJson(doc, buffer);
    Config.Log->append("Publishing default config:").append(buffer).Debug();
    PublishEquipment(buffer);
}

bool RTC::UpdateEquipment(const char* line) {
    // {'type':'DS1302','datPin':14,'clkPin':15,'cePin':16, 'tz':'2'}
    bool res = false;
    if (strlen(line) == 0) {
        Config.Log->Debug("No equipment data. Create default equipment.");
        PublishDefaultEquipment();
        return true;
    }
   const size_t CAPACITY = JSON_OBJECT_SIZE(10);
    StaticJsonDocument<CAPACITY> doc;
    DeserializationError error = deserializeJson(doc, line);
    if (error) {
        Config.Log->append("JSON Error=").append(error.f_str()).Error();
        return false;
    }

    // extract the data
    JsonObject json = doc.as<JsonObject>();

    if (json.containsKey("type")) {
        const char* s = json["type"];
        if (strcmp(s, "DS1302") == 0) {
            res |= (type != RTCType::RTC_DS1302);
            type = RTC_DS1302;
        } else {
            res |= (type != RTCType::RTC_DS3231);
            type = RTC_DS3231;
        }
    }
    if (json.containsKey("datPin")) {
        byte p = json["datPin"];
        res |= (pins.datPin != p);
        pins.datPin = p;
    }
    if (json.containsKey("clkPin")) {
        byte p = json["clkPin"];
        res |= (pins.clkPin != p);
        pins.clkPin = p;
    }
    if (json.containsKey("cePin")) {
        byte p = json["cePin"];
        res |= (pins.cePin != p);
        pins.cePin = p;
    }
    if (json.containsKey("tz")) {
        int p = json["tz"];
        res |= (tz != p);
        tz = p;
    }

    return res;
}
