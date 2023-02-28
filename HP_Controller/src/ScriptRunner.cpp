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
	default:
		Config.Log->Error("Wrong Step");
		break;
	}
}

bool ScriptRunner::heaterEmptyStep() {
	bool res = false;
	static unsigned long start = 0;
	unsigned long now = millis();

	if (start == 0) {
		start = now;
		Config.Log->append("Step: Empty, code=").append(step).Info();
		publishStep();
	}
	if (heatMode == HEATMODE_HEAT) {
		step = STEP_HEATER_0_IDLE;
		res = true;
	}
	if (res) {
		start = 0;
	}
	return res;
}

bool ScriptRunner::heaterIdle() {
	static unsigned long start = 0;
	unsigned long now = millis();
	bool res = false;

	if (start == 0) {
		start = now;
		Config.Log->append("Step: Idle, code=").append(step).Info();
		publishStep();
	}

	if ((alertCode == ALERT_EMPTY
		|| alertCode == ALERT_STEP_TOO_LONG)) {
		publishAlert(ALERT_EMPTY);

		if (Config.GetCommand() == CMD_RUN) {
			// start!
			step = STEP_HEATER_1_INITIAL;
			res = true;
		} else if (Config.GetCommand() == CMD_STOP) {
			step = STEP_HEATER_FULLSTOP;
			res = true;
		} else if (Config.GetCommand() == CMD_NOCMD) {
			step = STEP_EMPTY;
			res = true;
		}
	} else {
		// some alert. Do Nothing. Waiting for this alert is disappeared
	}
	if (res) {
		start = 0;
	}
	return res;
}

void ScriptRunner::Loop(unsigned long timeperiod) {
	//Config.Log->Debug("Runner Loop");
	if (heatMode == HEATMODE_HEAT) {
		HeatScript();
	}
}

void ScriptRunner::Init() {
	heatMode = Config.GetHeatMode(); // mode can't be changed during a workcycle
	Config.Log->append("Mode=").append(heatMode).Debug();
	step = STEP_EMPTY;
}


void ScriptRunner::publishAlert(ALERTCODE code) {

	if (code != alertCode) {

		Config.PublishAlert(code);
		alertCode = code;
	}
}

bool ScriptRunner::heaterStepInitial() {
	bool res = false;

	const unsigned long stepLong = 10 * 60 * (unsigned long)1000;
	static unsigned long start = 0;
	unsigned long now = millis();

	if (start == 0) {
		start = now;
		Config.Log->append("Step: Initial, code=").append(step).Info();
		publishStep();
	}

	if (checkConditions()) {
		if (now - start >= stepLong) {
			publishAlert(ALERT_STEP_TOO_LONG);
			prevStep = step;
			step = STEP_HEATER_FULLSTOP;
			res = true;
		} else {
			bool res1 = false;
			bool res2 = false;
			res1 = Config.DevMgr->PumpFloor1.ProcessUnit(ACT_ON);
			if (res1) {
				publishInfo("Floor 1 pump has been started");
			}
			res2 = Config.DevMgr->PumpFloor2.ProcessUnit(ACT_ON);
			if (res2) {
				publishInfo("Floor 2 pump has been started");
			}
			res = res1 & res2;
			if (res) {
				publishInfo("Both floor's pumps have been started");
				step = STEP_HEATER_2_CHECK_START;
				publishAlert(ALERT_EMPTY);
			} else {
				publishInfo("Waiting for floor pums are started");				
			}
		}
	}

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

bool ScriptRunner::checkConditions() {
	bool res = true;
	switch (step) {
		// it shouldn't break in some lines!
	case STEP_HEATER_FULLSTOP:
		break;
	case STEP_HEATER_2_CHECK_START:
		// Floor pumps are running
		res &= Config.DevMgr->PumpFloor1.IsOk();
		res &= Config.DevMgr->PumpFloor2.IsOk();		
	case STEP_HEATER_1_INITIAL:
		// Stop for any alert
		res &= alertCode == ALERT_EMPTY;
		// Check Temperature
		res &= Config.DevMgr->TTankIn.IsOk();
		res &= Config.DevMgr->TTankOut.IsOk();		
	case STEP_HEATER_0_IDLE:
		// Command
		res &= Config.GetCommand() == CMD_RUN;
	case STEP_EMPTY:	
		// Voltage check
		res &= Config.DevMgr->VoltageSwitch.IsOk();
	}
	return res;
}

bool ScriptRunner::heaterStepCheckStart() {
	bool res = false;
	static unsigned long start = 0;
	//unsigned long now = millis();

	if (start == 0) {
		start = 1;
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
		Config.Log->append("Step: FULL STOP, code=").append(step).Info();
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
