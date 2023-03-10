#pragma once
#include <Ethernet.h>
//#include "TimeLib.h"
#include <microDS3231.h>

// #define CALENDAR_SERVER "worldtimeapi.org"
#define CALENDAR_SERVER "213.188.196.246"
#define CALENDAR_SERVER_PATH "GET /api/timezone/%s HTTP/1.1"
#define BUF_SIZE	512
#define HTTP_CONTENT_LENGTH "content-length:"
#define TIMEZONE_LEN	50

class SigmaClock
{
public:
	
	typedef enum {
		CAL_SERVER_WORLDTIMEAPI
	} CalendarServerType;
	
	SigmaClock(EthernetClient* cli, const char* timezone = NULL);
	bool SyncClock();
	DateTime& GetClock();
//	bool GetClock(TimeElements& tm, bool isInternet=false);
	void SetClock(DateTime& dt);
	void SetClock();
	const char* PrintClock();
	void SetTimezone(const char* tzNew);
	const char* GetTimezone() { return tz; };
	void Init();
	void SetServerType(CalendarServerType type) { serverType = type; };
	CalendarServerType GetServerType() { return serverType; };
	static byte DayYesterday(byte day);
	
private:
	MicroDS3231 rtc;
	CalendarServerType serverType;
	DateTime dt;
	EthernetClient* client;
	bool readClock();
	bool parseResponse(int len);
	bool parseJson(const char* buf);
	const char* server = CALENDAR_SERVER;
	char tz[TIMEZONE_LEN];
	//char buf[BUF_SIZE];
	char strClock[25];
};

