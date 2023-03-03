#include "Mqtt.h"

#include "Configuration.h"


extern Configuration Config;

void callbackFunc(char* topic, uint8_t* payload, unsigned int length) {
	if (length <= MQTT_PAYLOAD_LENGTH && strlen(topic) <= MQTT_TOPIC_LENGTH) {
		char* pl = (char*)payload;
		pl[length] = 0;
		Config.Log->append("[").append(topic).append("]:").append((char*)payload).Info();
		Config.ProcessMessage(topic, (char*)payload);
	} else {
		Config.Log->Debug("Wrong Length");
	}
	
}


Mqtt::Mqtt(IPAddress ip, unsigned int port, EthernetClient& eth, const char* root) : 
	PubSubClient(ip, port, callbackFunc, eth) {
	topicRoot = root;
	mqttWaiting = MQTT_INITIAL_RETRY_DELAY;
}

void Mqtt::Init()
{
	if (Config.IsEthernetReady()) {
		long connectTry = 0;
		bool res = false;

		while (!res && connectTry <= MQTT_TRY_COUNT) {
			res = mqttReconnect();
			if (res)
				delay(MQTT_INITIAL_RETRY_DELAY);
			connectTry++;
		}
		if (!res) {
			Config.Log->Error(F("Too many attempt of MQTT reconnect"));
		}
		else {
			//delay(2000); //mqtt iob has delay 2 sec
		}
	}
}

void Mqtt::FinalInit()
{
}

bool Mqtt::mqttReconnect() {

	bool res = false;

	if (!connected()) {
		Config.Log->Info(F("Mqtt Reconnecting"));
		if (connect(Config.BoardName())) {
			res = true;
		}
		else {
			Config.Log->Error(F("MQTT connection Failed"));
			res = false;
		}
	}

	return res;
}

void Mqtt::RepeatedLoop(int n) {

	for (n *= 2; n >= 0; n--) {
		MqttLoop();
		delay(30);
	}
}
bool Mqtt::Publish(const char* topic, const char* payload) {
	createSafeString(topicFull, MQTT_TOPIC_LENGTH);
	topicFull = topicRoot;
	topicFull += topic;
	Config.Log->append(F("Publish [")).append(topicFull.c_str()).append(F("]:")).append(payload).Debug();
	if (connected()) {
		return publish(topicFull.c_str(), payload);
	}
	else {
		return false;
	}
}

void Mqtt::Subscribe(const char* topic) {
	createSafeString(topicFull, MQTT_TOPIC_LENGTH);
	topicFull = topicRoot;
	topicFull += topic;
	Config.Log->append(F("Subscription:")).append(topicFull.c_str()).Debug();
	if (connected()) {
		subscribe(topicFull.c_str());
	}
}
/*
void Mqtt::PublishLog(DebugLevel level, const char* message) {

	if (connected()) {
		sprintf(topicLog, MQTT_LOG, Config.BoardId(), LOG_END[level]);
		publish(topicLog, message);
	}
}
*/
bool Mqtt::simpleLoop() {
	static long lastConnected = 0;
	bool res = false;

	if (connected()) {
		res = loop();
		if (!res) {
			Config.Log->Error(F("Failed loop"));
		}

		lastConnected = millis();
	}
	else {
		if (lastConnected + millis() <= MQTT_RETRY_TIME) {
			Config.Log->Info(F("Trying to reconnect MQTT"));
			res = mqttReconnect();
			if (res) {
				Config.SubscribeAll();
			}
		}
	}
	return res;
}

void Mqtt::MqttLoop(int n) {
	Config.Log->append("MqttLoop:").append(n).Debug();
	for (n *= 2; n >= 0; n--) {
		simpleLoop();
		delay(100);
	}
	Config.Log->Debug("Endloop");
}

