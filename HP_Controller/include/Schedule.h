#pragma once
#include <Arduino.h>

#include "definitions.h"

class Schedule
{
public:
	double GetTemperature() { return temperature; };

	int CompareTime(byte hr, byte mn);
	
	void UpdateSchedule(byte hr, byte mn, double t) { hour = hr; minute = mn; temperature = t; };
	void UpdateSchedule(const char* line);
	void print(const char* header, DebugLevel level);
	
	byte GetHour() { return hour; };
	byte GetMinute() { return minute; };
	void Serialize(char* buffer);
private:
	bool isWeekEnd = false;
	byte hour = 0;
	byte minute = 0;
	double temperature = 25.0;
};

