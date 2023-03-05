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
    SyncClock();
}

bool SigmaClock::SyncClock() {
    bool res = false;
    if (Config.IsEthernetReady()) {
        if (readClock()) {
            res = SetClock();
        }
    }
    return res;
}

TimeElements SigmaClock::GetClock() {
    
    if (RTC.read(tm)) {
        if (RTC.chipPresent()) {
            Config.Log->Error("The RTC is stopped");
        }
        else {
            Config.Log->Error("The RTC read error!  Please check the circuitry.");
        }
    }
    return tm;
}
/*
bool SigmaClock::GetClock(TimeElements& tm)
{
    bool res = false;
    if (isInternet) {
    }
    if (!res) {
        res = RTC.read(tm);
        if (!res) {
            if (RTC.chipPresent()) {
                Config.Log->Error("The RTC is stopped");
            }
            else {
                Config.Log->Error("The RTC read error!  Please check the circuitry.");
            }
        }
    }
	return res;
}
*/
bool SigmaClock::SetClock(TimeElements tm)
{
    bool res = RTC.write(tm);
    if (!res) {
        Config.Log->Error("Can't set RTC");
    }
    return res;
}

bool SigmaClock::SetClock()
{   
    bool res = false;

    res = SetClock(tm);
    return res;
}

const char* SigmaClock::PrintClock()
{
    sprintf(strClock, "%u-%02u-%02u %02u:%02u:%02u", tm.Year + 1970, tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second);
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
    //            Config.Log->append("Try:").append(nTry).append("; response len=").append(len).Info();
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
 	return res;
}

bool SigmaClock::parseResponse(int len)
{
    bool res = false;
    char buf[BUF_SIZE];

    String s;
    int payloadLen = 0;
 
    s = client->readString();

    int i = s.indexOf(HTTP_CONTENT_LENGTH);
    if (i != 0) {
        i += strlen(HTTP_CONTENT_LENGTH);
    }
    payloadLen = s.substring(i + 1).toInt();
//    Config.Log->append("Response length=").append(payloadLen).Debug();
    if (payloadLen >= BUF_SIZE) {
        Config.Log->append("The response size(").append(payloadLen).append(") is too high").Error();
        buf[0] = 0;
    }
    else if (payloadLen!=0) {       
        strncpy(buf, s.substring(s.length() - payloadLen,s.length()).c_str(),payloadLen);
        buf[payloadLen] = 0;
    }
    else {
        buf[0] = 0;
    }
    res = (buf[0] != 0);
    if (res) {
        res = parseJson(buf);
    }
    return res;
}

bool SigmaClock::parseJson(const char* buf)
{
    bool res = false;
    if (buf[0] != 0) {
        //const size_t CAPACITY = JSON_OBJECT_SIZE(25);
        StaticJsonDocument<400> doc;
        //DynamicJsonDocument doc(200);
        DeserializationError error = deserializeJson(doc, buf);
        if (error) {
            Config.Log->append("JSON Error=").append(error.f_str()).Debug();
            return false;
        }
        // extract the data
        JsonObject root = doc.as<JsonObject>();
        
        if (root.containsKey("datetime")) {
 
            String s = root["datetime"]; //"2022-01-09T15:32:39.409582+02:00"
            tm.Year = s.substring(0, 4).toInt()-1970;
            tm.Month = s.substring(5, 7).toInt();
            tm.Day = s.substring(8, 10).toInt();
            tm.Hour = s.substring(11, 13).toInt();
            tm.Minute = s.substring(14, 16).toInt();
            tm.Second = s.substring(17, 19).toInt();
            
            res = true;
        }
        else if (root.containsKey("day_of_week")) {
            tm.Wday = root["day_of_week"];
            tm.Wday += 1; //Sunday is 1
        }
    }
//    Config.Log->append("DT=").append(PrintClock(t)).Debug();
    return res;
}
