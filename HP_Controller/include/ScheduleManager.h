#pragma once
#include "Schedule.h"

class ScheduleManager
{
public:

	ScheduleManager();
	void Init();
	void FinalInit();
	void SubscribeSchedules();
	void UpdateSchedule(byte shift, byte setNumber, const char* payload);
	double GetDesiredTemperature();
	Schedule& GetSchedule(byte shift, byte setNumber) { return AllSchedule[shift + setNumber]; };
	Schedule& GetSchedule(byte setNumber) { return AllSchedule[setNumber]; };
	
private:
	bool sort = false;
	Schedule AllSchedule[2 * CONFIG_NUMBER_SCHEDULES];
	void readFromEEPROM();
	bool sortSchedule(byte shift);
	void publishSchedules(byte shift);
};

