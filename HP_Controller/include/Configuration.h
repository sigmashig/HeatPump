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


class Configuration
{
public:
	typedef enum {
		MODE_MANUAL = 0,
		MODE_SCHEDULE = 1
	} MODE;

	typedef enum {
		MODE_WEEK_5_2 = 0,
		MODE_WEEK_6_1 = 1,
		MODE_WEEK_7_0 = 2
	} WEEKMODE;


	typedef enum {
		MODE_HEAT = 0,
		MODE_COLD = 1
	} HEATMODE;

	typedef enum {
		CMD_NOCMD = 0,
		CMD_RUN = 1,
		CMD_STOP = 2
	} CMD;

	//Flags
	bool IsMqttReady() { return isMqttReady; }
	bool IsSimulator() { return isSimulator;  }
	bool IsEthernetReady() { return isEthernetReady; }

	//const 
	byte BoardId() { return boardId; };
	const char* BoardName() { return boardName; };


	// public members
	Loger* Log;
	Mqtt* MqttClient;
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
	MODE GetMode() { return mode; };
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
	CMD GetCommand() { return command; };
	void Transfer(int n) { if (isMqttReady) { MqttClient->MqttLoop(n); } };
	void SubscribeAll();
	void Subscribe(const char* topic);
	void ProcessMessage(const char* topic, const char* payload);
	void Publish(const char* topic, const char* payload);

	char TopicBuff[MQTT_TOPIC_LENGTH];
	char PayloadBuff[MQTT_PAYLOAD_LENGTH];


private:


	//members
	bool isEthernetReady = false;
	bool isMqttReady = false;
	bool isConfigRequested = false;
	bool isSimulator = false;
	WEEKMODE weekMode = MODE_WEEK_5_2;
	MODE mode = MODE_MANUAL;
	HEATMODE heatMode = MODE_HEAT;
	byte hysteresis = 5;
	CMD command = CMD_RUN;
	char timezone[TIMEZONE_LEN]="EET";

	byte mac[6] = { 0x00, 0xAA, 0x22, 0x07, 0x69, 0x00 };
	IPAddress ip = IPAddress(192, 168, 0, 90);
	EthernetClient* ethClient;
	unsigned int mqttPort;
	IPAddress mqttIp = IPAddress(192, 168, 0, 90);
	byte boardId;
	char boardName[10];
	double manualTemp = 20.0;
	double desiredTemp = 20.0;

	//methods

	void readBoardId();
	void setBoardId(byte id);
	void setIp(byte ip0, byte ip1, byte ip2, byte ip3);
	void initializeEthernet();
	void unitsLoop(unsigned long timePeriod);
	void subscribeParameters();
	void publishTimezone();
	void readConfigEEPROM();

	void setMode(byte b);
	void setManualTemp(byte b);
	void setWeekMode(byte b);
	void setHysteresis(byte b);
	void setHeatCold(byte b);
	void setCmd(byte b);
	void setSimulator(byte b);
	void setTimeZone(const char* tz);
	void publishParameters();



	void publishMode();

	void publishWeekMode();

	void publishManualTemp();

	void publishHeatCold();

	void publishHysteresis();

	void publishSimulator();

	void publishCmd();


	void updateConfig(const char* topic, const char* payload);


};

