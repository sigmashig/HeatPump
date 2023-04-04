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
		STEP_HEATER_4_HEAT = 'H',
		STEP_HEATER_5_CHECK_STOP = 'h',
		STEP_HEATER_6_STOP_COMPRESSOR = 'C',
		STEP_HEATER_7_STOP_PUMPS='T',
		STEP_HEATER_FULLSTOP = 'Z',

		STEP_COOLER_0_IDLE = '9',
		STEP_COOLER_1_INITIAL = 'i',
		STEP_COOLER_2_CHECK_START = 's',
		STEP_COOLER_3_GND_START = 'g',
		STEP_COOLER_4_CHECK_STOP = 'c',
		STEP_COOLER_5_STOP = 't',
		
		STEP_COOLER_FULLSTOP = 'z'

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
    bool heaterStepCheckStop();
    bool heaterStepStopCompressor();
    bool heaterStepStopGnd();
    bool checkAlarm();
	bool heaterFullStop();

    bool coolerIdle();
    bool coolerStepInitial();
	bool coolerStepCheckStart();
    bool coolerStepGroundStart();

    bool coolerStepCheckStop();

    bool coolerStepStop();

    bool coolerFullStop();

    bool coolerStepCool();

	bool checkStopGround();

    bool checkStopHeating();

    bool checkStartHeating();

    bool checkStartCooling();

    bool checkStopCooling();


	void publishStep();
	void publishAlert(ALERTCODE code);
	void publishInfo(const char* msg);
	bool checkConditions();
	
};

