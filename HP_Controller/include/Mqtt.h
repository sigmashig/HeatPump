#pragma once
#include "PubSubClient.h"
#include <Ethernet.h>

#include "definitions.h"

#define MQTT_INITIAL_RETRY_DELAY	3000 //delay between INITIAL reconnection retry
#define MQTT_RETRY_TIME				10000 //delay between mqtt reconnect in loop
#define MQTT_TOPIC_LENGTH	100
#define MQTT_PAYLOAD_LENGTH	200
#define MQTT_TRY_COUNT 5

#define MQTT_ROOT "HeatPump"
#define MQTT_BOARD_ID "Board_%02d"
#define MQTT_SEPARATOR "/"



#define MQTT_CONFIG		MQTT_ROOT MQTT_SEPARATOR MQTT_BOARD_ID MQTT_SEPARATOR "Config"
#define MQTT_LOG		MQTT_ROOT MQTT_SEPARATOR MQTT_BOARD_ID MQTT_SEPARATOR "Logs" MQTT_SEPARATOR "%s"
#define MQTT_EQUIPMENT	MQTT_ROOT MQTT_SEPARATOR MQTT_BOARD_ID MQTT_SEPARATOR "Equipment"
#define MQTT_SCHEDULE	MQTT_ROOT MQTT_SEPARATOR MQTT_BOARD_ID MQTT_SEPARATOR "Schedule"
#define MQTT_STATUS		MQTT_ROOT MQTT_SEPARATOR MQTT_BOARD_ID MQTT_SEPARATOR "Status"
#define MQTT_ALERT		MQTT_ROOT MQTT_SEPARATOR MQTT_BOARD_ID MQTT_SEPARATOR "Alert"

#define MQTT_WATCH_DOG2		MQTT_CONFIG MQTT_SEPARATOR "WatchDog2"
#define MQTT_SIMULATOR		MQTT_CONFIG MQTT_SEPARATOR "Simulator"

#define	MQTT_MODE			MQTT_CONFIG MQTT_SEPARATOR "Mode"
#define	MQTT_MANUAL_TEMP	MQTT_CONFIG MQTT_SEPARATOR "ManualTemp"
#define	MQTT_HEAT_COLD		MQTT_CONFIG MQTT_SEPARATOR "HeatCold"
#define	MQTT_HYSTERESIS		MQTT_CONFIG MQTT_SEPARATOR "Hysteresis"
#define	MQTT_WEEKMODE		MQTT_CONFIG MQTT_SEPARATOR "WeekMode"
#define	MQTT_COMMAND		MQTT_CONFIG MQTT_SEPARATOR "Command"
#define	MQTT_TIMEZONE		MQTT_CONFIG MQTT_SEPARATOR "TimeZone"


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


void callbackFunc(char* topic, uint8_t* payload, unsigned int length);

class Mqtt :
    public PubSubClient
{
public:

//	char TopicBuff[MQTT_TOPIC_LENGTH];
//	char PayloadBuff[MQTT_PAYLOAD_LENGTH];

	
	//methods
    Mqtt(IPAddress ip, unsigned int port, EthernetClient& eth);
    void Init();
 //   void PutBuffer(const char* topic, const char* payload, unsigned int len);
//	void InitNEW();
	bool Publish(const char* topic, const char* payload);
	void Subscribe(const char* topic);
//	void PublishLog(DebugLevel level, const char* message);
	bool simpleLoop();
	void MqttLoop(int n = 0);
	void FinalInit();
//	void PublishConfig();
//	void PublishSchedule();
//	void PublishEquipment();
	bool IsMqtt() { return connected(); }


private:
    unsigned long mqttWaiting;
	//const char* LOG_END[7] = { "OFF", "FATAL","ERROR","WARN","INFO","DEBUG","ALL" };
	//char topicLog[MQTT_TOPIC_LENGTH];
	//char topicCB[MQTT_TOPIC_LENGTH];
	//char payloadCB[MQTT_PAYLOAD_LENGTH];
//	unsigned int lenCB;

	bool mqttReconnect();
//	void idleLoop();
//	void subscribeAlert();
//	void subscribeConfig();
	void RepeatedLoop(int n);
//	void subscribeEquipment();
//	void subscribeStatuses();
//	void subscribeSchedules();
//	void callback();
//	void watchDog2();

//	void updateEquipment(const char* topic, const char* payload);
//	void updateStatus(const char* topic, const char* payload);
//	void updateSchedule(const char* topic, const char* payload);
//	void updateConfig(const char* topic, const char* payload);
//	void updateAlert(const char* topic, const char* payload);
};

