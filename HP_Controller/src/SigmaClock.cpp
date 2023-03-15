#include "SigmaClock.h"
#include "ArduinoJson.h"
//#include "MemoryExplorer.h"

int ua_dst(const time_t* timer, int32_t* z) {
    struct tm       tmptr;
    uint8_t         month, mday, hour, day_of_week, d;
    int             n;

    /* obtain the variables */
    gmtime_r(timer, &tmptr);
    month = tmptr.tm_mon;
    day_of_week = tmptr.tm_wday;
    mday = tmptr.tm_mday - 1;
    hour = tmptr.tm_hour;

    if ((month > MARCH) && (month < OCTOBER))
        return ONE_HOUR;

    if (month < MARCH)
        return 0;
    if (month > OCTOBER)
        return 0;

    /* determine mday of last Sunday */
    n = tmptr.tm_mday - 1;
    n -= day_of_week;
    n += 7;
    d = n % 7;  /* date of first Sunday */

    n = 31 - d;
    n /= 7; /* number of Sundays left in the month */

    d = d + 7 * n;  /* mday of final Sunday */
    if (month == MARCH) {
        if (d < mday) {
            return ONE_HOUR;
        } else if (d > mday) {
            return 0;
        } else if (hour < 2) {
            return 0;
        }
        return ONE_HOUR;
    } else if (d < mday) {
        return 0;
    } else if (d > mday) {
        return ONE_HOUR;
    } else if (hour < 2) {
        return ONE_HOUR;
    }
    return 0;
}
/*
void SigmaClock::syncNTP() {
    EthernetUDP Udp;
    NTP ntp = NTP(Udp);
    Serial.println("Starting UDP");
    ntp.ruleDST("EEST", Last, Sun, Mar, 2, 180); // last sunday in march 2:00, timetone +180min (+2 GMT + 1h summertime offset)
    ntp.ruleSTD("EET", Last, Sun, Oct, 3, 120); // last sunday in october 3:00, timezone +120min (+2 GMT)
    Serial.println("begin");
    ntp.begin("time.nist.gov");
    //Serial.println("update");
    //ntp.update();
    Serial.println("formattedTime");
    Serial.println(ntp.formattedTime("%Y-%m-%d %H:%M:%S %w %Z %a %A")); // dd. Mmm yyyy
    ntp.stop();
}
*/

unsigned long SigmaClock::getNtpTime() {
    EthernetUDP Udp;
    unsigned int localPort = 8888;       // local port to listen for UDP packets
    char timeServer[] = "pool.ntp.org"; // NTP server
    int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
    byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

    unsigned long seconds = 0;
    Udp.begin(localPort);
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
 
    Udp.beginPacket(timeServer, 123); // NTP requests are to port 123
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();

    int nTry = 20;
    int ret = 0;
    while (ret != 48 && nTry > 0) {
        delay(100);
        ret = Udp.parsePacket();
        nTry--;
    }

    if (ret == 48) {
        // We've received a packet, read the data from it
        Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

        // the timestamp starts at byte 40 of the received packet and is four bytes,
        // or two words, long. First, extract the two words:

        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
        // combine the four bytes (two words) into a long integer
         // this is NTP time (seconds since Jan 1 1900):
        seconds = highWord << 16 | lowWord;
        seconds -= 2208988800UL; // subtract seventy years: 1970-1900
    }
    Udp.stop();

    return seconds;
}



bool SigmaClock::SyncClock(CalendarServerType type, int tz) {
    bool res = false;
    if (Ethernet.linkStatus() != LinkOFF) {
        switch (type) {
            case CAL_SERVER_WORLDTIMEAPI:
                res = readWorldTimeApi(tz);
                break;
            case CAL_SERVER_NTP:
                unsigned long t;
                t= getNtpTime();
                if (t>0) {
                    tm tm0 = fromUnixEpoch(t, tz);
                    SetClock(tm0);
                    res = true;
                }
                break;
        }
    }
    return res;
}

tm SigmaClock::fromUnixEpoch(time_t t, int tz) {
    tm tm0;
    set_zone(tz * ONE_HOUR);
    set_dst(ua_dst);
    localtime_r(&t, &tm0);
    return tm0;
}

bool SigmaClock::readWorldTimeApi(int tz) {
    bool res = true;
    EthernetClient* client = new EthernetClient();
    const char* server = "worldtimeapi.org";
    char* path = (char*)"/api/timezone/GMT";
    tm tm0;
    
    if (httpConnection(client, server, path, 80)) {
        tm0 = worldTimeApiParseResponse(client,tz);
    }
    if (tm0.tm_year >=2023 && tm0.tm_year<=2050) {
        SetClock(tm0);
    } else {
        res = false;
    }
    client->stop();
    delete client;
    return res;
}

tm SigmaClock::worldTimeApiParseResponse(EthernetClient* client, int tz) {
    char buf[512];
    tm tm0;

    if (extractBody(client, buf)) {
        tm0 = worldTimeApiParseJson(buf, tz);
    }
    return tm0;
}

bool SigmaClock::extractBody(EthernetClient* client, char* body) {
    bool res = false;
    String s;

    if (client->available()) {
        s = client->readStringUntil('\n');
        if (s.indexOf("200 OK") != -1) {
            while (client->available()) {
                s = client->readStringUntil('\n');
            }
            if (s.length() != 0) {
                strcpy(body, s.c_str());
                res = true;
            }
        }
    }
    return res;
}



tm SigmaClock::worldTimeApiParseJson(const char* buf, int tz) {
    tm tm0;
    if (buf[0] != 0) {
        //const size_t CAPACITY = JSON_OBJECT_SIZE(25);
        StaticJsonDocument<400> doc;
        //DynamicJsonDocument doc(200);
        DeserializationError error = deserializeJson(doc, buf);
        if (!error) {

            // extract the data
            JsonObject root = doc.as<JsonObject>();
            if (root.containsKey("unixtime")) {
                time_t t = root["unixtime"];
                tm0 = fromUnixEpoch(t, tz);
            }
            /*
            if (root.containsKey("datetime")) {

                String s = root["datetime"]; //"2022-01-09T15:32:39.409582+02:00"
                tm0.tm_year = s.substring(0, 4).toInt() - 1900;
                tm0.tm_mon = s.substring(5, 7).toInt() - 1;
                tm0.tm_mday = s.substring(8, 10).toInt() - 1;
                tm0.tm_hour = s.substring(11, 13).toInt();
                tm0.tm_min = s.substring(14, 16).toInt();
                tm0.tm_min = s.substring(17, 19).toInt();

                //  res = true;
            }
            else if (root.containsKey("day_of_week")) {
            //    tm.Wday = root["day_of_week"];
            //    tm.Wday += 1; //Sunday is 1
            //}
*/
        }
    }
    return tm0;
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



tm SigmaClock::GetClock() {
    MicroDS3231 rtc;
    DateTime dt = rtc.getTime();
    return fromDateTime(dt);
}


tm SigmaClock::fromDateTime(DateTime& dt) {
    tm t;
    t.tm_year = dt.year - 1900 - 30;
    t.tm_mon = dt.month - 1;
    t.tm_mday = dt.date;
    t.tm_hour = dt.hour;
    t.tm_min = dt.minute;
    t.tm_sec = dt.second;
    return t;
}

DateTime SigmaClock::toDateTime(tm& t) {
    DateTime dt;
    dt.year = t.tm_year + 1900;
    dt.month = t.tm_mon + 1;
    dt.date = t.tm_mday;
    dt.hour = t.tm_hour;
    dt.minute = t.tm_min;
    dt.second = t.tm_sec;
    return dt;
}

void SigmaClock::SetClock(tm& t)
{
    MicroDS3231 rtc;
    
    rtc.setTime(toDateTime(t));
}

/*
const char* SigmaClock::PrintClock()
{
    GetClock();
    sprintf(strClock, "%u-%02u-%02u %02u:%02u:%02u", dt.year, dt.month, dt.date, dt.hour, dt.minute, dt.second);
    return strClock;
}
*/

enum _WEEK_DAYS_ SigmaClock::DayYesterday(enum _WEEK_DAYS_ day) {
    if (day == MONDAY) {
        day = SUNDAY;
    } else {
        day = (_WEEK_DAYS_) (day - 1);
    }
    return day;
}

const char* SigmaClock::PrintClock() {
    static char strClock[30];
    tm t = GetClock();

    strftime(strClock, sizeof(strClock), "%Y-%m-%d %H:%M:%S %A (%w)", &t);
    return strClock;    
}


