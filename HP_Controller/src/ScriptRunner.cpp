#include "ScriptRunner.h"
#include "Configuration.h"

extern Configuration Config;

void ScriptRunner::Loop(unsigned long timeperiod) {
	//Config.Log->Debug("Runner Loop");
	StepScript();
}

void ScriptRunner::Init() {
//	heatMode = Config.GetHeatMode(); // mode can't be changed during a workcycle
	Config.Log->append("Mode=").append(heatMode).Debug();
	step = STEP_EMPTY;
}

void ScriptRunner::StepScript() {
	switch (step) {
	case STEP_EMPTY:
		emptyStep();
		break;
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

bool ScriptRunner::emptyStep() {
	bool res = false;
	static unsigned long start = 0;
	unsigned long now = millis();

	if (start == 0) {
		start = now;
		Config.Log->append("Step: Empty, code=").append(step).Info();
		publishStep();
	}
	if (checkConditions()) {
		if (Config.GetHeatMode() == HEATMODE_HEAT) {
			step = STEP_HEATER_0_IDLE;
			res = true;
		}
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
	if (checkConditions()) {
		if (Config.GetCommand() == CMD_RUN) {
			// start!
			step = STEP_HEATER_1_INITIAL;
			res = true;
		} else if (Config.GetCommand() == CMD_STOP) {
			prevStep = step;
			step = STEP_HEATER_FULLSTOP;
			res = true;
		} else if (Config.GetCommand() == CMD_NOCMD) {
			//step = STEP_EMPTY;
			res = false;
		}

	} else {
		// some conditions are failed. Do Nothing. Waiting for these conditions are fixed no step ahead
	}
	if (res) {
		start = 0;
	}
	return res;
}


bool ScriptRunner::heaterStepInitial() {
	bool res = false;

	const unsigned long stepLong = 10 * 60 * (unsigned long)1000;
	static unsigned long start = 0;
	static bool infoMsg1 = false;
	static bool infoMsg2 = false;
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
		} else if (checkCommand()) {
			
			Config.DevMgr->PumpFloor1.ProcessUnit(ACT_ON);
			Config.DevMgr->PumpFloor2.ProcessUnit(ACT_ON);
			res = Config.DevMgr->PumpFloor1.IsOk() && Config.DevMgr->PumpFloor2.IsOk();
			if (res) {
				if (!infoMsg2) {
					publishInfo("Both floor's pumps have been started");
					infoMsg2 = true;
				}
					step = STEP_HEATER_2_CHECK_START;
			} else {
				if (!infoMsg1) {
					publishInfo("Waiting for floor pums are started");
					infoMsg1 = true;
				}
			}
		} else // CMD_STOP CMD_NOCMD
		{
			res = true;
		}	
	}

	if (res) {
		start = 0;
		infoMsg1 = false;
		infoMsg2 = false;
	}
	return res;
}


bool ScriptRunner::checkCommand() {
	bool res = true;
	
	if (Config.GetCommand() == CMD_RUN) {
		res = true;
	} else {
		prevStep = step;
		step = STEP_HEATER_FULLSTOP;
		res = false;
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
	bool res1;
	switch (step) {
		// it shouldn't break in some lines!
	case STEP_HEATER_FULLSTOP:
		break;
	case STEP_HEATER_2_CHECK_START:
		// Floor pumps are running
		res1 &= Config.DevMgr->PumpFloor1.IsOk();
		if (res1) {
			Config.DevMgr->PumpFloor1.PublishDeviceAlert(ALERT_NOT_RUNNING);
		}
		res &= res1;
		res1 = Config.DevMgr->PumpFloor2.IsOk();
		if (res1) {
			Config.DevMgr->PumpFloor2.PublishDeviceAlert(ALERT_NOT_RUNNING);
		}
		res &= res1;
	case STEP_HEATER_1_INITIAL:
		// Stop for any alert
		res &= alertCode == ALERT_EMPTY;
		// Check Temperature
		res1 = Config.DevMgr->TTankIn.IsOk();
		if (!res1) {
			Config.DevMgr->TTankIn.PublishDeviceAlert(ALERT_TEMP_IS_OUT_OF_RANGE);
		}
		res &= res1;

		res1 = Config.DevMgr->TTankOut.IsOk();
		if (!res1) {
			Config.DevMgr->TTankOut.PublishDeviceAlert(ALERT_TEMP_IS_OUT_OF_RANGE);
		}
		res &= res1;

	case STEP_HEATER_0_IDLE:
	case STEP_EMPTY:
		// Voltage check
		res1 = Config.DevMgr->VoltageSwitch.IsOk();
		if (!res1) {
			Config.DevMgr->VoltageSwitch.PublishDeviceAlert(ALERT_VOLTAGE_IS_OUT_OF_RANGE);
		}
		res &= res1;
		break;
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

void ScriptRunner::publishAlert(ALERTCODE code) {

	if (code != alertCode) {

		Config.PublishAlert(code);
		alertCode = code;
	}
}
