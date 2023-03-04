#pragma once

#include <Ethernet.h>

//constexpr auto TIMEZONE_LEN = 50;


#include "Loger.h"
#include "Mqtt.h"
#include "DeviceManager.h"
#include "Schedule.h"
#include "ScriptRunner.h"
#include "ScheduleManager.h"
#include "SigmaClock.h"
#include "Utils.h"
#include "definitions.h"

/*
#define MQTT_ROOT "HeatPump"
//#define MQTT_BOARD_ID "Board_%02d"
#define MQTT_SEPARATOR "/"

#define MQTT_CONFIG			"Config"
#define MQTT_IS_READY 		"IsReady"
#define MQTT_WATCH_DOG2 	"WatchDog2"
#define MQTT_SIMULATOR 		"Simulator"
#define	MQTT_MODE	 		"Mode"
#define	MQTT_MANUAL_TEMP	"ManualTemp"
#define	MQTT_DESIRED_TEMP	"DesiredTemp"
#define	MQTT_HEAT_COLD		"HeatCold"
#define	MQTT_HYSTERESIS		"Hysteresis"
#define	MQTT_WEEKMODE		"WeekMode"
#define	MQTT_COMMAND		"Command"
#define	MQTT_TIMEZONE		"TimeZone"

#define MQTT_ALERT			"Alert"
#define MQTT_ALERT_CODE		"Code"
#define MQTT_ALERT_MSG		"Text"
#define MQTT_ALERT_SCRIPT	"Script"

#define MQTT_EQUIPMENT		"Equipment"
#define MQTT_SCHEDULE		"Schedule"
#define MQTT_STATUS			"Status"
*/

//#define MQTT_CONFIG		MQTT_ROOT MQTT_SEPARATOR MQTT_BOARD_ID MQTT_SEPARATOR "Config"
/*
#define MQTT_LOG		MQTT_ROOT MQTT_SEPARATOR MQTT_BOARD_ID MQTT_SEPARATOR "Logs" MQTT_SEPARATOR "%s"
#define MQTT_EQUIPMENT	MQTT_ROOT MQTT_SEPARATOR MQTT_BOARD_ID MQTT_SEPARATOR "Equipment"
#define MQTT_SCHEDULE	MQTT_ROOT MQTT_SEPARATOR MQTT_BOARD_ID MQTT_SEPARATOR "Schedule"
#define MQTT_STATUS		MQTT_ROOT MQTT_SEPARATOR MQTT_BOARD_ID MQTT_SEPARATOR "Status"
#define MQTT_ALERT		MQTT_ROOT MQTT_SEPARATOR MQTT_BOARD_ID MQTT_SEPARATOR "Alert"

#define MQTT_ALERT_CODE	MQTT_ALERT MQTT_SEPARATOR "%s" MQTT_SEPARATOR "Code"
#define MQTT_ALERT_MSG	MQTT_ALERT MQTT_SEPARATOR "%s" MQTT_SEPARATOR "Text"

#define MQTT_STEP		MQTT_STATUS MQTT_SEPARATOR "Step"
#define MQTT_INFO		MQTT_STATUS MQTT_SEPARATOR "Info"



#define MQTT_ALERT_STEP		MQTT_ALERT MQTT_SEPARATOR "Step"
#define MQTT_ALERT_DEVICE	MQTT_ALERT MQTT_SEPARATOR "%s"

//Schedule

#define MQTT_SCHEDULE_WORKDAY		MQTT_SCHEDULE MQTT_SEPARATOR "WorkDay"
#define MQTT_SCHEDULE_WEEKEND		MQTT_SCHEDULE MQTT_SEPARATOR "WeekEnd"
#define MQTT_SCHEDULE_WORKDAY_SET	MQTT_SCHEDULE_WORKDAY MQTT_SEPARATOR "Set%02d"
#define MQTT_SCHEDULE_WEEKEND_SET	MQTT_SCHEDULE_WEEKEND MQTT_SEPARATOR "Set%02d"

//Equipment
#define MQTT_RELAYS			"Relay"
#define MQTT_THERMOMETERS	"Temperature"
#define MQTT_POWERMETERS	"PowerMeter"
#define MQTT_CONTACTORS		"Contactor"

#define MQTT_EQUIPMENT_RELAYS		MQTT_EQUIPMENT MQTT_SEPARATOR MQTT_RELAYS
#define MQTT_EQUIPMENT_RELAY		MQTT_EQUIPMENT_RELAYS MQTT_SEPARATOR "%s"
#define MQTT_EQUIPMENT_THERMOMETERS	MQTT_EQUIPMENT MQTT_SEPARATOR MQTT_THERMOMETERS 
#define MQTT_EQUIPMENT_THERMOMETER	MQTT_EQUIPMENT_THERMOMETERS MQTT_SEPARATOR "%s"
#define MQTT_EQUIPMENT_POWERMETERS	MQTT_EQUIPMENT MQTT_SEPARATOR MQTT_POWERMETERS 
#define MQTT_EQUIPMENT_POWERMETER	MQTT_EQUIPMENT_POWERMETERS MQTT_MQTT_SEPARATOR "%s"
#define MQTT_EQUIPMENT_CONTACTORS	MQTT_EQUIPMENT MQTT_SEPARATOR MQTT_CONTACTORS
#define MQTT_EQUIPMENT_CONTACTOR	MQTT_EQUIPMENT_CONTACTORS MQTT_SEPARATOR "%s"

//Status
#define MQTT_STATUS_RELAYS			MQTT_STATUS MQTT_SEPARATOR MQTT_RELAYS
#define MQTT_STATUS_RELAY			MQTT_STATUS_RELAYS MQTT_SEPARATOR "%s"
#define MQTT_STATUS_THERMOMETERS	MQTT_STATUS MQTT_SEPARATOR MQTT_THERMOMETERS
#define MQTT_STATUS_THERMOMETER		MQTT_STATUS_THERMOMETERS MQTT_SEPARATOR "%s"
#define MQTT_STATUS_POWERMETERS		MQTT_STATUS MQTT_SEPARATOR MQTT_POWERMETERS
#define MQTT_STATUS_POWERMETER		MQTT_STATUS_POWERMETERS MQTT_SEPARATOR "%s"
#define MQTT_STATUS_CONTACTORS		MQTT_STATUS MQTT_SEPARATOR MQTT_CONTACTORS
#define	MQTT_STATUS_CONTACTOR		MQTT_STATUS_CONTACTORS MQTT_SEPARATOR "%s"
*/



class Configuration
{
public:

	
	//Flags
	bool IsMqttReady() { return isMqttReady; }
	bool IsSimulator() { return isSimulator;  }
	bool IsEthernetReady() { return isEthernetReady; }

	//const 
	byte BoardId() { return boardId; };
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
	void SetTimezone(const char* tzNew) { setTimeZone(tzNew); };
	const char* GetTimezone() { return timezone; };
	void SetMode(const char* str) { setMode(str[0] - '0'); };
	WORKMODE GetMode() { return mode; };
	void SetWeekMode(const char* str) { setWeekMode(str[0] - '0'); };
	WEEKMODE GetWeekMode() { return weekMode; };
	void SetManualTemp(const char* str);
	double GetManualTemp() { return manualTemp; };
	void SetHeatMode(const char* str) { setHeatCold(str[0] - '0'); };
	HEATMODE GetHeatMode() { return heatMode;  }
	void SetHysteresis(const char* str) { setHysteresis(str[0] - '0'); };
	byte GetHysteresis() { return hysteresis; };
	void SetSimulator(const char* str) { setSimulator(str[0] - '0'); };
	bool GetSimulator() { return isSimulator; };
	void SetCommand(const char* str) { setCmd(str[0] - '0'); };
	void SetCommand(CMD cmd) { setCmd(cmd); };
	CMD GetCommand() { return command; };
	double GetDesiredTemp();
	
	void Transfer(int n) { if (isMqttReady) { mqttClient->MqttLoop(n); } };
	void SubscribeAll();
	void Subscribe(const char* topic);
	void ProcessMessage(const char* topic, const char* payload);

	//void Publish() { Publish(TopicBuff, PayloadBuff); };

	//char TopicBuff[MQTT_TOPIC_LENGTH];
	//char PayloadBuff[MQTT_PAYLOAD_LENGTH];
	void PublishAlert(ALERTCODE code) { publishAlert(code, ScriptRunner::STEP_EMPTY, NULL); };
	void PublishAlert(ALERTCODE code, const char* name) { publishAlert(code, ScriptRunner::STEP_EMPTY, name); };
	void PublishAlert(ALERTCODE code, ScriptRunner::STEPS step) { publishAlert(code, step, NULL); };
	void Publish(DeviceType dType, const char* name, byte status);
	void Publish(DeviceType dType, const char* name, double status);
	void PublishStep(ScriptRunner::STEPS step);
	void SubscribeEquipment(DeviceType dType, const char* name);
	void SubscribeStatus(DeviceType dType, const char* name);
    void SubscribeSchedule(int number);
	void PublishInfo(const char* txt);

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
	
	const char* MQTT_ROOT = "HeatPump/";
	//const char 	MQTT_SEPARATOR = '/';
	const char* MqttConfigParamName[CONFIG_PARAMS_LAST] = { "IsReady", "WatchDog", "Simulator", "Mode", "ManualTemp","DesiredTemp", "HeatCold" ,
									"Hysteresis", "WeekMode","Command","TimeZone" };
	const char* MqttSectionName[MQTT_SECTION_LAST] = { "Config/", "Alert/", "Warning/", "Equipment/", "Status/", "Schedule/" };
	const char* MqttAlertParamName[MQTT_ALERT_LAST] = { "Code", "Text", "Script" };
	const char* MqttDeviceTypeName[DEVICE_TYPE_LAST] = { "Relay/", "Contactor/", "Bus/", "Temperature/","Script/" };
	const char* MqttScheduleSetName[SCHEDULE_SET_LAST] = { "WeekEnd/", "WorkDays/" };

	void Publish(const char* topic, const char* payload);

	//members
	bool isEthernetReady = false;
	bool isMqttReady = false;
	bool isConfigRequested = false;
	bool isSimulator = false;
	WEEKMODE weekMode = WEEKMODE_5_2;
	WORKMODE mode = WORKMODE_MANUAL;
	HEATMODE heatMode = HEATMODE_HEAT;
	byte hysteresis = 5;
	CMD command = CMD_RUN;
	char timezone[TIMEZONE_LEN]="EET";
	Mqtt *mqttClient;

	byte mac[6] = { 0x00, 0xAA, 0x22, 0x07, 0x69, 0x00 };
	IPAddress ip = IPAddress(192, 168, 0, 90);
	EthernetClient* ethClient;
	unsigned int mqttPort;
	IPAddress mqttIp = IPAddress(192, 168, 0, 90);
	byte boardId;
	char boardName[10];
	double manualTemp = 20.0;
	double desiredTemp = 20.0;

	char topicRoot[MQTT_TOPIC_LENGTH+1];
	//methods

	void readBoardId();
	void setBoardId(byte id);
	void setIp(byte ip0, byte ip1, byte ip2, byte ip3);
	void initializeEthernet();
    
	void unitsLoop(unsigned long timePeriod);
	void subscribeParameters();
	void publishTimezone();
	void readConfigEEPROM();

	void setMode(byte b, bool save = true);
	void setManualTemp(byte b, bool save = true);
	void setWeekMode(byte b, bool save = true);
	void setHysteresis(byte b, bool save = true);
	void setHeatCold(byte b, bool save = true);
	void setCmd(byte b, bool save = true);
	void setSimulator(byte b, bool save = true);
	void setTimeZone(const char* tz, bool save = true);
	void publishParameters();
	void publishMode();

	void publishConfigParameter(MqttConfigParam parmId, double payload);
	//void publishManualTemp();
    //void publishDesiredTemp();
	//void publishHeatCold();
	//void publishHysteresis();
	//void publishSimulator();
	//void publishCmd();
	void updateConfig(const char* topic, const char* payload);
    void initMqttTopics();
    void publishConfigParameter(MqttConfigParam parmId, const char* payload);
    void subscribeConfigParameter(MqttConfigParam parmId);
    void publishConfigParameter(MqttConfigParam parmId, byte payload);

	//void createMqttConfigParamName(SafeString& str, MqttConfigParam.parmId) 
    void publishStatus(DeviceType dType, const char* name, const char* payload);
	void publishAlert(ALERTCODE code, ScriptRunner::STEPS step, const char* name);

    


};

