#pragma once
#include "Schedule.h"
#include <SigmaClock.hpp>
#include <time.h>

class ScheduleManager
{
public:

	ScheduleManager();
	void Init();
	void FinalInit();
	void SubscribeSchedules();
	void UpdateSchedule(byte shift, byte setNumber, const char* payload);
 	double GetDesiredTemperature();
    double GetDesiredTemperature(byte h, byte m, byte day);
	double GetDesiredTemperature(tm& t);
	
private:
	bool sort = false;
	//Schedule workDays[CONFIG_NUMBER_SCHEDULES];
	//Schedule weekEnd[CONFIG_NUMBER_SCHEDULES];
	Schedule AllSchedule[2 * CONFIG_NUMBER_SCHEDULES];
	void readFromEEPROM();
	bool sortSchedule(byte shift);
	byte wdOrWe(tm& t) { return wdOrWe((_WEEK_DAYS_)(t.tm_wday)); };
	byte wdOrWe(_WEEK_DAYS_ day);
	//void publishSchedules();
	//Schedule& GetSchedule(byte shift, byte setNumber) { return AllSchedule[shift + setNumber]; };
	//Schedule& GetSchedule(byte setNumber) { return AllSchedule[setNumber]; };

};

