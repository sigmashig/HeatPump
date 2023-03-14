#pragma once
#include <Ethernet.h>
//#include "TimeLib.h"
#include <microDS3231.h>

// #define CALENDAR_SERVER "worldtimeapi.org"
//#define BUF_SIZE	512
//#define HTTP_CONTENT_LENGTH "content-length:"
#define TIMEZONE_LEN	50

class SigmaClock
{
public:
	
	typedef enum {
		CAL_SERVER_WORLDTIMEAPI
	} CalendarServerType;
	

	static bool SyncClock(CalendarServerType type = CAL_SERVER_WORLDTIMEAPI, const char* timezone = NULL);
	static DateTime GetClock();
	static void SetClock(DateTime& dt);
	static const char* PrintClock();
	static byte DayYesterday(byte day);
	static bool IsDateTimeValid(DateTime& dt);
	
private:
	static bool readWorldTimeApi(const char* timezone);
	static bool httpConnection(EthernetClient* client, const char* url, const char* path, unsigned long port = 80);
	static bool extractBody(EthernetClient* client, char* body);
	static DateTime worldTimeApiParseResponse(EthernetClient* client);
	static DateTime worldTimeApiParseJson(const char* buf);
};

