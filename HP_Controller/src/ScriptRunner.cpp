#include "ScriptRunner.h"
#include "Configuration.h"

extern Configuration Config;

void ScriptRunner::HeatScript() {
	switch (step) {
	case STEP_EMPTY:
		heaterEmptyStep();
	case STEP_HEATER_0_IDLE:
		heaterIdle();
		break;
	case STEP_HEATER_1_INITIAL:
		heaterStepInitial();
		break;
	case STEP_HEATER_2_CHECK_START:
		heaterStepCheckStart();
		break;
		//case STEP_HEATER_3_GND_START:

	case STEP_HEATER_FULLSTOP:
		heaterFullStop();
		break;
	}
}

bool ScriptRunner::heaterEmptyStep() {
	bool res = false;
	static unsigned long start = 0;
	//unsigned long now = millis();

	if (start == 0) {
		publishStep();
	}
	if (mode == MODE_HEAT) {
		step = STEP_HEATER_0_IDLE;
		res = true;
	}
	if (res) {
		start = 0;
	}
	return res;
}

bool ScriptRunner::heaterIdle() {
	//static unsigned long start = 0;
	//unsigned long now = millis();
	bool res = false;


	if ((alertCode == ALERT_EMPTY
		|| alertCode == ALERT_STEP_TOO_LONG)
		&& Config.GetCommand() == CMD_RUN) {// start!

		publishAlert(ALERT_EMPTY);
		step = STEP_HEATER_1_INITIAL;
		res = true;
	} else if (Config.GetCommand() == CMD_STOP) {
		publishAlert(ALERT_EMPTY);
		step = STEP_HEATER_FULLSTOP;
		res = true;
	}
	return res;
}

void ScriptRunner::Loop(unsigned long timeperiod) {
	if (mode == MODE_HEAT) {
		HeatScript();
	}
}

void ScriptRunner::Init() {
	mode = Config.GetHeatMode(); // mode can't be changed during a workcycle
	step = STEP_EMPTY;
}


void ScriptRunner::publishAlert(ALERTCODE code) {

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

bool ScriptRunner::heaterStepInitial() {
	bool res = false;
	bool res1 = false;
	bool res2 = false;

	static unsigned long start = 0;
	unsigned long now = millis();

	if (start == 0) {
		start = now;
		publishStep();
	}
	if (step1Long != NAN && now - start >= step1Long) {
		publishAlert(ALERT_STEP_TOO_LONG);
		res1 = Config.DevMgr->PumpFloor1.ProcessUnit(ACT_OFF);
		res2 = Config.DevMgr->PumpFloor2.ProcessUnit(ACT_OFF);

		res = res1 & res2;
		if (res) {
			step = STEP_HEATER_0_IDLE;
		}
	}

	if (Config.DevMgr->TTankOut.IsOk() && Config.DevMgr->TTankIn.IsOk()) {
		if (!res1) {
			res1 = Config.DevMgr->PumpFloor1.ProcessUnit(ACT_ON);
		}
		if (!res2) {
			res2 = Config.DevMgr->PumpFloor2.ProcessUnit(ACT_ON);
		}

		res = res1 & res2;
		if (res) {
			step = STEP_HEATER_2_CHECK_START;
			//step = STEP_HEATER_FULLSTOP;
			publishAlert(ALERT_EMPTY);
		}
	}
	publishInfo("Waiting for floor pums are started");

	if (res) {
		start = 0;
	}
	return res;
}



void ScriptRunner::publishStep() {
	sprintf(Config.PayloadBuff, "%c", step);
	sprintf(Config.TopicBuff, MQTT_STEP, Config.BoardId());
	Config.Publish();
}


void ScriptRunner::publishInfo(const char* msg) {
	sprintf(Config.PayloadBuff, "Step: %c, %s", step, msg);
	sprintf(Config.TopicBuff, MQTT_INFO, Config.BoardId());
	Config.Publish();
}

bool ScriptRunner::heaterStepCheckStart() {
	bool res = false;
	static unsigned long start = 0;
	//unsigned long now = millis();

	if (start == 0) {
		publishStep();
	}

	if (res) {
		start = 0;
	}
	return res;
}

bool ScriptRunner::heaterFullStop() {
	bool res = false;
	static unsigned long start = 0;
	//unsigned long now = millis();

	if (start == 0) {
		publishStep();
		publishInfo("FULL STOP!!!");
	}
	//Config.SetCommand(CMD_STOP);

	if (Config.GetCommand() == CMD_NOCMD || Config.GetCommand() == CMD_RUN) {
		step = STEP_HEATER_0_IDLE;
		res = true;
	}
	if (res) {
		start = 0;
	}
	return res;
}
