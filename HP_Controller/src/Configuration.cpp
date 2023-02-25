#include "Configuration.h"

#include <ArduinoJson.h>

#include "SigmaEEPROM.h"
#include "ScriptRunner.h"


void Configuration::Init() {
	Log = new Loger(512);


	readBoardId();
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
		SubscribeAll();
	}

	mqttClient->FinalInit();
	ScheduleMgr->FinalInit();
	DevMgr->FinalInit();

	Runner.Init();

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

	setMode(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_MODE));
	setManualTemp(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_MANUALTEMP));
	setWeekMode(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_WEEKMODE));
	setHysteresis(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_HYSTERESIS));
	setHeatCold(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_HEATCOLD));
	setCmd(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_CMD));
	//setSimulator(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_SIMULATOR));

	char tmp[TIMEZONE_LEN];
	setTimeZone(SigmaEEPROM::ReadTimezone(tmp));

}

void Configuration::setTimeZone(const char* tz) {
	//Log->append("SetTZ=").append(tz).Debug();
	if (0 != strcmp(timezone, tz)) {
		strcpy(timezone, tz);
		Log->Debug("EEPROM TZ");
		SigmaEEPROM::WriteTimezone(timezone);
	}
}


void Configuration::setMode(byte b) {
	if (b == 0 || b == 1) {
		if (mode != (MODE)(b)) {
			mode = (MODE)(b);
			Log->Debug("EEPROM Mode");
			SigmaEEPROM::Write8(EEPROM_ADDR_CONFIG_MODE, mode);
		}
	}
}

void Configuration::setCmd(byte b) {
	if (b == 0 || b == 1) {
		if (command != (CMD)(b)) {
			command = (CMD)(b);
			Log->Debug("EEPROM Cmd");

			SigmaEEPROM::Write8(EEPROM_ADDR_CONFIG_MODE, command);
		}
	}
}

void Configuration::setSimulator(byte b) {
	if (b == 0 || b == 1) {
		isSimulator = b;
	}
}


void Configuration::setManualTemp(byte b) {
	double t = (double)b / 2.0;
	Log->append("Manual Temp:").append("system:").append(manualTemp).append("; t=").append(t).append("; b=").append(b).Debug();
	if (t >= 15.0 && t <= 50) {
		if (manualTemp != t) {
			manualTemp = t;
			Log->Debug("EEPROM Manual Temp");

			SigmaEEPROM::Write8(EEPROM_ADDR_CONFIG_MANUALTEMP, (byte)(manualTemp * 2));
			byte b1 = SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_MANUALTEMP);
			Log->append("ReadTemp:").append(b1).Debug();
		}
	}
}

void Configuration::setWeekMode(byte b) {
	if (b == 0 || b == 1) {
		if (weekMode != (WEEKMODE)(b)) {
			weekMode = (WEEKMODE)(b);
			Log->Debug("EEPROM WeekMode");

			SigmaEEPROM::Write8(EEPROM_ADDR_CONFIG_WEEKMODE, weekMode);
		}
	}
}

void Configuration::setHysteresis(byte b) {
	if (b == 0 || b == 1) {
		if (hysteresis != b) {
			hysteresis = b;
			Log->Debug("EEPROM Hysteresis");

			SigmaEEPROM::Write8(EEPROM_ADDR_CONFIG_HYSTERESIS, hysteresis);
		}
	}
}

void Configuration::setHeatCold(byte b) {
	if (b == 0 || b == 1) {
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
		//Runner.HeatScript(0);
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


void Configuration::SubscribeAll() {
	subscribeParameters();
	DevMgr->SubscribeEquipment();
	DevMgr->SubscribeStatuses();
	ScheduleMgr->SubscribeSchedules();
}

void Configuration::subscribeParameters() {

	int n = 0;
	sprintf(TopicBuff, MQTT_WATCH_DOG2, boardId);
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_MODE, boardId);
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_WEEKMODE, boardId);
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_MANUAL_TEMP, boardId);
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_HEAT_COLD, boardId);
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_HYSTERESIS, boardId);
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_TIMEZONE, boardId);
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_SIMULATOR, boardId);
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_COMMAND, boardId);
	Subscribe(TopicBuff);
	n++;
	Transfer(n);
}

void Configuration::publishTimezone() {
	sprintf(TopicBuff, MQTT_TIMEZONE, boardId);
	Publish(TopicBuff, timezone);
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

void Configuration::PublishAlert(ALERTCODE code, ScriptRunner::STEPS step, const char* name ) {

	char tmp[20];
	PayloadBuff[0] = 0;
	if (name != NULL) {
		sprintf(PayloadBuff, "Device: %s. ", name);
	}
	
	if (step != ScriptRunner::STEP_EMPTY) {
		sprintf(PayloadBuff, "Step: %c. ", step);
	}
	

	switch (code) {
	case ALERT_EMPTY:
		PayloadBuff[0] = 0;
		break;
	case ALERT_TEMP_FLOOR:
		strcat(PayloadBuff, "Temperature of floor is too low");
		break;
	case ALERT_STEP_TOO_LONG:
		strcat(PayloadBuff, "Step is too long");
		break;
	case ALERT_TEMP_IS_OUT_OF_RANGE:
		strcat(PayloadBuff, "Temperature is out of range");
		break;
	}

	sprintf(TopicBuff, MQTT_ALERT_MSG, boardId);
	Publish();

	sprintf(TopicBuff, MQTT_ALERT_CODE, boardId);
	sprintf(PayloadBuff, "%c", code);
	Publish();
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
	sprintf(TopicBuff, MQTT_MODE, boardId);
	PayloadBuff[0] = (byte)mode + '0';
	PayloadBuff[1] = 0;
	Publish();
}

void Configuration::publishWeekMode() {
	sprintf(TopicBuff, MQTT_WEEKMODE, boardId);
	PayloadBuff[0] = (byte)weekMode + '0';
	PayloadBuff[1] = 0;
	Publish();
}

void Configuration::publishManualTemp() {
	sprintf(TopicBuff, MQTT_MANUAL_TEMP, boardId);

	int mt1 = (int)manualTemp;
	int mt2 = (manualTemp - mt1) * 10;
	sprintf(PayloadBuff, "%u.%1u", mt1, mt2);
	Publish();
}

void Configuration::publishHeatCold() {
	sprintf(TopicBuff, MQTT_HEAT_COLD, boardId);
	PayloadBuff[0] = (byte)heatMode + '0';
	PayloadBuff[1] = 0;
	Publish();
}

void Configuration::publishHysteresis() {
	sprintf(TopicBuff, MQTT_HYSTERESIS, boardId);
	PayloadBuff[0] = hysteresis + '0';
	PayloadBuff[1] = 0;
	Publish();
}


void Configuration::publishSimulator() {
	sprintf(TopicBuff, MQTT_SIMULATOR, boardId);
	PayloadBuff[0] = (isSimulator ? '1' : '0');
	PayloadBuff[1] = 0;
	Publish();
}

void Configuration::publishCmd() {
	sprintf(TopicBuff, MQTT_COMMAND, boardId);
	PayloadBuff[0] = (byte)command + '0';
	PayloadBuff[1] = 0;
	Publish();
}

void Configuration::ProcessMessage(const char* topic, const char* payload) {
	sprintf(TopicBuff, MQTT_CONFIG, BoardId());
	if (strncmp(topic, TopicBuff, strlen(TopicBuff)) == 0) { //Config
		updateConfig(topic, payload);
	} else {
		sprintf(TopicBuff, MQTT_STATUS, BoardId());
		if (strncmp(topic, TopicBuff, strlen(TopicBuff)) == 0) { //Status
			DevMgr->UpdateStatuses(topic, payload);
		} else {

			sprintf(TopicBuff, MQTT_SCHEDULE, BoardId());
			if (strncmp(topic, TopicBuff, strlen(TopicBuff)) == 0) { //Schedule
				ScheduleMgr->UpdateSchedule(topic, payload);
			} else {
				sprintf(TopicBuff, MQTT_EQUIPMENT, BoardId());
				if (strncmp(topic, TopicBuff, strlen(TopicBuff)) == 0) { //Equipment
					DevMgr->UpdateEquipment(topic, payload);
				}
			}
		}
	}
}


void Configuration::updateConfig(const char* topic, const char* payload) {

	sprintf(TopicBuff, MQTT_MODE, boardId);
	if (strcmp(topic, TopicBuff) == 0) {
		SetMode(payload);
	} else {
		sprintf(TopicBuff, MQTT_WEEKMODE, boardId);
		if (strcmp(topic, TopicBuff) == 0) {
			SetWeekMode(payload);
		} else {
			sprintf(TopicBuff, MQTT_MANUAL_TEMP, boardId);
			if (strcmp(topic, TopicBuff) == 0) {
				SetManualTemp(payload);
			} else {
				sprintf(TopicBuff, MQTT_HEAT_COLD, boardId);
				if (strcmp(topic, TopicBuff) == 0) {
					SetHeatMode(payload);
				} else {
					sprintf(TopicBuff, MQTT_HYSTERESIS, boardId);
					if (strcmp(topic, TopicBuff) == 0) {
						SetHysteresis(payload);
					} else {
						sprintf(TopicBuff, MQTT_TIMEZONE, boardId);
						if (strcmp(topic, TopicBuff) == 0) {
							Clock->SetTimezone(payload);
						} else {
							sprintf(TopicBuff, MQTT_SIMULATOR, boardId);
							if (strcmp(topic, TopicBuff) == 0) {
								SetSimulator(payload);
							} else {
								sprintf(TopicBuff, MQTT_COMMAND, boardId);
								if (strcmp(topic, TopicBuff) == 0) {
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