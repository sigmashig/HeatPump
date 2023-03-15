#pragma once
#include <Ethernet.h>
#include <microDS3231.h>
#include <time.h>

// #define CALENDAR_SERVER "worldtimeapi.org"
#define BUF_SIZE	512
//#define HTTP_CONTENT_LENGTH "content-length:"
#define TIMEZONE_LEN	50

class SigmaClock
{
public:
	
	typedef enum {
		CAL_SERVER_WORLDTIMEAPI,
		CAL_SERVER_NTP
	} CalendarServerType;
	

	static bool SyncClock(CalendarServerType type = CAL_SERVER_NTP, int tz = 2);
	static tm GetClock();
	static void SetClock(tm& tm0);
	static const char* PrintClock();
	static enum _WEEK_DAYS_ DayYesterday(enum _WEEK_DAYS_ day);

private:
	static tm fromDateTime(DateTime& dt);
	static DateTime toDateTime(tm& t);
	static tm worldTimeApiParseJson(const char* buf, int tz);
	static tm fromUnixEpoch(time_t t, int tz);
	static unsigned long getNtpTime();
	static bool readWorldTimeApi(int tz);
	static bool httpConnection(EthernetClient* client, const char* url, const char* path, unsigned long port = 80);
	static bool extractBody(EthernetClient* client, char* body);
	static tm worldTimeApiParseResponse(EthernetClient* client, int tz);
};

