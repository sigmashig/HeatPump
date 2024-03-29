#pragma once
#include <Arduino.h>
#include <Ethernet.h>

#include "Loger.h"
#include "Mqtt.h"
#include "DeviceManager.h"
#include "Schedule.h"
#include "ScriptRunner.h"
#include "ScheduleManager.h"
#include "SigmaClock.hpp"
#include "Utils.h"
#include "definitions.h"


#define WATCHDOG_PUBLICATION_INTERVAL ((unsigned long)2*60*1000) //2 min
//#define WATCHDOG_PUBLICATION_INTERVAL ((unsigned long)30*1000) //2 min


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
	ScriptRunner Runner;


	//Timer 
	unsigned long Counter0 = 0;		//Runtime
	unsigned long Counter1 = 0;		//1 sec
	unsigned long Counter10 = 0;	//10s
	unsigned long Counter30 = 0;	//30 sec
	unsigned long Counter60 = 0;	//1min



// Methods
	void Init();
	void Loop(unsigned long timePeriod);

	// Config parameters settings
//	const char* GetTimezone() { return timezone; };
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

	void PublishAlert(ALERTCODE code, const char* name) { publishAlert(code, ScriptRunner::STEP_EMPTY, name); };
	void Publish(DeviceType dType, const char* name, byte status);
    void Publish(DeviceType dType, const char* name, const char* payload);
	void Publish(DeviceType dType, const char* name, double status);
	void PublishStep(ScriptRunner::STEPS step);
	void PublishInfo(const char* txt);
	void Transfer(int n) { if (isMqttReady) { mqttClient->MqttLoop(n); } };
	void ProcessMessage(const char* topic, const char* payload);
  	void SubscribeAll();
	void SubscribeEquipment(DeviceType dType, const char* name);
	void SubscribeStatus(DeviceType dType, const char* name);
	void SubscribeSchedule(int number);
	//void PublishSchedule(int number);
	void PublishEquipment(DeviceType dType, const char* name, const char* payload);
	
	byte GetLengthRootTopic() { return lengthOfRoot; }
	void WatchDogPublication();

    void PublishLog(DebugLevel level, const char* message);
    tm GetTime();

	
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
		//PARAMS_TIMEZONE,
		PARAMS_IP,
		PARAMS_BOARD_ID,
		PARAMS_MQTT_IP,
		PARAMS_MQTT_PORT,
		//PARAMS_CLOCK_TYPE,
		PARAMS_VERSION,
		PARAMS_RESET,
		CONFIG_PARAMS_LAST
	} MqttConfigParam;

	typedef enum {
		SECTION_CONFIG,
		SECTION_ALERT,
		SECTION_WARNING,
		SECTION_EQUIPMENT,
		SECTION_STATUS,
		SECTION_SCHEDULE_WEEKEND,
		SECTION_SCHEDULE_WORKDAYS,
		SECTION_LOG,
		MQTT_SECTION_LAST
	} MqttSection;

	typedef enum {
		ALERT_CODE,
		ALERT_TEXT,
		MQTT_ALERT_LAST
	} MqttAlertParam;
	/*
		typedef enum {
			SCHEDULE_WEEKEND,
			SCHEDULE_WORKDAY,
			SCHEDULE_SET_LAST
		} ScheduleSet;
	*/
/*
	typedef enum {
		CLOCK_DS3231,
		CLOCK_DS1302
	} ClockType;

	struct Clock {
		ClockType type;
		DS1302_Pins pins;
	} clock;
*/
	// Setters for config parameters
	void setWorkMode(byte b, bool save = true);
	void setManualTemp(double d, bool save = true);
    void setDesiredTemp(double t);
	void setWeekMode(byte b, bool save = true);
	void setHysteresis(byte b, bool save = true);
	void setHeatMode(byte b, bool save = true);
	void setCommand(byte b, bool save = true);
	void setSimulator(byte b, bool save = true);
//	void setTimeZone(const char* tz, bool save = true);
	void setMqttIp(IPAddress& ip, bool save = true);
	void setMqttPort(uint16_t port, bool save = true);
	void setIp(IPAddress& ipNew, bool save = true);
//	void setClockType(byte b, bool save = true);
	void setBoardId(byte id, bool save = true);
	

	byte BoardId() { return boardId; };

	const char* mQTT_ROOT = "HeatPump/";
	const char* mqttConfigParamName[CONFIG_PARAMS_LAST] = { "IsReady", "WatchDog", "Simulator", "Mode", "ManualTemp","DesiredTemp", "HeatCold" ,
										"Hysteresis", "WeekMode","Command",/*"TimeZone",*/"IP","BoardId","MqttIP","MqttPort",/*"ClockType",*/"Version",
										"Reset"};

	const char* mqttSectionName[MQTT_SECTION_LAST] = { "Config/", "Alert/", "Warning/", "Equipment/", "Status/", "Schedule/Weekend/Set_", "Schedule/Workdays/Set_","Log/" };
	const char* mqttAlertParamName[MQTT_ALERT_LAST] = { "Code", "Text"};
	const char* mqttDeviceTypeName[DEVICE_TYPE_LAST] = { "Relay/", "Contactor/", "Bus/", "Temperature/","Script/", "Clock/" };
	const char* LOG_END[8] = { "OFF", "INTERNAL","FATAL","ERROR","WARN","INFO","DEBUG","ALL" };


	//const char* mqttScheduleSetName[SCHEDULE_SET_LAST] = { "WeekEnd/", "WorkDays/" };
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
//	char timezone[TIMEZONE_LEN] = "EET";
	Mqtt* mqttClient;
	byte mac[6] = { 0x00, 0xAA, 0x22, 0x07, 0x69, 0x00 };
	IPAddress ip = IPAddress(192, 168, 0, 200);
	EthernetClient* ethClient;
	unsigned int mqttPort;
	IPAddress mqttIp = IPAddress(192, 168, 0, 99);
	byte boardId;
	char boardName[10];
	double manualTemp = 20.0;
	double desiredTemp = 20.0;
	byte lengthOfRoot = 0;
	unsigned long lastWatchDogPublication = millis();

	//methods

	void readBoardId();
	void initializeEthernet();
	void unitsLoop(unsigned long timePeriod);

	void subscribeParameters();
	void readConfigEEPROM();
	void publishParameters();
	void publishConfigParameter(MqttConfigParam parmId, double payload);
	void updateConfig(const char* topic, const char* payload);
	void publishConfigParameter(MqttConfigParam parmId, const char* payload);
	void subscribeConfigParameter(MqttConfigParam parmId);
	void publishConfigParameter(MqttConfigParam parmId, byte payload);
	
	void publishStatus(DeviceType dType, const char* name, const char* payload);
	void publishAlert(ALERTCODE code, ScriptRunner::STEPS step, const char* name);

	void updateSingleParam(MqttConfigParam parm, const char* payload);

	void checkWatchDogPublication();

	void testTemperature();

};

