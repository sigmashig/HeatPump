#pragma once
#include <Arduino.h>

#include "definitions.h"

class Schedule
{
public:
	byte GetHour() { return hour; };
	byte GetMinute() { return minute; };
	double GetTemp() { return temperature; };

	void SetHour(byte hr) { hour = hr; };
	void SetMinute(byte mn) { minute = mn; };
	void SetTemp(double t) { temperature = t; };

	void UpdateSchedule(byte hr, byte mn, double t) { hour = hr; minute = mn; temperature = t; };
	void UpdateSchedule(const char* line);
	void print(const char* header, DebugLevel level);

private:
	byte hour = 0;
	byte minute = 0;
	double temperature = 25.0;

};

