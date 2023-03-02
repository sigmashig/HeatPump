#include "Mqtt.h"

#include "Configuration.h"


extern Configuration Config;
/*
void callbackFunc(char* topic, uint8_t* payload, unsigned int length) {
	char* pl = (char*)payload;
	pl[length] = 0;
	Config.MqttClient->PutBuffer(topic, pl, length);
}
*/

void callbackFunc(char* topic, uint8_t* payload, unsigned int length) {
	if (length <= MQTT_PAYLOAD_LENGTH && strlen(topic) <= MQTT_TOPIC_LENGTH) {
		char* pl = (char*)payload;
		pl[length] = 0;
		Config.Log->append("[").append(topic).append("]:").append((char*)payload).Info();
		Config.ProcessMessage(topic, (char*)payload);
	}
}


Mqtt::Mqtt(IPAddress ip, unsigned int port, EthernetClient& eth) : 
	PubSubClient(ip, port, callbackFunc, eth) {
	mqttWaiting = MQTT_INITIAL_RETRY_DELAY;
}

/*
void Mqtt::PutBuffer(const char* topic, const char* payload, unsigned int length)
{
	if (length <= MQTT_PAYLOAD_LENGTH && strlen(topic) <= MQTT_TOPIC_LENGTH) {
		strncpy(topicCB, topic, MQTT_TOPIC_LENGTH);
		strncpy(payloadCB, payload, length + 1);
		payloadCB[length] = 0;
		lenCB = length;
	}
	else {
		lenCB = 0;
	}
}
*/
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

/*
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
			delay(2000); //mqtt iob has delay 2 sec
			subscribeAlert();
			subscribeConfig();
			subscribeEquipment();
			subscribeSchedules();
		}
	}
}
*/
void Mqtt::FinalInit()
{
//	subscribeStatuses();

//	if (!connected()) {
//		Config.Log->Info(F("MqttReconnect"));
//		if (connect(Config.BoardName())) {
//			if (!Config.IsSimulator()) {
//				subscribeEquipment();
//				subscribeStatuses();
//			}
//		}
//	}
}

bool Mqtt::mqttReconnect() {

	bool res = false;

	if (!connected()) {
		Config.Log->Info(F("Mqtt Reconnecting"));
		if (connect(Config.BoardName())) {
//			idleLoop(); // clean the buffer
//			Config.Subscribe();
			res = true;
		}
		else {
			Config.Log->Error(F("MQTT connection Failed"));
			res = false;
		}
	}

	return res;
}

/*
void Mqtt::idleLoop()
{
	while (lenCB > 0) {
		lenCB = 0;
		loop();
	}
}
*/
/*
void Mqtt::subscribeAlert()
{
	// do nothing
}
*/
/*
void Mqtt::subscribeConfig()
{
	int n = 0;
	sprintf(TopicBuff, MQTT_WATCH_DOG2, Config.BoardId());
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_MODE, Config.BoardId());
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_WEEKMODE, Config.BoardId());
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_MANUAL_TEMP, Config.BoardId());
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_HEAT_COLD, Config.BoardId());
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_HYSTERESIS, Config.BoardId());
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_TIMEZONE, Config.BoardId());
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_SIMULATOR, Config.BoardId());
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_COMMAND, Config.BoardId());
	Subscribe(TopicBuff);
	n++;
	RepeatedLoop(n);
}
*/
void Mqtt::RepeatedLoop(int n) {

	for (n *= 2; n >= 0; n--) {
		MqttLoop();
		delay(30);
	}
}
/*
void Mqtt::subscribeSchedules()
{
	for (int i = 1; i <= CONFIG_NUMBER_SCHEDULES; i++) {
		sprintf(TopicBuff, MQTT_SCHEDULE_WORKDAY_SET, Config.BoardId(), i);
		Subscribe(TopicBuff);
		sprintf(TopicBuff, MQTT_SCHEDULE_WEEKEND_SET, Config.BoardId(), i);
		Subscribe(TopicBuff);
		RepeatedLoop(2);
	}
}
*/
bool Mqtt::Publish(const char* topic, const char* payload) {
	Config.Log->append(F("Publish [")).append(topic).append(F("]:")).append(payload).Debug();
	if (connected()) {
		return publish(topic, payload);
	}
	else {
		return false;
	}
}

void Mqtt::Subscribe(const char* topic) {
	Config.Log->append(F("Subscription:")).append(topic).Debug();
	if (connected()) {
		subscribe(topic);
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
	for (n *= 2; n >= 0; n--) {
		simpleLoop();
		delay(100);
	}
}

