#pragma once

#include <Arduino.h>

#define PAUSE_STEP0	(unsigned long)5000//2*60*1000
#define PAUSE_STEP2	(unsigned long)5000//2*60*1000
#define PAUSE_STEP6	(unsigned long)5000//2*60*1000
#define MIN_INTERVAL_COMPRESSOR		5000//(unsigned long)5*60*1000

class ScriptRunner
{
public:
	typedef enum {
		STEP_HEATER_INITIAL='I',
		STEP_HEATER_START_PUMP='P',
		STEP_HEATER_START='S',
		STEP_HEATER_GND_START='G',
		STEP_HEATER_PAUSE2='2',
		STEP_HEATER_MIN_TIME='T',
		STEP_HEATER_MAIN_START='M',
		STEP_HEATER_MAIN_STOP='N',
		STEP_HEATER_PAUSE6='6',
		STEP_HEATER_GND_STOP='H'
	} STEPS;

	void HeatScript(unsigned long timeperiod);
	bool IsAlert = false;

private:
	STEPS step = STEP_HEATER_INITIAL;

	unsigned long lastStart = millis();

	bool heaterStepInitial();
	bool heaterStepPumpStart();
	bool heaterStepStart();
	bool heaterStepGndStart();
	bool heaterStepPause2();
	bool heaterStepMainStart();
	bool heaterStepMainStop();
	bool heaterStepPause6();
	bool heaterStepGndStop();
	bool heaterStepMinTime();
	void publishStep();
	void publishStepAlert(const char* msg);

	bool checkElectricityFail();
	bool checkPressureFail();
	bool checkInternalTempFail();
	bool checkGroundTempFail();
	bool checkInsideTempFail();
};

