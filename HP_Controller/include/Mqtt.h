#pragma once
#include "PubSubClient.h"
#include <Ethernet.h>

#include "definitions.h"

#define MQTT_INITIAL_RETRY_DELAY	3000 //delay between INITIAL reconnection retry
#define MQTT_RETRY_TIME				10000 //delay between mqtt reconnect in loop
#define MQTT_TOPIC_LENGTH	100
#define MQTT_PAYLOAD_LENGTH	200
#define MQTT_TRY_COUNT 5

#define MQTT_WATCH_DOG_PUBLICATION "Config/WatchDogPublication"

void callbackFunc(char* topic, uint8_t* payload, unsigned int length);

class Mqtt:
	public PubSubClient {
public:

	//methods
	Mqtt(IPAddress ip, unsigned int port, EthernetClient& eth, const char* root);
	void Init();
	bool Publish(const char* topic, const char* payload);
	void Subscribe(const char* topic);
	//	void PublishLog(DebugLevel level, const char* message);
	bool simpleLoop();
	void MqttLoop(int n = 0);
	void FinalInit();
	bool IsMqtt() { return connected(); }
	void SubscribeWatchDogPublication(){ subscribe(MQTT_WATCH_DOG_PUBLICATION); };
private:
	unsigned long mqttWaiting;
	char topicRoot[CONFIG_LENGTH_OF_ROOT + 1];

	bool mqttReconnect();
	void repeatedLoop(int n);
};

