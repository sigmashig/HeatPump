#include "ScriptRunner.h"
#include "Configuration.h"

extern Configuration Config;

void ScriptRunner::HeatScript(unsigned long timeperiod)
{
	if (step != STEP_HEATER_INITIAL) {
		checkElectricityFail();
		checkPressureFail();
		checkInternalTempFail();
		checkGroundTempFail();
		checkInsideTempFail();
	}

	switch (step) {
	case STEP_HEATER_START_PUMP:
		heaterStepPumpStart();
		break;
	case STEP_HEATER_INITIAL:
		heaterStepInitial();
		break;
	case STEP_HEATER_PAUSE2:
		heaterStepPause2();
		break;
	case STEP_HEATER_START:
		heaterStepStart();
		break;
	case STEP_HEATER_GND_START:
		heaterStepGndStart();
		break;
	case STEP_HEATER_GND_STOP:
		heaterStepGndStart();
		break;
	case STEP_HEATER_MAIN_STOP:
		heaterStepMainStop();
		break;
	case STEP_HEATER_MAIN_START:
		heaterStepMainStart();
		break;
	case STEP_HEATER_PAUSE6:
		heaterStepPause6();
		break;
	case STEP_HEATER_MIN_TIME:
		heaterStepMinTime();
	}
}

bool ScriptRunner::heaterStepPumpStart() {
	bool res = false;

	if (!checkInternalTempFail()) {
		Config.DevMgr->PumpTankOut.ProcessUnit(ACT_ON);
		Config.DevMgr->PumpFloor1.ProcessUnit(ACT_ON);
		Config.DevMgr->PumpFloor2.ProcessUnit(ACT_ON);
		res = true;
	}
	else if (!IsAlert) {
		publishStepAlert("Temperature of the tank in is out of range");
		IsAlert = true;
	}
	if (res) {
		step = STEP_HEATER_START;
		IsAlert = false;
		publishStepAlert("");
	}
	return res;
}

void ScriptRunner::publishStepAlert(const char* msg) {
	publishStep();
//	sprintf(Config.MqttClient->TopicBuff, MQTT_ALERT_STEP, Config.BoardId());
//	Config.MqttClient->Publish(Config.MqttClient->TopicBuff, msg);
}

bool ScriptRunner::checkElectricityFail()
{
	bool res = !Config.DevMgr->VoltageSwitch.IsOk();
	static bool fail = false;
	if (res) {
		if (!fail) {
			fail = true;
			Config.DevMgr->Compressor.ProcessUnit(ACT_OFF);
			lastStart = millis();
			step = STEP_HEATER_PAUSE6;
			if (!IsAlert) {
				IsAlert = true;
				publishStepAlert("Electricity failed");
			}
		}
	}
	else {
		if (fail) {
			IsAlert = false;
			publishStepAlert("");
			fail = false;
		}
	}
	return res;
}

bool ScriptRunner::checkPressureFail()
{
	static bool fail = false;
	bool res = !Config.DevMgr->PressureSwitch.IsOk();
	if (res) {
		if (!fail) {
			fail = true;
			Config.DevMgr->Compressor.ProcessUnit(ACT_OFF);
			lastStart = millis();
			step = STEP_HEATER_PAUSE6;
			if (!IsAlert) {
				IsAlert = true;
				publishStepAlert("Pressure failed");
			}
		}
	}
	else {
		if (fail) {
			IsAlert = false;
			publishStepAlert("");
			fail = false;
		}
	}
	return res;
}

bool ScriptRunner::checkInternalTempFail()
{
	static bool fail = false;
	bool res = !Config.DevMgr->TCompressor.IsOk()
		|| !Config.DevMgr->TVapOut.IsOk()
		|| !Config.DevMgr->TCondIn.IsOk()
		|| !Config.DevMgr->TCondVap.IsOk();
	if (res) {
		if (!fail) {
			fail = true;
			Config.DevMgr->Compressor.ProcessUnit(ACT_OFF);
			lastStart = millis();

			step = STEP_HEATER_PAUSE6;
			if (!IsAlert) {
				IsAlert = true;
				publishStepAlert("Internal Temperature failed");
			}
		}
	}
	else {
		if (fail) {
			IsAlert = false;
			publishStepAlert("");
			fail = false;
		}
	}

	return res;
}

bool ScriptRunner::checkGroundTempFail()
{
	static bool fail = false;
	bool res = !Config.DevMgr->TGndIn.IsOk()
		|| !Config.DevMgr->TGndOut.IsOk();
	if (res) {
		if (!fail) {
			fail = true;
			Config.DevMgr->Compressor.ProcessUnit(ACT_OFF);
			lastStart = millis();
			Config.DevMgr->MotorGnd.ProcessUnit(ACT_OFF);
			Config.DevMgr->PumpTankIn.ProcessUnit(ACT_OFF);

			step = STEP_HEATER_INITIAL;
			if (!IsAlert) {
				IsAlert = true;
				publishStepAlert("Ground temperature failed");
			}
		}
	}
	else {
		if (fail) {
			IsAlert = false;
			publishStepAlert("");
			fail = false;
		}
	}

	return res;
}

bool ScriptRunner::checkInsideTempFail()
{
	static bool fail = false;
	bool res = !Config.DevMgr->TOut.IsOk()
		|| !Config.DevMgr->TIn.IsOk()
		|| !Config.DevMgr->TTankIn.IsOk()
		|| !Config.DevMgr->TTankOut.IsOk()
		|| !Config.DevMgr->TInside.IsOk();
	if (res) {
		if (!fail) {
			fail = true;
			Config.DevMgr->Compressor.ProcessUnit(ACT_OFF);
			lastStart = millis();
			Config.DevMgr->PumpTankIn.ProcessUnit(ACT_OFF);
			Config.DevMgr->MotorGnd.ProcessUnit(ACT_OFF);
			Config.DevMgr->PumpTankOut.ProcessUnit(ACT_OFF);
			Config.DevMgr->PumpFloor1.ProcessUnit(ACT_OFF);
			Config.DevMgr->PumpFloor2.ProcessUnit(ACT_OFF);

			step = STEP_HEATER_INITIAL;
			if (!IsAlert) {
				IsAlert = true;
				publishStepAlert("Inside Temperature failed");
			}
		}
	}
	else {
		if (fail) {
			IsAlert = false;
			publishStepAlert("");
			fail = false;
		}
	}

	return res;
}


bool ScriptRunner::heaterStepStart()
{
	bool res = false;

	//double temp = Config.DevMgr->TInside.GetTemperature();
	//if (temp <= Config.GetDesiredTemperature() - Config.GetHysteresis()) {
	//if (Config.ScheduleMgr->IsTempActionRequired()) {
	//	res = true;
	//}
	if (res) {
		step = STEP_HEATER_GND_START;
		IsAlert = false;
		publishStepAlert("");
	} else if (!IsAlert) {
		publishStepAlert("It is too cold. Heater is not running");
		IsAlert = true;
	}
	return res;
}

bool ScriptRunner::heaterStepGndStart() {
	bool res = false;

	if (!checkElectricityFail()
		&& !checkGroundTempFail()) {
		Config.DevMgr->MotorGnd.ProcessUnit(ACT_ON);
		Config.DevMgr->PumpTankIn.ProcessUnit(ACT_ON);
		res = true;
	}
	if (res) {
		step = STEP_HEATER_PAUSE2;
		IsAlert = false;
	} else if (!IsAlert) {
		IsAlert = true;
		publishStepAlert((const char*)(F("Temperature of the ground is out of the range")));
	}
	return res;
}

bool ScriptRunner::heaterStepInitial() {
	bool res = false;
	static unsigned long startPause = 0;
	unsigned long now = millis();

	if (startPause == 0) {
		startPause = now;
	}
	if (now - startPause >= PAUSE_STEP0) {
		startPause = 0;
		res = true;
	}
	if (res) {
		step = STEP_HEATER_START_PUMP;
		IsAlert = false;
		publishStepAlert("");
	}
	else if (!IsAlert) {
		IsAlert = true;
		publishStepAlert("Waiting for few minutes before start");
	}
	
	return res;
}


bool ScriptRunner::heaterStepPause2() {
	bool res = false;
	static unsigned long startPause=0;
	unsigned long now = millis();
	
	if (startPause == 0) {
		startPause = now;
	}

	if (now-startPause >= PAUSE_STEP2) {
		startPause = 0;
		res = true;
	}
	if (res) {
		step = STEP_HEATER_MIN_TIME;
		IsAlert = false;
		publishStepAlert("");
	}
	else if (!IsAlert) {
		IsAlert = true;
		publishStepAlert("Waiting for few minutes before start heating");
	}

	return res;
}


bool ScriptRunner::heaterStepMinTime() {
	bool res = true;
	unsigned long now = millis();


	if (now - lastStart >= MIN_INTERVAL_COMPRESSOR) {
		res = true;
		publishStep();
	}
	if (res) {
		step = STEP_HEATER_MAIN_START;
		IsAlert = false;
		publishStepAlert("");
	}
	else if (!IsAlert) {
			IsAlert = true;
			publishStepAlert("Waiting for few minutes for start compressor");
	}

	return res;
}

bool ScriptRunner::heaterStepMainStart() {
	bool res = true;

	if (!checkElectricityFail()
		&& !checkElectricityFail()
		&& !checkInternalTempFail()
		&& !checkPressureFail()) {
		Config.DevMgr->Compressor.ProcessUnit(ACT_ON);
		res = true;
	}
	if (res) {
		step = STEP_HEATER_MAIN_STOP;
		IsAlert = false;
		publishStepAlert("");
	}
	else if (!IsAlert) {
		IsAlert = true;
		publishStepAlert("Can't start Compressor");
	}
	
	return res;
}

bool ScriptRunner::heaterStepMainStop() {
	bool res = false;
	//double temp = Config.DevMgr->TInside.GetTemperature();kjjkjjkj
	//if (temp >= Config.GetDesiredTemperature() + Config.GetHysteresis()) {
//	if (!Config.ScheduleMgr->IsTempActionRequired()) {
		Config.DevMgr->Compressor.ProcessUnit(ACT_OFF);
		lastStart = millis();
		res = true;
//	}
	if (res) {
		step = STEP_HEATER_PAUSE6;
		IsAlert = false;
		publishStepAlert("");
	}
	else if (!IsAlert) {
		IsAlert = true;
		publishStepAlert("Heating");
	}
	return res;
}

bool ScriptRunner::heaterStepPause6() {
	bool res = false;
	static unsigned long startPause = 0;
	unsigned long now = millis();

	if (startPause == 0) {
		startPause = now;
	}

	if (now - startPause >= PAUSE_STEP6) {
		startPause = 0;
		res = true;
	}
	if (res) {
		step = STEP_HEATER_GND_STOP;
		IsAlert = false;
		publishStepAlert("");
	} else if(!IsAlert) {
		IsAlert = true;
		publishStepAlert("Waiting for few minutes before stop the system");
	}

	return res;
}

bool ScriptRunner::heaterStepGndStop() {
	bool res = true;

	Config.DevMgr->MotorGnd.ProcessUnit(ACT_OFF);
	Config.DevMgr->PumpTankIn.ProcessUnit(ACT_OFF);
	step = STEP_HEATER_START;
	return res;
}
void ScriptRunner::publishStep()
{
//	sprintf(Config.MqttClient->PayloadBuff, "%c", step);
//	sprintf(Config.MqttClient->TopicBuff, MQTT_ALERT_STEP, Config.BoardId());
//	Config.MqttClient->Publish(Config.MqttClient->TopicBuff, Config.MqttClient->PayloadBuff);
}

