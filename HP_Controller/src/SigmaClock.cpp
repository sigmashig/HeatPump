#include "SigmaClock.h"
#include "ArduinoJson.h"
#include "Configuration.h"
#include "SigmaEEPROM.h"
//#include "MemoryExplorer.h"

extern Configuration Config;


SigmaClock::SigmaClock(EthernetClient* cli, const char* timezone) {
    client = cli;
    if (timezone == NULL) {
        SigmaEEPROM::ReadTimezone(tz);
    } else {
        strcpy(tz, timezone);
    }
    SetTimezone(tz);

    if (!rtc.begin()) {
        Config.Log->Error("DS3231 not found");
    }
    SyncClock();
}

bool SigmaClock::SyncClock() {
    bool res = false;
    if (Config.IsEthernetReady()) {
        if (readClock()) {
            SetClock();
        }
    }
    return res;
}



DateTime& SigmaClock::GetClock() {
    dt = rtc.getTime();
    return dt;
}

void SigmaClock::SetClock(DateTime& dt)
{
    rtc.setTime(dt);
}

void SigmaClock::SetClock()
{   
    SetClock(dt);
}

const char* SigmaClock::PrintClock()
{
    GetClock();
    sprintf(strClock, "%u-%02u-%02u %02u:%02u:%02u", dt.year, dt.month, dt.date, dt.hour, dt.minute, dt.second);
    return strClock;
}

void SigmaClock::SetTimezone(const char* tzNew)
{
    if (strcmp(tzNew, tz) != 0 && strlen(tzNew) != 0) {
        strcpy(tz, tzNew);
        SigmaEEPROM::WriteTimezone(tz);
        SyncClock();
    }
}

void SigmaClock::Init()
{
}

bool SigmaClock::readClock()
{
    int numbTry = 0;
    int len = 0;
    bool res = false;
    int connRes;
    char buf[100];
    
    while (numbTry < 5 && len == 0) {
        numbTry++;
        connRes = client->connect(server, 80);
        if (connRes) {
            IPAddress ip = client->remoteIP();
            Config.Log->append(F("Connected to ")).append(ip[0]).append(".").append(ip[1]).append(".").append(ip[2]).append(".").append(ip[3]).Info();
            // Make a HTTP request:
            sprintf(buf, CALENDAR_SERVER_PATH, tz);
            client->println(buf);
            sprintf(buf, "Host: %s", server);
            client->println(buf);
            client->println("Connection: close");
            client->println();

            int nTry = 0;
            len = client->available();
            while (nTry <= 5 && len == 0) {
                delay(2000);
                nTry++;
                len = client->available();
               //Config.Log->append("Try:").append(nTry).append("; response len=").append(len).Info();
            }
        }
        else {
            Config.Log->append("Can't connect...").append(connRes).Info();
        }
    }
    if (len == 0) {
        Config.Log->Error("Time server doesn't respond");
        return res;
    }
    res = parseResponse(len);
    Config.Log->append("Response:").append(res).Debug();
    return res;
}

bool SigmaClock::parseResponse(int len) {
    bool res = false;
 
    String s;

    s = client->readStringUntil('\n');
    if (s.indexOf("200 OK") == -1) {
        Config.Log->append("Response:").append(s.c_str()).Error();
        return false;
    }
    while (client->available()) {
        s = client->readStringUntil('\n');
        //Config.Log->append("s=").append(s.c_str()).Debug();
    }
    //Config.Log->append("Final=").append(s.c_str()).Debug();

    res = parseJson(s.c_str());
    
    return res;
}



bool SigmaClock::parseJson(const char* buf)
{
    //Config.Log->append("JSON:").append(buf).Debug();
    bool res = false;
    if (buf[0] != 0) {
        //const size_t CAPACITY = JSON_OBJECT_SIZE(25);
        StaticJsonDocument<400> doc;
        //DynamicJsonDocument doc(200);
        DeserializationError error = deserializeJson(doc, buf);
        if (error) {
            Config.Log->append("JSON Error=").append(error.f_str()).Error();
            return false;
        }
        // extract the data
        JsonObject root = doc.as<JsonObject>();
        
        if (root.containsKey("datetime")) {
 
            String s = root["datetime"]; //"2022-01-09T15:32:39.409582+02:00"
            dt.year = s.substring(0, 4).toInt();
            dt.month = s.substring(5, 7).toInt();
            dt.date = s.substring(8, 10).toInt();
            dt.hour = s.substring(11, 13).toInt();
            dt.minute = s.substring(14, 16).toInt();
            dt.second = s.substring(17, 19).toInt();
            
            res = true;
        }
        //else if (root.containsKey("day_of_week")) {
        //    tm.Wday = root["day_of_week"];
        //    tm.Wday += 1; //Sunday is 1
        //}
    }
//    Config.Log->append("DT=").append(PrintClock(t)).Debug();
    return res;
}

byte SigmaClock::DayYesterday(byte day) {
    if (day == 1) {
        day = 7;
    } else {
        day--;
    }
    return day;
}
