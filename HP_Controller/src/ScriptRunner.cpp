#include "ScriptRunner.h"
#include "Configuration.h"

extern Configuration Config;

void ScriptRunner::HeatScript()
{
	switch (step)
	{
	case STEP_HEATER_0_IDLE:
		heaterIdle();
		break;
	case STEP_HEATER_1_INITIAL:
		heaterStepInitial();
		break;
	}
}

void ScriptRunner::heaterIdle() {
	static unsigned long start = 0;
	unsigned long now = millis();
	

	if (alertCode == Configuration::ALERT_EMPTY)
	{// start!
		step = STEP_HEATER_1_INITIAL;
	} else if (alertCode == Configuration::ALERT_STEP_TOO_LONG) {
		//waiting for 3 min
		if (start == 0) {
			start = now;
		} else {
			if (now - start >= 3 * 60 * 1000) {
				start = 0;
				step = STEP_HEATER_1_INITIAL;
				publishAlert(Configuration::ALERT_EMPTY);
			}
		}
	}
}

void ScriptRunner::Loop(unsigned long timeperiod)
{
	if (mode == Configuration::MODE_HEAT)
	{
		HeatScript();
	}
}

void ScriptRunner::Init()
{
	mode = Config.GetHeatMode(); // mode can't be changed during a workcycle
}


void ScriptRunner::publishAlert(Configuration::ALERTCODE code) {
	
	if (code != alertCode) {

		Config.PublishAlert(code);
		alertCode = code;
	}
}
/*
bool ScriptRunner::checkElectricityFail()
{
	bool res = !Config.DevMgr->VoltageSwitch.IsOk();

	return res;
}

bool ScriptRunner::checkPressureFail()
{
	static bool fail = false;
	bool res = !Config.DevMgr->PressureSwitch.IsOk();
	return res;
}

bool ScriptRunner::checkInternalTempFail()
{
	static bool fail = false;
	bool res = !Config.DevMgr->TCompressor.IsOk() || !Config.DevMgr->TVapOut.IsOk() || !Config.DevMgr->TCondIn.IsOk() || !Config.DevMgr->TCondVap.IsOk();
	return res;
}

bool ScriptRunner::checkGroundTempFail()
{
	static bool fail = false;
	bool res = !Config.DevMgr->TGndIn.IsOk() || !Config.DevMgr->TGndOut.IsOk();
	return res;
}

bool ScriptRunner::checkInsideTempFail()
{
	static bool fail = false;
	bool res = !Config.DevMgr->TOut.IsOk() || !Config.DevMgr->TIn.IsOk() || !Config.DevMgr->TTankIn.IsOk() || !Config.DevMgr->TTankOut.IsOk() || !Config.DevMgr->TInside.IsOk();
	return res;
}

*/

bool ScriptRunner::heaterStepInitial()
{
	bool res = false;
	bool res1 = false;
	bool res2 = false;

	static unsigned long start = 0;
	unsigned long now = millis();

	if (start == 0)
	{
		start = now;
	}
	if (step1Long!=NAN &&  now - start >= step1Long) {
		publishAlert(Configuration::ALERT_STEP_TOO_LONG);
		res1 = Config.DevMgr->PumpFloor1.ProcessUnit(ACT_OFF);
		res2 = Config.DevMgr->PumpFloor2.ProcessUnit(ACT_OFF);
		
		res = res1 & res2;
		if (res) {
			step = STEP_HEATER_0_IDLE;
		}
	}

	
	if (Config.DevMgr->TTankOut.IsOk() && Config.DevMgr->TTankIn.IsOk())
	{
		if (!res1)
		{
			res1 = Config.DevMgr->PumpFloor1.ProcessUnit(ACT_ON);
		}
		if (!res2)
		{
			res2 = Config.DevMgr->PumpFloor2.ProcessUnit(ACT_ON);
		}

		if (res1 && res2)
		{
			res = true;
			step = STEP_HEATER_2_CHECK_START;
			publishStep();
			publishAlert(Configuration::ALERT_EMPTY);
		}
	}
		publishInfo("Waiting for floor pums are started");
	

	return res;
}


void ScriptRunner::publishStep()
{
	sprintf(Config.PayloadBuff, "%c", step);
	sprintf(Config.TopicBuff, MQTT_STEP, Config.BoardId());
	Config.Publish();
}


void ScriptRunner::publishInfo(const char* msg) {
	sprintf(Config.PayloadBuff, "Step: %c, %s", step, msg);
	sprintf(Config.TopicBuff, MQTT_INFO, Config.BoardId());
	Config.Publish();
}
