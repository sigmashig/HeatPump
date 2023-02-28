#pragma once

#include <Arduino.h>

#include "definitions.h"

/*
#define PAUSE_STEP0	(unsigned long)5000//2*60*1000
#define PAUSE_STEP2	(unsigned long)5000//2*60*1000
#define PAUSE_STEP6	(unsigned long)5000//2*60*1000
#define MIN_INTERVAL_COMPRESSOR		5000//(unsigned long)5*60*1000
*/

class ScriptRunner
{
public:
	typedef enum {
		STEP_EMPTY = 0,
		STEP_HEATER_0_IDLE = '0',
		STEP_HEATER_1_INITIAL='I',
		STEP_HEATER_2_CHECK_START='S',
		//STEP_HEATER_3_GND_START='G',
		//STEP_HEATER_4_HEAT='H',
		//STEP_HEATER_5_STOP_COMPRESSOR='C',
		//STEP_HEATER_6_STOP_HEATING='T',
		//STEP_HEATER_E1_STOP='1',
		//STEP_HEATER_E2_STOP='2',
		//STEP_HEATER_E3_STOP = '3',
		STEP_HEATER_FULLSTOP = 'Z'
	} STEPS;

	
	void Loop(unsigned long timeperiod);
	void HeatScript();
 //	bool IsAlert = false;
	void Init();

private:
	STEPS step;
	STEPS prevStep;
	HEATMODE heatMode;
	ALERTCODE alertCode = ALERT_EMPTY;

	bool heaterEmptyStep();
	bool heaterIdle();
	bool heaterStepInitial();
	bool heaterStepCheckStart();
	bool heaterFullStop();


	void publishStep();
	void publishAlert(ALERTCODE code);
	void publishInfo(const char* msg);
	bool checkConditions();
	
};

