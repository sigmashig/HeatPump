#pragma once
#include <Ethernet.h>
#include "TimeLib.h"
#include "DS1307RTC.h"

// #define CALENDAR_SERVER "worldtimeapi.org"
#define CALENDAR_SERVER "213.188.196.246"
#define CALENDAR_SERVER_PATH "GET /api/timezone/%s HTTP/1.1"
#define BUF_SIZE	512
#define HTTP_CONTENT_LENGTH "content-length:"
#define TIMEZONE_LEN	50

class SigmaClock
{
public:
	SigmaClock(EthernetClient* cli, const char* timezone = NULL);
	bool SyncClock();
	TimeElements GetClock();
//	bool GetClock(TimeElements& tm, bool isInternet=false);
	bool SetClock(TimeElements tm);
	bool SetClock();
	const char* PrintClock();
	void SetTimezone(const char* tzNew);
	const char* GetTimezone() { return tz; };
	void Init();

private:
	TimeElements tm;
	EthernetClient* client;
	bool readClock();
	bool parseResponse(int len);
	bool parseJson(const char* buf);
	const char* server = CALENDAR_SERVER;
	char tz[TIMEZONE_LEN];
	//char buf[BUF_SIZE];
	char strClock[25];
};

