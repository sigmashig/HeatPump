#pragma once

#include <Arduino.h>

#include "definitions.h"


class ScriptRunner
{
public:
	typedef enum {
		STEP_EMPTY = 0,
		STEP_HEATER_0_IDLE = '0',
		STEP_HEATER_1_INITIAL='I',
		STEP_HEATER_2_CHECK_START='S',
		STEP_HEATER_3_GND_START='G',
		STEP_HEATER_4_HEAT='H',
		STEP_HEATER_5_STOP_COMPRESSOR='C',
		//STEP_HEATER_6_STOP_HEATING='T',
		//STEP_HEATER_E1_STOP='1',
		//STEP_HEATER_E2_STOP='2',
		//STEP_HEATER_E3_STOP = '3',
		STEP_HEATER_FULLSTOP = 'Z'
	} STEPS;

	
	void Loop(unsigned long timeperiod);
	void StepScript();
 //	bool IsAlert = false;
	void Init();

private:
	STEPS step;
	STEPS prevStep;
	HEATMODE heatMode;
	ALERTCODE alertCode = ALERT_EMPTY;

	bool emptyStep();
	bool heaterIdle();
	bool heaterStepInitial();
    bool checkCommand();
	bool heaterStepCheckStart();
    bool heaterStepGroundStart();
    bool heaterStepHeat();
    bool checkAlarm();
	bool heaterFullStop();

    bool checkStartHeating();


	void publishStep();
	void publishAlert(ALERTCODE code);
	void publishInfo(const char* msg);
	bool checkConditions();
	
};

