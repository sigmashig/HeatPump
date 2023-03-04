#pragma once

#include <Ethernet.h>

#include "Loger.h"
#include "Mqtt.h"
#include "DeviceManager.h"
#include "Schedule.h"
#include "ScriptRunner.h"
#include "ScheduleManager.h"
#include "SigmaClock.h"
#include "Utils.h"
#include "definitions.h"





class Configuration {
public:


	//Flags
	bool IsMqttReady() { return isMqttReady; }
	bool IsSimulator() { return isSimulator; }
	bool IsEthernetReady() { return isEthernetReady; }
	const char* BoardName() { return boardName; };

	// public members
	Loger* Log;
	DeviceManager* DevMgr;
	ScheduleManager* ScheduleMgr;
	SigmaClock* Clock;
	ScriptRunner Runner;


	//Timer 
	unsigned long Counter10 = 0;	//10ms
	unsigned long Counter1 = 0;		//1 sec
	unsigned long Counter30 = 0;	//30 sec
	unsigned long Counter60 = 0;	//1min


	// Methods
	void Init();
	void Loop(unsigned long timePeriod);

	// Config parameters settings
	const char* GetTimezone() { return timezone; };
	WORKMODE GetWorkMode() { return workMode; };
	WEEKMODE GetWeekMode() { return weekMode; };
	double GetManualTemp() { return manualTemp; };
	HEATMODE GetHeatMode() { return heatMode; }
	byte GetHysteresis() { return hysteresis; };
	bool GetSimulator() { return isSimulator; };
	CMD GetCommand() { return command; };
	double GetDesiredTemp();

	// Subscriptions
	void Subscribe(const char* topic);

	void PublishAlert(ALERTCODE code) { publishAlert(code, ScriptRunner::STEP_EMPTY, NULL); };
	void PublishAlert(ALERTCODE code, const char* name) { publishAlert(code, ScriptRunner::STEP_EMPTY, name); };
	void PublishAlert(ALERTCODE code, ScriptRunner::STEPS step) { publishAlert(code, step, NULL); };
	void Publish(DeviceType dType, const char* name, byte status);
	void Publish(DeviceType dType, const char* name, double status);
	void PublishStep(ScriptRunner::STEPS step);
	void PublishInfo(const char* txt);
	void Transfer(int n) { if (isMqttReady) { mqttClient->MqttLoop(n); } };
	void ProcessMessage(const char* topic, const char* payload);
	void SubscribeAll();
	void SubscribeEquipment(DeviceType dType, const char* name);
	void SubscribeStatus(DeviceType dType, const char* name);
	void SubscribeSchedule(int number);

private:

	typedef enum {
		PARAMS_IS_READY,
		PARAMS_WATCHDOG,
		PARAMS_SIMULATOR,
		PARAMS_WORKMODE,
		PARAMS_MANUAL_TEMP,
		PARAMS_DESIRED_TEMP,
		PARAMS_HEAT_COLD,
		PARAMS_HYSTERESIS,
		PARAMS_WEEKMODE,
		PARAMS_COMMAND,
		PARAMS_TIMEZONE,
		CONFIG_PARAMS_LAST
	} MqttConfigParam;

	typedef enum {
		SECTION_CONFIG,
		SECTION_ALERT,
		SECTION_WARNING,
		SECTION_EQUIPMENT,
		SECTION_STATUS,
		SECTION_SCHEDULE,
		MQTT_SECTION_LAST
	} MqttSection;

	typedef enum {
		ALERT_CODE,
		ALERT_TEXT,
		ALERT_SCRIPT,
		MQTT_ALERT_LAST
	} MqttAlertParam;

	typedef enum {
		SCHEDULE_WEEKEND,
		SCHEDULE_WORKDAY,
		SCHEDULE_SET_LAST
	} ScheduleSet;


	// Setters for config parameters
	void setWorkMode(byte b, bool save = true);
	void setWorkMode(const char* str) { setWorkMode(str[0] - '0'); };
	void setManualTemp(byte b, bool save = true);
	void setManualTemp(const char* str);
	void setWeekMode(byte b, bool save = true);
	void setWeekMode(const char* str) { setWeekMode(str[0] - '0'); };
	void setHysteresis(byte b, bool save = true);
	void setHysteresis(const char* str) { setHysteresis(str[0] - '0'); };
	void setHeatMode(byte b, bool save = true);
	void setHeatMode(const char* str) { setHeatMode(str[0] - '0'); };
	void setCommand(const char* str) { setCommand(str[0] - '0'); };
	void setCommand(byte b, bool save = true);
	void setSimulator(byte b, bool save = true);
	void setSimulator(const char* str) { setSimulator(str[0] - '0'); };
	void setTimeZone(const char* tz, bool save = true);


	byte BoardId() { return boardId; };

	const char* mQTT_ROOT = "HeatPump/";
	const char* mqttConfigParamName[CONFIG_PARAMS_LAST] = { "IsReady", "WatchDog", "Simulator", "Mode", "ManualTemp","DesiredTemp", "HeatCold" ,
									"Hysteresis", "WeekMode","Command","TimeZone" };
	const char* mqttSectionName[MQTT_SECTION_LAST] = { "Config/", "Alert/", "Warning/", "Equipment/", "Status/", "Schedule/" };
	const char* mqttAlertParamName[MQTT_ALERT_LAST] = { "Code", "Text", "Script" };
	const char* mqttDeviceTypeName[DEVICE_TYPE_LAST] = { "Relay/", "Contactor/", "Bus/", "Temperature/","Script/" };
	const char* mqttScheduleSetName[SCHEDULE_SET_LAST] = { "WeekEnd/", "WorkDays/" };
	void publish(const char* topic, const char* payload);

	//members
	bool isEthernetReady = false;
	bool isMqttReady = false;
	bool isConfigRequested = false;
	bool isSimulator = false;
	WEEKMODE weekMode = WEEKMODE_5_2;
	WORKMODE workMode = WORKMODE_MANUAL;
	HEATMODE heatMode = HEATMODE_HEAT;
	byte hysteresis = 5;
	CMD command = CMD_RUN;
	char timezone[TIMEZONE_LEN] = "EET";
	Mqtt* mqttClient;
	byte mac[6] = { 0x00, 0xAA, 0x22, 0x07, 0x69, 0x00 };
	IPAddress ip = IPAddress(192, 168, 0, 90);
	EthernetClient* ethClient;
	unsigned int mqttPort;
	IPAddress mqttIp = IPAddress(192, 168, 0, 90);
	byte boardId;
	char boardName[10];
	double manualTemp = 20.0;
	double desiredTemp = 20.0;

	char topicRoot[MQTT_TOPIC_LENGTH + 1];
	//methods

	void readBoardId();
	void setBoardId(byte id);
	void setIp(byte ip0, byte ip1, byte ip2, byte ip3);
	void initializeEthernet();
	void unitsLoop(unsigned long timePeriod);

	void subscribeParameters();
	void readConfigEEPROM();
	void publishParameters();
	void publishConfigParameter(MqttConfigParam parmId, double payload);
	void updateConfig(const char* topic, const char* payload);
	void initMqttTopics();
	void publishConfigParameter(MqttConfigParam parmId, const char* payload);
	void subscribeConfigParameter(MqttConfigParam parmId);
	void publishConfigParameter(MqttConfigParam parmId, byte payload);
	void publishStatus(DeviceType dType, const char* name, const char* payload);
	void publishAlert(ALERTCODE code, ScriptRunner::STEPS step, const char* name);
};

