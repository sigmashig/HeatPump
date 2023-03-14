#include "Mqtt.h"

#include "Configuration.h"
//#include "MemoryExplorer.h"

extern Configuration Config;

void callbackFunc(char* topic, uint8_t* payload, unsigned int length) {
	if (length <= MQTT_PAYLOAD_LENGTH && strlen(topic) <= MQTT_TOPIC_LENGTH) {
		payload[length] = '\0';
		Config.Log->append("[").append(topic).append("]:(").append(length).append(")").append((char*)payload).Info();
		if (strcmp(topic, MQTT_WATCH_DOG_PUBLICATION) == 0) {
			Config.WatchDogPublication();
		} else {
			Config.ProcessMessage(topic + Config.GetLengthRootTopic(), (char*)payload);
		}
	} else {
		Config.Log->Error("Wrong Length");
	}
}


Mqtt::Mqtt(IPAddress ip, unsigned int port, EthernetClient& eth, const char* root) : 
	PubSubClient(ip, port, callbackFunc, eth) {
	strcpy(topicRoot,root);
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
	} else {
		Config.Log->Error(F("Ethernet not ready"));
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

void Mqtt::repeatedLoop(int n) {

	for (n *= 2; n >= 0; n--) {
		MqttLoop();
		delay(30);
	}
}
bool Mqtt::Publish(const char* topic, const char* payload) {
	bool res = false;
	createSafeString(topicFull, MQTT_TOPIC_LENGTH);
	topicFull = topicRoot;
	topicFull += topic;
	if (connected()) {
		res = publish(topicFull.c_str(), payload);
		Config.Log->append(F("Publish:")).append(topicFull.c_str()).append(F("->")).append(payload).Internal();
	}
	else {
		res = false;
	}
	return res;
}

void Mqtt::Subscribe(const char* topic) {
	createSafeString(topicFull1, MQTT_TOPIC_LENGTH);
	topicFull1 = topicRoot;
	topicFull1 += topic;
	Config.Log->append(F("Subscription:")).append(topicFull1.c_str()).Info();
	if (connected()) {
		subscribe(topicFull1.c_str());
	}
}
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
	for (n *= 2; n >= 0; n--) {
		simpleLoop();
		delay(100);
	}
}

