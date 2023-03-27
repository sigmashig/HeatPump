#include "RTC.hpp"
#include "Configuration.h"

extern Configuration Config;

RTC::RTC(const char* nm) : Unit(DEVTYPE_CLOCK, nm) {
}

void RTC::InitUnit() {
    time_t tm = SigmaClock::SyncClock();
    SigmaClock::SetClock(tm, tz, type, pins);
}

void RTC::UnitLoop(unsigned long timePeriod) {
    if (timePeriod % 1000 == 0 && Config.Counter60!= 0  && Config.Counter60 %(60*24)==0 ) {
        time_t t = SigmaClock::SyncClock();
        SigmaClock::SetClock(t, tz, type, pins);
    }
}

tm RTC::GetTime() {
    return SigmaClock::GetClock(type, pins);
}


bool RTC::UpdateEquipment(const char* line) {
    // {'type':'DS1302','datPin':14,'clkPin':15,'cePin':16, 'tz':'2'}
    bool res = false;
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
