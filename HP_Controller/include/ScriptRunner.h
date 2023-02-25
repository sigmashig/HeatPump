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
		STEP_EMPTY = 0,
		STEP_HEATER_0_IDLE = '0',
		STEP_HEATER_1_INITIAL='I',
		STEP_HEATER_2_CHECK_START='S',
		STEP_HEATER_3_GND_START='G',
		STEP_HEATER_4_HEAT='H',
		STEP_HEATER_5_STOP_COMPRESSOR='C',
		STEP_HEATER_6_STOP_HEATING='T',
		STEP_HEATER_E1_STOP='1',
		STEP_HEATER_E2_STOP='2',
		STEP_HEATER_E3_STOP='3'
	} STEPS;

	
	void Loop(unsigned long timeperiod);
	void HeatScript();
//	bool IsAlert = false;
	void Init();

private:
	STEPS step;
	Configuration::HEATMODE mode;
	Configuration::ALERTCODE alertCode = Configuration::ALERT_EMPTY;

	const unsigned long step1Long = 0;
	const unsigned long step2Long = 0;
	const unsigned long step3Long = 10*60*1000;
	const unsigned long step4Long = 20*60*1000;

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
	void publishAlert(Configuration::ALERTCODE code);
	void publishInfo(const char* msg);

	bool checkElectricityFail();
	bool checkPressureFail();
	bool checkInternalTempFail();
	bool checkGroundTempFail();
	bool checkInsideTempFail();

	
	
};

