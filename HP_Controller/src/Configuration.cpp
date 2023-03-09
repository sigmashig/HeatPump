#include "Configuration.h"

#include <ArduinoJson.h>

#include "SigmaEEPROM.h"
#include "ScriptRunner.h"
#include "MemoryExplorer.h"


void Configuration::Init() {
	Log = new Loger(512);


	readBoardId();
	//initMqttTopics();
	ethClient = new EthernetClient();

	initializeEthernet();
	readConfigEEPROM();
	Log->Debug("Clock");
	Clock = new SigmaClock(ethClient);
	Clock->SetClock();
	Log->append("Now:").Info(Clock->PrintClock());

	Log->Debug("DevMgr");
	DevMgr = new DeviceManager();
	DevMgr->Init();

	Log->Debug("Schedule Mgr");
	ScheduleMgr = new ScheduleManager();
	ScheduleMgr->Init();

	char topicRoot[CONFIG_LENGTH_OF_ROOT+1];
	sprintf(topicRoot, "%s%s/", mQTT_ROOT, boardName);
	lengthOfRoot = strlen(topicRoot);

	Log->Debug("Mqtt");
	mqttClient = new Mqtt(mqttIp, mqttPort, *ethClient, topicRoot);
	mqttClient->Init();
	if (mqttClient->IsMqtt()) {
		isMqttReady = true;
		publishConfigParameter(PARAMS_IS_READY, "0");
		SubscribeAll();
	}

	mqttClient->FinalInit();
	//ScheduleMgr->FinalInit();
	DevMgr->FinalInit();
	Runner.Init();

	if (mqttClient->IsMqtt()) {
		publishConfigParameter(PARAMS_IS_READY, "1");
	}
	Log->Info(F("Config init is finished"));
}


void Configuration::setBoardId(byte id, bool save) {
	if (save) {
		if (id != boardId) {
			Log->Debug("EEROM write board id");
			SigmaEEPROM::Write8(EEPROM_ADDR_ID, id);
		}
	}
	
	boardId = id;
	mac[5] = id;
	sprintf(boardName, "Board_%02u", id);
}

/*
void Configuration::setIp(byte ip0, byte ip1, byte ip2, byte ip3) {
	ip[0] = ip0;
	ip[1] = ip1;
	ip[2] = ip2;
	ip[3] = ip3;
}
*/
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
	setBoardId(id, false);
	IPAddress ip0;
	SigmaEEPROM::ReadIp(ip0, EEPROM_ADDR_IP);
	setIp(ip0, false);
	SigmaEEPROM::ReadIp(ip0, EEPROM_ADDR_MQTT_IP);
	setMqttIp(ip0, false);
	unsigned long port;
	port = SigmaEEPROM::Read16(EEPROM_ADDR_MQTT_PORT);
	setMqttPort(port, false);
	
	Log->append(F("IP Address is: ")).append(ip[0]).append(".").append(ip[1]).append(".")
		.append(ip[2]).append(".").append(ip[3]).Info();
	Log->append(F("Mqtt Address is: ")).append(mqttIp[0]).append(".").append(mqttIp[1]).append(".")
		.append(mqttIp[2]).append(".").append(mqttIp[3]).append(":").append((uint16_t)mqttPort).Info();

}

void Configuration::readConfigEEPROM() {

	setWorkMode(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_MODE), false);
	setManualTemp(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_MANUALTEMP), false);
	setWeekMode(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_WEEKMODE), false);
	setHysteresis(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_HYSTERESIS), false);
	setHeatMode(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_HEATCOLD), false);
	setCommand(SigmaEEPROM::Read8(EEPROM_ADDR_CONFIG_CMD), false);

	char tmp[TIMEZONE_LEN];
	setTimeZone(SigmaEEPROM::ReadTimezone(tmp));

}

void Configuration::setTimeZone(const char* tz, bool save) {
	if (!save) {
		strcpy(timezone, tz);
	}

	if (0 != strcmp(timezone, tz)) {
		strcpy(timezone, tz);

		Log->Debug("EEPROM TZ");
		SigmaEEPROM::WriteTimezone(timezone);
	}
}

void Configuration::setIp(IPAddress& ipNew, bool save) {
	if (!save) {
		this->ip = ipNew;
	} else {
		if (this->ip != ipNew && Utils::IsIpValid(ipNew)) {
			this->ip = ipNew;
			Log->Debug("EEPROM IP");
			SigmaEEPROM::WriteIp(ipNew, EEPROM_ADDR_IP);
		}
	}
}

void Configuration::setMqttIp(IPAddress& ip, bool save) {
	if (!save) {
		this->mqttIp = ip;
	} else {
		if (this->mqttIp != ip && Utils::IsIpValid(ip)) {
			this->mqttIp = ip;
			Log->Debug("EEPROM Mqtt IP");
			SigmaEEPROM::WriteIp(ip, EEPROM_ADDR_MQTT_IP);
		}
	}
}

void Configuration::setMqttPort(uint16_t port, bool save) {
	if (!save) {
		mqttPort = port;
	} else {
		if (mqttPort != port) {
			mqttPort = port;
			Log->Debug("EEPROM Mqtt Port");
			SigmaEEPROM::Write16(EEPROM_ADDR_MQTT_PORT, port);
		}
	}
}

void Configuration::setClockType(byte b, bool save) {
	
	SigmaClock::CalendarServerType type = (SigmaClock::CalendarServerType)b;
	
	if (!save) {
		Clock->SetServerType(type);
	} else {
		if (Clock->GetServerType() != type) {
			Clock->SetServerType(type);
			Log->Debug("EEPROM ClockType");
			SigmaEEPROM::Write8(EEPROM_ADDR_CONFIG_CALENDARSERVICETYPE, type);
		}
	}
}

void Configuration::setWorkMode(byte b, bool save) {
	if (b == 0 || b == 1 || b == 2) {
		if (!save) {
			workMode = (WORKMODE)(b);
		}
		if (workMode != (WORKMODE)(b)) {
			workMode = (WORKMODE)(b);
			Log->Debug("EEPROM WorkMode");
			SigmaEEPROM::Write8(EEPROM_ADDR_CONFIG_MODE, workMode);
		}
	}
}

void Configuration::setCommand(byte b, bool save) {
	if (b == 0 || b == 1 || b == 2) {
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

void Configuration::setManualTemp(double t, bool save) {
	if (t >= 15.0 && t <= 50) {
		if (!save) {
			manualTemp = t;
		}
		if (manualTemp != t) {
			manualTemp = t;
			Log->Debug("EEPROM Manual Temp");
			SigmaEEPROM::Write8(EEPROM_ADDR_CONFIG_MANUALTEMP, (byte)(manualTemp * 2));
		}
	}
}

/*
void Configuration::setManualTemp(byte b, bool save) {
	double t = (double)b / 2.0;
	if (t >= 15.0 && t <= 50) {
		if (!save) {
			manualTemp = t;
		}
		if (manualTemp != t) {
			manualTemp = t;
			Log->Debug("EEPROM Manual Temp");
			SigmaEEPROM::Write8(EEPROM_ADDR_CONFIG_MANUALTEMP, (byte)(manualTemp * 2));
		}
	}
}
*/

void Configuration::setWeekMode(byte b, bool save) {
	if (b == 0 || b == 1 || b == 2) {
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
	if (b == 0 || b == 1 || b == 2) {
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

void Configuration::setHeatMode(byte b, bool save) {
	if (b == 0 || b == 1 || b == 2) {
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
		//memoryReport("Configuration::Loop 60000");
		unitsLoop(timePeriod);
	} else if (timePeriod == 30000) {
		unitsLoop(timePeriod);
	} else if (timePeriod == 10000) {
		unitsLoop(timePeriod);
	} else if (timePeriod == 1000) {
		unitsLoop(timePeriod);
	}
}
/*
void Configuration::setManualTemp(const char* str) {
	double t = Utils::Str2Double(str);
	setManualTemp((byte)(t * 2.0));
}
*/

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
	if (isMqttReady) {
		mqttClient->SubscribeWatchDogPublication();
	}
}

void Configuration::subscribeParameters() {

	for (int i = 0; i < CONFIG_PARAMS_LAST; i++) {
		if (i != PARAMS_IS_READY) {
			subscribeConfigParameter((MqttConfigParam)i);
		}
	}
	Transfer(CONFIG_PARAMS_LAST);
}

void Configuration::publishParameters() {
	publishConfigParameter(PARAMS_TIMEZONE, timezone);
	publishConfigParameter(PARAMS_WORKMODE, (byte)workMode);
	publishConfigParameter(PARAMS_MANUAL_TEMP, (byte)manualTemp);
	publishConfigParameter(PARAMS_WEEKMODE, (byte)weekMode);
	publishConfigParameter(PARAMS_HYSTERESIS, hysteresis);
	publishConfigParameter(PARAMS_HEAT_COLD, (byte)heatMode);
	publishConfigParameter(PARAMS_COMMAND, (byte)command);
	publishConfigParameter(PARAMS_DESIRED_TEMP, desiredTemp);
}

void Configuration::PublishInfo(const char* txt) {
	createSafeString(topic, MQTT_TOPIC_LENGTH);
	topic = mqttSectionName[SECTION_STATUS];
	topic += "Info/";
	publish(topic.c_str(), txt);
}

void Configuration::publishAlert(ALERTCODE code, ScriptRunner::STEPS step, const char* name) {
	createSafeString(payload, MQTT_PAYLOAD_LENGTH);
	if (name != NULL) {
		payload = "Device: ";
		payload += name;
		payload += ". ";
	}
	if (step != ScriptRunner::STEP_EMPTY) {
		payload = "Step: ";
		payload += step;
		payload += ". ";
	}
	switch (code) {
	case ALERT_EMPTY:
		payload = "";
		break;
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
	topicAlert = mqttSectionName[SECTION_ALERT];
	if (name != NULL) {
		topicAlert += name;
		topicAlert += '/';
	} else {
		//topicAlert += mqttAlertParamName[ALERT_SCRIPT];
	}
	{
		createSafeString(topic, MQTT_TOPIC_LENGTH);
		topic = topicAlert;
		topic += mqttAlertParamName[ALERT_CODE];
		char p[2];
		p[0] = code;
		p[1] = 0;
		publish(topic.c_str(), p);
	}
	{
		createSafeString(topic, MQTT_TOPIC_LENGTH);
		topic = topicAlert;
		topic += mqttAlertParamName[ALERT_TEXT];
		publish(topic.c_str(), payload.c_str());
	}
}

void Configuration::publish(const char* topic, const char* payload) {
	if (isMqttReady) {
		mqttClient->Publish(topic, payload);
	}
}

void Configuration::Subscribe(const char* topic) {
	if (isMqttReady) {
		mqttClient->Subscribe(topic);
	}
}

void Configuration::ProcessMessage(const char* topic, const char* payload) {
	char const * topic0;
	topic0 = mqttSectionName[SECTION_CONFIG];
	if (strncmp(topic, topic0, strlen(topic0)) == 0) { //Config
		updateConfig(topic + strlen(topic0), payload);
	} else {
		topic0 = mqttSectionName[SECTION_STATUS];
		if (strncmp(topic, topic0, strlen(topic0)) == 0) { //Status
			for (int i = 0; i < DEVICE_TYPE_LAST; i++) {
				if (strncmp(topic + strlen(topic0), mqttDeviceTypeName[i], strlen(mqttDeviceTypeName[i])) == 0) {
					DevMgr->UpdateStatus((DeviceType)i, topic + strlen(topic0) + strlen(mqttDeviceTypeName[i]), payload);
					break;
				}
			}
		} else {
			topic0 = mqttSectionName[SECTION_SCHEDULE_WEEKEND];
			if (strncmp(topic, topic0, strlen(topic0)) == 0) { //Schedule weekend
				byte setNumber = atoi(topic + strlen(topic0));
				ScheduleMgr->UpdateSchedule(0, setNumber, payload);
			} else {
				topic0 = mqttSectionName[SECTION_SCHEDULE_WORKDAYS];
				if (strncmp(topic, topic0, strlen(topic0)) == 0) { //Schedule workday
					byte setNumber = atoi(topic + strlen(topic0));
					ScheduleMgr->UpdateSchedule(CONFIG_NUMBER_SCHEDULES, setNumber, payload);
				} else {
					topic0 = mqttSectionName[SECTION_EQUIPMENT];
					if (strncmp(topic, topic0, strlen(topic0)) == 0) { //Equipment
						for (int i = 0; i < DEVICE_TYPE_LAST; i++) {
							if (strncmp(topic + strlen(topic0), mqttDeviceTypeName[i], strlen(mqttDeviceTypeName[i])) == 0) {
								DevMgr->UpdateEquipment((DeviceType)i, topic + strlen(topic0) + strlen(mqttDeviceTypeName[i]), payload);
								break;
							}
						}
					}
				}
			}
		}
	}
}

void Configuration::updateSingleParam(MqttConfigParam parm, const char* payload) {
	switch (parm) {
	case PARAMS_WORKMODE: {
		byte b = atoi(payload);
		setWorkMode(b);
		break;
	}
	case PARAMS_WEEKMODE: {
		byte b = atoi(payload);
		setWeekMode(b);
		break;
	}
	case PARAMS_MANUAL_TEMP: {
		double f = atof(payload);
		setManualTemp(f);
		break;	
	}
	case PARAMS_SIMULATOR: {
		byte b = atoi(payload);
		setSimulator(b);
		break;
	}
	case PARAMS_HEAT_COLD: {
		byte b = atoi(payload);
		setHeatMode(b);
		break;
	}
	case PARAMS_HYSTERESIS: {
		double f = atof(payload);
		setHysteresis(f);
		break;
	}
	case PARAMS_COMMAND: {
		byte b = atoi(payload);
		setCommand(b);
		break;
	}
	case PARAMS_TIMEZONE:
		setTimeZone(payload);
		break;
	case PARAMS_IP: {
		IPAddress ip;
		ip.fromString(payload);
		setIp(ip);
		break;
	}
	case PARAMS_BOARD_ID: {
		byte b = atoi(payload);
		setBoardId(b);
		break;
	}
	case PARAMS_MQTT_IP: {
		IPAddress ip;
		ip.fromString(payload);
		setMqttIp(ip);
		break;
	}
	case PARAMS_MQTT_PORT: {
		int i = atoi(payload);
		setMqttPort(i);
		break;
	}
	
	case PARAMS_CLOCK_TYPE: {
		byte b = atoi(payload);
		setClockType(b);
		break;
	}
	case CONFIG_PARAMS_LAST:
	case PARAMS_DESIRED_TEMP:
	case PARAMS_WATCHDOG:
	case PARAMS_IS_READY:
		break;
	}
}

/// @brief Update configuration parameters received from MQTT
/// @param topic - topic of MQTT message
/// @param payload - payload of MQTT message
void Configuration::updateConfig(const char* topic, const char* payload) {

	for (int i = 0; i < CONFIG_PARAMS_LAST; i++) {
		if (strcmp(topic, mqttConfigParamName[i]) == 0) {
			updateSingleParam((MqttConfigParam)i, payload);
			return;
		}
	}
	/*
	createSafeString(topic0, MQTT_TOPIC_LENGTH);
	topic0 = mqttConfigParamName[PARAMS_WORKMODE];
	if (strcmp(topic, topic0.c_str()) == 0) {
		setWorkMode(payload);
	} else {
		topic0 = mqttConfigParamName[PARAMS_WEEKMODE];
		if (strcmp(topic, topic0.c_str()) == 0) {
			setWeekMode(payload);
		} else {
			topic0 = mqttConfigParamName[PARAMS_MANUAL_TEMP];
			if (strcmp(topic, topic0.c_str()) == 0) {
				setManualTemp(payload);
			} else {
				topic0 = mqttConfigParamName[PARAMS_HEAT_COLD];
				if (strcmp(topic, topic0.c_str()) == 0) {
					setHeatMode(payload);
				} else {
					topic0 = mqttConfigParamName[PARAMS_HYSTERESIS];
					if (strcmp(topic, topic0.c_str()) == 0) {
						setHysteresis(payload);
					} else {
						topic0 = mqttConfigParamName[PARAMS_TIMEZONE];
						if (strcmp(topic, topic0.c_str()) == 0) {
							Clock->SetTimezone(payload);
						} else {
							topic0 = mqttConfigParamName[PARAMS_SIMULATOR];
							if (strcmp(topic, topic0.c_str()) == 0) {
								setSimulator(payload);
							} else {
								topic0 = mqttConfigParamName[PARAMS_COMMAND];
								if (strcmp(topic, topic0.c_str()) == 0) {
									setCommand(payload);
								}
							}
						}
					}
				}
			}
		}
	}
*/
}

void Configuration::publishConfigParameter(MqttConfigParam parmId, const char* payload) {
	createSafeString(topic, MQTT_TOPIC_LENGTH);
	topic = mqttSectionName[SECTION_CONFIG];
	topic += mqttConfigParamName[parmId];
	publish(topic.c_str(), payload);
}

void Configuration::subscribeConfigParameter(MqttConfigParam parmId) {
	createSafeString(topic, MQTT_TOPIC_LENGTH);
	topic = mqttSectionName[SECTION_CONFIG];
	topic += mqttConfigParamName[parmId];
	Subscribe(topic.c_str());
}

void Configuration::publishConfigParameter(MqttConfigParam parmId, byte payload) {
	char p[4];
	sprintf(p, "%u", payload);
	publishConfigParameter(parmId, p);
}

void Configuration::publishConfigParameter(MqttConfigParam parmId, double payload) {
	createSafeString(p, MQTT_TOPIC_LENGTH);
	p = payload;
	publishConfigParameter(parmId, p.c_str());
}

void Configuration::Publish(DeviceType dType, const char* name, byte status) {
	char p[4];
	sprintf(p, "%u", status);
	publishStatus(dType, name, p);
}


void Configuration::Publish(DeviceType dType, const char* name, const char* payload) {
	publishStatus(dType, name, payload);
}


void Configuration::PublishStep(ScriptRunner::STEPS step) {
	char p[2];
	sprintf(p, "%c", step);
	publishStatus(DEVTYPE_SCRIPT, "Step", p);
}

void Configuration::Publish(DeviceType dType, const char* name, double status) {
	char p[10];
	Utils::Double2Str(p, status, 1);
	publishStatus(dType, name, p);
}

void Configuration::publishStatus(DeviceType dType, const char* name, const char* payload) {
	if (isMqttReady) {
		createSafeString(topic, MQTT_TOPIC_LENGTH);
		topic = mqttSectionName[SECTION_STATUS];
		topic += mqttDeviceTypeName[dType];
		topic += name;
		mqttClient->Publish(topic.c_str(), payload);
	}
}

void Configuration::SubscribeEquipment(DeviceType dType, const char* name) {
	createSafeString(topic, MQTT_TOPIC_LENGTH);
	topic = mqttSectionName[SECTION_EQUIPMENT];
	topic += mqttDeviceTypeName[dType];
	topic += name;
	mqttClient->Subscribe(topic.c_str());
}

void Configuration::SubscribeStatus(DeviceType dType, const char* name) {
	createSafeString(topic, MQTT_TOPIC_LENGTH);
	topic = mqttSectionName[SECTION_STATUS];
	topic += mqttDeviceTypeName[dType];
	topic += name;
	mqttClient->Subscribe(topic.c_str());
}

void Configuration::SubscribeSchedule(int number) {
	createSafeString(topic, MQTT_TOPIC_LENGTH);
	if (number <= CONFIG_NUMBER_SCHEDULES) {
		topic = mqttSectionName[SECTION_SCHEDULE_WEEKEND];
	} else {
		topic = mqttSectionName[SECTION_SCHEDULE_WORKDAYS];
		number -= CONFIG_NUMBER_SCHEDULES;
	}
	//topic = (number < CONFIG_NUMBER_SCHEDULES ? mqttSectionName[SECTION_SCHEDULE_WEEKEND] : mqttSectionName[SECTION_SCHEDULE_WORKDAYS]);
	topic += number;
	mqttClient->Subscribe(topic.c_str());
}

void Configuration::PublishSchedule(int number) {
	Log->append("Schedule:").append(number).Debug();
	createSafeString(topic, MQTT_TOPIC_LENGTH);
	topic = (number < CONFIG_NUMBER_SCHEDULES ? mqttSectionName[SECTION_SCHEDULE_WEEKEND] : mqttSectionName[SECTION_SCHEDULE_WORKDAYS]);
	topic += number;
	char payload[MQTT_PAYLOAD_LENGTH + 1];
	ScheduleMgr->GetSchedule(number).Serialize(payload);
	Log->append("Schedule:").append(number).append("=").append(payload).Debug();
	//mqttClient->Publish(topic.c_str(), payload);
}

void Configuration::WatchDogPublication() {
	if (lastWatchDogPublication + WATCHDOG_PUBLICATION_INTERVAL > millis()) {
		lastWatchDogPublication = millis();
	} else {
		Log->Error("WatchDogPublication: a long time ago...");
		SubscribeAll();
	}
	lastWatchDogPublication = millis();
}
