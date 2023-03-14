#include "SigmaClock.h"
#include "ArduinoJson.h"
//#include "Configuration.h"
#include "MemoryExplorer.h"


bool SigmaClock::IsDateTimeValid(DateTime& dt) {
    if (dt.year < 2023 || dt.year > 2050) {
        return false;
    }
    if (dt.month < 1 || dt.month > 12) {
        return false;
    }
    if (dt.day < 1 || dt.day > 31) {
        return false;
    }
    if (dt.hour < 0 || dt.hour > 23) {
        return false;
    }
    if (dt.minute < 0 || dt.minute > 59) {
        return false;
    }
    if (dt.second < 0 || dt.second > 59) {
        return false;
    }
    return true;
}


bool SigmaClock::SyncClock(CalendarServerType type, const char* timezone) {
    bool res = false;
    memoryReport("SyncClock");
    if (Ethernet.linkStatus() != LinkOFF) {
        memoryReport("SyncClock_link");
        switch (type) {
        case CAL_SERVER_WORLDTIMEAPI:
            memoryReport("SyncClock_WORLDTIMEAPI");
            res = readWorldTimeApi(timezone);
                break;
        }
    }
    return res;
}
bool SigmaClock::readWorldTimeApi(const char* timezone) {
    bool res = false;
    memoryReport("readWorldTimeApi_0");
    
    EthernetClient* client = new EthernetClient();
    
    //client->setTimeout(5000);
    memoryReport("readWorldTimeApi");
    char buf[50];
#define CALENDAR_SERVER "213.188.196.246"
//#define CALENDAR_SERVER "worldtimeapi.org"
#define CALENDAR_SERVER_PATH "/api/timezone/%s"
    const char* server = CALENDAR_SERVER;

    if (timezone == NULL) {
        sprintf(buf, CALENDAR_SERVER_PATH, "EET");
    } else {
        sprintf(buf, CALENDAR_SERVER_PATH, timezone);
    }

    DateTime dt;
    
    if (httpConnection(client, server, buf, 80)) {
        //Serial.println("Connected to server");
        Serial.println("Reading response");
        dt = worldTimeApiParseResponse(client);
    }

    if (dt.year != 0) {
        SetClock(dt);
    }
    client->stop();
    delete client;

    return res;
}

DateTime SigmaClock::worldTimeApiParseResponse(EthernetClient* client) {
    char buf[400];
    DateTime dt;

    if (extractBody(client, buf)) {
        Serial.println("Body extracted");
        Serial.println(buf);
        dt = worldTimeApiParseJson(buf);
    }

    return dt;
}

bool SigmaClock::extractBody(EthernetClient* client, char* body) {
    bool res = false;
    memoryReport("extractBody");
    if (client->available()) {
        
        String s;
        s = client->readStringUntil('\n');
        Serial.println(s);
        if (s.indexOf("200 OK") != -1) {
            while (client->available()) {
                s = client->readStringUntil('\n');
                Serial.println(s);
            }
            Serial.print("len=");
            Serial.println(s.length());
            Serial.print("Body=");
            Serial.println(s.c_str());
            if (s.length() != 0) {
                strcpy(body, s.c_str());
                res = true;
            }
        }
        memoryReport("extractBody_end");
    }
    return res;
}



DateTime SigmaClock::worldTimeApiParseJson(const char* buf) {
    DateTime dt;
    if (buf[0] != 0) {
        //const size_t CAPACITY = JSON_OBJECT_SIZE(25);
        //StaticJsonDocument<400> doc;
        DynamicJsonDocument doc(400);
        DeserializationError error = deserializeJson(doc, buf);
        if (!error) {

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

                //  res = true;
            }
            //else if (root.containsKey("day_of_week")) {
            //    tm.Wday = root["day_of_week"];
            //    tm.Wday += 1; //Sunday is 1
            //}
        }else {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
        }
        
    }
    return dt;
}



bool SigmaClock::httpConnection(EthernetClient* client, const char* url, const char* path, unsigned long port) {

    int numbTry = 0;
    int len = 0;
    bool res = false;
    unsigned int connRes;

    Serial.println("Connecting to server");
    while (numbTry < 3 && len == 0) {
        numbTry++;

        Serial.println(numbTry);
        Serial.println(url);
        connRes = client->connect(url, port);
        if (connRes) {
            // Make a HTTP request:
            Serial.println("Connected to server");
            char buf[100];
            sprintf(buf, "GET %s HTTP/1.1", path);
            client->println(buf);
            sprintf(buf, "Host: %s", url);
            client->println(buf);
            client->println("Connection: close");
            client->println();

            int nTry = 0;
            len = client->available();
            Serial.println(len);
            while (nTry <= 5 && len == 0) {
                delay(2000);
                nTry++;
                len = client->available();
            }
            Serial.print("len=");
            Serial.println(len);
            
        } else {
            Serial.println("Connection failed");
            break;
        }
    }
    Serial.println("Connection done");
    if (len != 0) {
        res = true;
    }
    return res;
}



DateTime SigmaClock::GetClock() {
    MicroDS3231 rtc;
    DateTime dt = rtc.getTime();
    return dt;
}



void SigmaClock::SetClock(DateTime& dt)
{
    if (IsDateTimeValid(dt)) {
        MicroDS3231 rtc;
        rtc.setTime(dt);
    }
}

/*
const char* SigmaClock::PrintClock()
{
    GetClock();
    sprintf(strClock, "%u-%02u-%02u %02u:%02u:%02u", dt.year, dt.month, dt.date, dt.hour, dt.minute, dt.second);
    return strClock;
}
*/

byte SigmaClock::DayYesterday(byte day) {
    if (day == 1) {
        day = 7;
    } else {
        day--;
    }
    return day;
}

const char* SigmaClock::PrintClock() {
    static char strClock[30];
    DateTime dt = GetClock();
    sprintf(strClock, "%u-%02u-%02u %02u:%02u:%02u, day=%u", dt.year, dt.month, dt.date, dt.hour, dt.minute, dt.second, dt.day);
    return strClock;    
}
