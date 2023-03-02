#include "Configuration.h"

#include <ArduinoJson.h>

#include "SigmaEEPROM.h"
#include "ScriptRunner.h"


void Configuration::Init() {
	Log = new Loger(512);
	

	readBoardId();
	initMqttTopics();
	ethClient = new EthernetClient();

	initializeEthernet();
	readConfigEEPROM();

	Log->Debug("Clock1");

	Clock = new SigmaClock(ethClient);
	Clock->SetClock();
	Log->append("Now:").Info(Clock->PrintClock());

	Log->Debug("DevMgr");
	DevMgr = new DeviceManager();
	DevMgr->Init();
	Log->Debug("Schedule Mgr");
	ScheduleMgr = new ScheduleManager();
	ScheduleMgr->Init();
	Log->Debug("Mqtt");

	mqttClient = new Mqtt(mqttIp, mqttPort, *ethClient);
	mqttClient->Init();

	if (mqttClient->IsMqtt()) {
		isMqttReady = true;
		publishConfigParameter(MQTT_IS_READY, "0");
		SubscribeAll();
	}

	mqttClient->FinalInit();
	ScheduleMgr->FinalInit();
	DevMgr->FinalInit();

	Runner.Init();

	if (mqttClient->IsMqtt()) {
		publishConfigParameter(MQTT_IS_READY, "1");
	}
	Log->Info(F("Config init is finished"));
}


void Configuration::setBoardId(byte id) {
	boardId = id;
	mac[5] = id;
	sprintf(boardName, "Board_%02u", id);
}

void Configuration::setIp(byte ip0, byte ip1, byte ip2, byte ip3) {
	ip[0] = ip0;
	ip[1] = ip1;
	ip[2] = ip2;
	ip[3] = ip3;
}

void Configuration::initializeEthernet() {
	Log->Info("Initializing Ethernet...");
	if (Ethernet.begin(mac) == 0) {
		Log->Error("Failed to configure Ethernet using DHCP");
		// Check for Ethernet hardware present
		if (Ethernet.hardwareStatus() == EthernetNoHardware) {
			Log->Error("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
		} else if (Ethernet.linkStatus() == LinkOFF) {
			Log->Error("Ethernet cable is not connected.");
		} else {
			Ethernet.begin(mac, ip);
		}
	}
	EthernetLinkStatus link = Ethernet.linkStatus();
	if (EthernetLinkStatus::LinkOFF != link) {
		isEthernetReady = true;
		Log->Info("Ethernet has been initialized");
		//delay(1000);
	} else {
		Log->append("Can't connect to Ethernet=").append(link).Error();
	}
}

void Configuration::readBoardId() {
	byte id = SigmaEEPROM::ReadBoardId();
	setBoardId(id);
	SigmaEEPROM::ReadIp(ip);
	mqttPort = SigmaEEPROM::ReadMqtt(mqttIp);

	Log->append(F("IP Address is: ")).append(ip[0]).append(".").append(ip[1]).append(".")
		.append(ip[2]).append(".").append(ip[3]).Info();
	Log->append(F("Mqtt Address is: ")).append(mqttIp[0]).append(".").append(mqttIp[1]).append(".")
		.append(mqttIp[2]).append(".").append(mqttIp[3]).append(":").append((uint16_t)mqttPort).Info();

}

void Configuration::readConfigEEPROM() {

	setMode(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_MODE), false);
	setManualTemp(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_MANUALTEMP), false);
	setWeekMode(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_WEEKMODE), false);
	setHysteresis(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_HYSTERESIS), false);
	setHeatCold(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_HEATCOLD), false);
	setCmd(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_CMD), false);
	//setSimulator(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_SIMULATOR));

	char tmp[TIMEZONE_LEN];
	setTimeZone(SigmaEEPROM::ReadTimezone(tmp));

}

void Configuration::setTimeZone(const char* tz, bool save) {
	//Log->append("SetTZ=").append(tz).Debug();
	if (!save) {
		strcpy(timezone, tz);
	}

	if (0 != strcmp(timezone, tz)) {
		strcpy(timezone, tz);

		Log->Debug("EEPROM TZ");
		SigmaEEPROM::WriteTimezone(timezone);
	}
}


void Configuration::setMode(byte b, bool save) {
	if (b == 0 || b == 1 || b==2) {
		if (!save) {
			mode = (WORKMODE)(b);
		}
		if (mode != (WORKMODE)(b)) {
			mode = (WORKMODE)(b);
			Log->Debug("EEPROM Mode");
			SigmaEEPROM::Write8(EEPROM_ADDR_CONFIG_MODE, mode);
		}
	}
}

void Configuration::setCmd(byte b, bool save) {
	if (b == 0 || b == 1 || b==2)  {
		if (!save) {
			command = (CMD)(b);
		}
		if (command != (CMD)(b)) {
			command = (CMD)(b);
			Log->Debug("EEPROM Cmd");

			SigmaEEPROM::Write8(EEPROM_ADDR_CONFIG_MODE, command);
		}
	}
}

void Configuration::setSimulator(byte b, bool save) {
	if (b == 0 || b == 1) {
		isSimulator = b;
	}
}


void Configuration::setManualTemp(byte b, bool save) {
	double t = (double)b / 2.0;
	//Log->append("Manual Temp:").append("system:").append(manualTemp).append("; t=").append(t).append("; b=").append(b).Debug();
	if (t >= 15.0 && t <= 50) {
		if (!save) {
			manualTemp = t;
		}
		if (manualTemp != t) {
			manualTemp = t;
			Log->Debug("EEPROM Manual Temp");

			SigmaEEPROM::Write8(EEPROM_ADDR_CONFIG_MANUALTEMP, (byte)(manualTemp * 2));
			byte b1 = SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_MANUALTEMP);
			Log->append("ReadTemp:").append(b1).Debug();
		}
	}
}

void Configuration::setWeekMode(byte b, bool save) {
	if (b == 0 || b == 1 || b==2) {
		if (!save) {
			weekMode = (WEEKMODE)(b);
		}

		if (weekMode != (WEEKMODE)(b)) {
			weekMode = (WEEKMODE)(b);
			Log->Debug("EEPROM WeekMode");

			SigmaEEPROM::Write8(EEPROM_ADDR_CONFIG_WEEKMODE, weekMode);
		}
	}
}

void Configuration::setHysteresis(byte b, bool save) {
	if (b == 0 || b == 1 || b==2) {
		if (!save) {
			hysteresis = b;
		}

		if (hysteresis != b) {
			hysteresis = b;
			Log->Debug("EEPROM Hysteresis");

			SigmaEEPROM::Write8(EEPROM_ADDR_CONFIG_HYSTERESIS, hysteresis);
		}
	}
}

void Configuration::setHeatCold(byte b, bool save) {
	if (b == 0 || b == 1 || b==2) {
		if (!save) {
			heatMode = (HEATMODE)(b);
		}

		if (heatMode != (HEATMODE)(b)) {
			heatMode = (HEATMODE)(b);
			Log->Debug("EEPROM HeatCold");

			SigmaEEPROM::Write8(EEPROM_ADDR_CONFIG_HEATCOLD, heatMode);
		}
	}
}


void Configuration::Loop(unsigned long timePeriod) {
	if (timePeriod == 0) {

		mqttClient->MqttLoop();
		unitsLoop(timePeriod);
		Runner.Loop(timePeriod);

	} else if (timePeriod == 60000) {
		unitsLoop(timePeriod);
	} else if (timePeriod == 30000) {

		unitsLoop(timePeriod);
	} else if (timePeriod == 10000) {
		unitsLoop(timePeriod);
	} else if (timePeriod == 1000) {
		unitsLoop(timePeriod);
	}
}

void Configuration::SetManualTemp(const char* str) {
	double t = Utils::Str2Double(str);
	setManualTemp((byte)(t * 2.0));
}

void Configuration::unitsLoop(unsigned long timePeriod) {
	DevMgr->UnitLoop(timePeriod);
}


double Configuration::GetDesiredTemp() {
	return manualTemp; //TODO: should be fixed when ScheduleMgr is done
}

void Configuration::SubscribeAll() {
	subscribeParameters();
	DevMgr->SubscribeEquipment();
	DevMgr->SubscribeStatuses();
	ScheduleMgr->SubscribeSchedules();
}

void Configuration::subscribeParameters() {

	int n = 0;
	subscribeConfigParameter(MQTT_WATCH_DOG2);
	n++;
	subscribeConfigParameter(MQTT_MODE);
	n++;
	subscribeConfigParameter(MQTT_WEEKMODE);
	n++;
	subscribeConfigParameter(MQTT_MANUAL_TEMP);
	n++;
	subscribeConfigParameter(MQTT_HEAT_COLD);
	n++;
	subscribeConfigParameter(MQTT_HYSTERESIS);
	n++;
	subscribeConfigParameter(MQTT_TIMEZONE);
	n++;
	subscribeConfigParameter(MQTT_SIMULATOR);
	n++;
	subscribeConfigParameter(MQTT_COMMAND);
	n++;
	Transfer(n);
}

void Configuration::publishTimezone() {
	publishConfigParameter(MQTT_TIMEZONE, timezone);
}

void Configuration::publishParameters() {

	publishTimezone();
	publishMode();
	publishManualTemp();
	publishWeekMode();
	publishHysteresis();
	publishHeatCold();
	publishCmd();
}

void Configuration::publishAlert(ALERTCODE code, ScriptRunner::STEPS step, const char* name) {
	createSafeString(payload, MQTT_PAYLOAD_LENGTH);

	if (name != NULL) {
		payload += "Device: ";
		payload += name;
	}
	if (step != ScriptRunner::STEP_EMPTY) {
		payload = "Step: ";
		payload += step;
	}

	switch (code) {
	case ALERT_EMPTY:
		payload = "";
		break;
		//case ALERT_TEMP_FLOOR:
		//	payload += "Temperature of floor is too low";
		//	break;
	case ALERT_STEP_TOO_LONG:
		payload += "Step is too long";
		break;
	case ALERT_TEMP_IS_OUT_OF_RANGE:
		payload += "Temperature is out of range";
		break;
	case ALERT_NOT_RUNNING:
		payload += "Engine is not running";
		break;
	case ALERT_OTHER:
		payload += "Some unknown error";
		break;
	case ALERT_PRESSURE_IS_OUT_OF_RANGE:
		payload += "Pressure is out of range";
		break;
	case ALERT_VOLTAGE_IS_OUT_OF_RANGE:
		payload += "Voltage is out of range";
		break;

	}

	createSafeString(topicAlert, MQTT_TOPIC_LENGTH);
	topicAlert = topicRoot;
	topicAlert += MQTT_ALERT MQTT_SEPARATOR;
	if (name != NULL) {
		topicAlert += name;
	} else {
		topicAlert += MQTT_ALERT_SCRIPT;
	}
	topicAlert += MQTT_SEPARATOR;

	{
		createSafeString(topic, MQTT_TOPIC_LENGTH);
		topic = topicAlert;
		topic += MQTT_ALERT_CODE;
		char payload[2];
		payload[0] = code;
		Publish(topic.c_str(), payload);
	}
	{
		createSafeString(topic, MQTT_TOPIC_LENGTH);
		topic = topicAlert;
		topic += MQTT_ALERT_MSG;
		Publish(topic.c_str(), payload.c_str());
	}

}

void Configuration::Publish(const char* topic, const char* payload) {
	if (isMqttReady) {
		mqttClient->Publish(topic, payload);
	}
}

void Configuration::Subscribe(const char* topic) {
	if (isMqttReady) {
		mqttClient->Subscribe(topic);
	}
}


void Configuration::publishMode() {
	publishConfigParameter(MQTT_MODE, (byte)mode);
}

void Configuration::publishWeekMode() {
	publishConfigParameter(MQTT_WEEKMODE, weekMode);
}

void Configuration::publishManualTemp() {
	createSafeString(payload, MQTT_PAYLOAD_LENGTH);

	payload = manualTemp;
	publishConfigParameter(MQTT_MANUAL_TEMP, payload.c_str());
}

void Configuration::publishDesiredTemp() {
	createSafeString(payload, MQTT_PAYLOAD_LENGTH);

	payload = desiredTemp;
	publishConfigParameter(MQTT_DESIRED_TEMP, payload.c_str());
}

void Configuration::publishHeatCold() {
	publishConfigParameter(MQTT_HEAT_COLD, (byte)heatMode);
}

void Configuration::publishHysteresis() {
	publishConfigParameter(MQTT_HYSTERESIS, (byte)hysteresis);
}

/*
void Configuration::publishSimulator() {
	sprintf(TopicBuff, MQTT_SIMULATOR, boardId);
	PayloadBuff[0] = (isSimulator ? '1' : '0');
	PayloadBuff[1] = 0;
	Publish();
}
*/

void Configuration::publishCmd() {
	publishConfigParameter(MQTT_COMMAND, (byte)command);
}

void Configuration::ProcessMessage(const char* topic, const char* payload) {
	createSafeString(topic0, MQTT_TOPIC_LENGTH);
	topic0 = topicRoot;
	topic0 += MQTT_CONFIG MQTT_SEPARATOR;

	if (strncmp(topic,  topic0.c_str(), topic0.length()) == 0) { //Config
		updateConfig(topic, payload);
	} else {
		topic0 = topicRoot;
		topic0 += MQTT_STATUS MQTT_SEPARATOR;
		if (strncmp(topic, topic0.c_str(), topic0.length()) == 0) { //Status
			DevMgr->UpdateStatuses(topic, payload);
		} else {
			topic0 = topicRoot;
			topic0 += MQTT_SCHEDULE MQTT_SEPARATOR;
			if (strncmp(topic, topic0.c_str(), topic0.length()) == 0) { //Schedule
				ScheduleMgr->UpdateSchedule(topic, payload);
			} else {
				topic0 = topicRoot;
				topic0 += MQTT_EQUIPMENT MQTT_SEPARATOR;
				if (strncmp(topic, topic0.c_str(), topic0.length()) == 0) { //Equipment
					DevMgr->UpdateEquipment(topic, payload);
				}
			}
		}
	}
}


void Configuration::updateConfig(const char* topic, const char* payload) {

	createSafeString(topicConfig, MQTT_TOPIC_LENGTH);
	topicConfig = topicRoot;
	topicConfig += MQTT_CONFIG MQTT_SEPARATOR;

	createSafeString(topic0, MQTT_TOPIC_LENGTH);
	topic0 = topicConfig;
	topic0 += MQTT_MODE;
	if (strcmp(topic, topic0.c_str()) == 0) {
		SetMode(payload);
	} else {
		topic0 = topicConfig;
		topic0 += MQTT_WEEKMODE;
		if (strcmp(topic, topic0.c_str()) == 0) {
			SetWeekMode(payload);
		} else {
			topic0 = topicConfig;
			topic0 += MQTT_MANUAL_TEMP;
			if (strcmp(topic, topic0.c_str()) == 0) {
				SetManualTemp(payload);
			} else {
				topic0 = topicConfig;
				topic0 += MQTT_HEAT_COLD;
				if (strcmp(topic, topic0.c_str()) == 0) {
					SetHeatMode(payload);
				} else {
					topic0 = topicConfig;
					topic0 += MQTT_HYSTERESIS;
					if (strcmp(topic, topic0.c_str()) == 0) {
						SetHysteresis(payload);
					} else {
						topic0 = topicConfig;
						topic0 += MQTT_TIMEZONE;
						if (strcmp(topic, topic0.c_str()) == 0) {
							Clock->SetTimezone(payload);
						} else {
							topic0 = topicConfig;
							topic0 += MQTT_SIMULATOR;
							if (strcmp(topic, topic0.c_str()) == 0) {
								SetSimulator(payload);
							} else {
								topic0 = topicConfig;
								topic0 += MQTT_COMMAND;
								if (strcmp(topic, topic0.c_str()) == 0) {
									SetCommand(payload);
								}
							}
						}
					}
				}
			}
		}
	}
}

void Configuration::initMqttTopics() {

	strcpy(topicRoot, MQTT_ROOT MQTT_SEPARATOR);
	strcat(topicRoot, boardName);
	strcat(topicRoot, MQTT_SEPARATOR);
}

void Configuration::publishConfigParameter(const char* name, const char* payload) {
	createSafeString(topicConfig, MQTT_TOPIC_LENGTH);
	topicConfig = topicRoot;
	topicConfig += MQTT_CONFIG MQTT_SEPARATOR;
	topicConfig += name;

	Publish(topicConfig.c_str(), payload);
}

void Configuration::subscribeConfigParameter(const char* name) {
	createSafeString(topicConfig, MQTT_TOPIC_LENGTH);
	topicConfig = topicRoot;
	topicConfig += MQTT_CONFIG MQTT_SEPARATOR;
	topicConfig += name;

	Subscribe(topicConfig.c_str());
}

void Configuration::publishConfigParameter(const char* name, byte payload) {
	char p[2];
	p[0] = (byte)payload + '0';
	publishConfigParameter(name, p);
}
