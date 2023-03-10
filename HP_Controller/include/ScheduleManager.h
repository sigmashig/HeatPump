#pragma once
#include "Schedule.h"
#include <microDS3231.h>

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
	double GetDesiredTemperature(DateTime& dt);
	
private:
	bool sort = false;
	//Schedule workDays[CONFIG_NUMBER_SCHEDULES];
	//Schedule weekEnd[CONFIG_NUMBER_SCHEDULES];
	Schedule AllSchedule[2 * CONFIG_NUMBER_SCHEDULES];
	void readFromEEPROM();
	bool sortSchedule(byte shift);
	byte wdOrWe(DateTime& dt) { return wdOrWe(dt.day); };
	byte wdOrWe(byte day);
	//void publishSchedules();
	//Schedule& GetSchedule(byte shift, byte setNumber) { return AllSchedule[shift + setNumber]; };
	//Schedule& GetSchedule(byte setNumber) { return AllSchedule[setNumber]; };

};

