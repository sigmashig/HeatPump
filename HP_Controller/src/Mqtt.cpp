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

/*
void Mqtt::MqttLoop(int n) {

	static long lastConnected = 0;

	if (connected()) {
		bool res = loop();
		if (!res) {
			Config.Log->Error(F("Failed loop"));
		}

		lastConnected = millis();
		if (lenCB != 0) {
			callback();
			lenCB = 0;
		}
	}
	else {
		if (lastConnected + millis() <= MQTT_RETRY_TIME) {
			Config.Log->Info(F("Trying to reconnect MQTT"));
			mqttReconnect();
		}
	}
}
*/
/*
void Mqtt::watchDog2() {
	sprintf(TopicBuff, MQTT_WATCH_DOG2, Config.BoardId());
	Publish(TopicBuff, "0");
}
*/
/*
void Mqtt::callback() {

	if (lenCB > 0) {
//		Config.Log->append("LENCB=").append(lenCB).Debug();
		Config.Log->append("[").append(topicCB).append("]:").append(payloadCB).Info();

		sprintf(TopicBuff, MQTT_CONFIG, Config.BoardId());
		if (strncmp(topicCB, TopicBuff, strlen(TopicBuff)) ==0 ) { //Config
			updateConfig(topicCB, payloadCB);			
		}
		else {
			sprintf(TopicBuff, MQTT_STATUS, Config.BoardId());
			if (strncmp(topicCB, TopicBuff, strlen(TopicBuff)) ==0 ) { //Status
				updateStatus(topicCB, payloadCB);
			}
			else {
				
				sprintf(TopicBuff, MQTT_SCHEDULE, Config.BoardId());
				if (strncmp(topicCB, TopicBuff, strlen(TopicBuff)) == 0) { //Schedule
					updateSchedule(topicCB, payloadCB);
				}
				else {
					sprintf(TopicBuff, MQTT_EQUIPMENT, Config.BoardId());
					if (strncmp(topicCB, TopicBuff, strlen(TopicBuff)) == 0) { //Equipment
						updateEquipment(topicCB, payloadCB);
					}
					else {
						sprintf(TopicBuff, MQTT_ALERT, Config.BoardId());
						if (strncmp(topicCB, TopicBuff, strlen(TopicBuff)) == 0) { //Alert
							updateAlert(topicCB, payloadCB);
						}
					}
				}
			}
		}
	}
}
*/

/*
void Mqtt::subscribeEquipment() {
	int n = 0;
	for (int i = 0; i < CONFIG_NUMBER_RELAYS; i++) {
		sprintf(TopicBuff, MQTT_EQUIPMENT_RELAY, Config.BoardId(), Config.DevMgr->AllRelays[i]->Name);
		Subscribe(TopicBuff);
		n++;
	}
	sprintf(TopicBuff, MQTT_EQUIPMENT_CONTACTOR, Config.BoardId(), Config.DevMgr->PressureSwitch.Name);
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_EQUIPMENT_CONTACTOR, Config.BoardId(), Config.DevMgr->VoltageSwitch.Name);
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_EQUIPMENT_THERMOMETER, Config.BoardId(), Config.DevMgr->Bus.Name);
	Subscribe(TopicBuff);
	n++;
	for (int i = 0; i < CONFIG_NUMBER_THERMO; i++) {
		sprintf(TopicBuff, MQTT_EQUIPMENT_THERMOMETER, Config.BoardId(), Config.DevMgr->AllThermo[i]->Name);
		Subscribe(TopicBuff);
		n++;
	}
	RepeatedLoop(n);
}
*/

/*
void Mqtt::subscribeStatuses() {

	int n = 0;
	for (int i = 0; i < CONFIG_NUMBER_RELAYS; i++) {
		sprintf(TopicBuff, MQTT_STATUS_RELAY, Config.BoardId(), Config.DevMgr->AllRelays[i]->Name);
		Subscribe(TopicBuff);
		n++;
	}
	sprintf(TopicBuff, MQTT_STATUS_CONTACTOR, Config.BoardId(), Config.DevMgr->PressureSwitch.Name);
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_STATUS_CONTACTOR, Config.BoardId(), Config.DevMgr->VoltageSwitch.Name);
	Subscribe(TopicBuff);
	n++;
	sprintf(TopicBuff, MQTT_STATUS_THERMOMETER, Config.BoardId(), Config.DevMgr->Bus.Name);
	Subscribe(TopicBuff);
	n++;
	for (int i = 0; i < CONFIG_NUMBER_THERMO; i++) {
		sprintf(TopicBuff, MQTT_STATUS_THERMOMETER, Config.BoardId(), Config.DevMgr->AllThermo[i]->Name);
		Subscribe(TopicBuff);
		n++;
	}

	RepeatedLoop(n);
}
*/

/*
void Mqtt::updateEquipment(const char* topic, const char* payload) {

	int len;
	sprintf(TopicBuff, MQTT_EQUIPMENT_RELAYS, Config.BoardId());
	len = strlen(TopicBuff);
	if (strncmp(TopicBuff, topic, len) == 0) {
		strncpy(TopicBuff, topic + len + 1, strlen(topic));
		Config.DevMgr->UpdateRelayEquipment(TopicBuff, payload);
	}
	else {
		sprintf(TopicBuff, MQTT_EQUIPMENT_CONTACTORS, Config.BoardId());
		len = strlen(TopicBuff);
		if (strncmp(TopicBuff, topic, len) == 0) {
			strncpy(TopicBuff, topic + len + 1, strlen(topic));
			Config.DevMgr->UpdateContactorEquipment(TopicBuff, payload);
		}
		else {
			sprintf(TopicBuff, MQTT_EQUIPMENT_THERMOMETERS, Config.BoardId());
			len = strlen(TopicBuff);
			if (strncmp(TopicBuff, topic, len) == 0) {
				strncpy(TopicBuff, topic + len + 1, strlen(topic));
				Config.DevMgr->UpdateThermoEquipment(TopicBuff, payload);
			}
		}
	}
}
*/
/*
void Mqtt::updateStatus(const char* topic, const char* payload)
{
	int len;
	sprintf(TopicBuff, MQTT_STATUS_RELAYS, Config.BoardId());
	len = strlen(TopicBuff);
	if (strncmp(TopicBuff, topic, len) == 0) {
		strncpy(TopicBuff, topic + len + 1, strlen(topic));
		Config.DevMgr->UpdateRelayStatus(TopicBuff, payload);
	}
	else {
		sprintf(TopicBuff, MQTT_STATUS_CONTACTORS, Config.BoardId());
		len = strlen(TopicBuff);
		if (strncmp(TopicBuff, topic, len) == 0) {
			strncpy(TopicBuff, topic + len + 1, strlen(topic));
			Config.DevMgr->UpdateContactorStatus(TopicBuff, payload);
		}
		else {
			sprintf(TopicBuff, MQTT_STATUS_THERMOMETERS, Config.BoardId());
			len = strlen(TopicBuff);
			if (strncmp(TopicBuff, topic, len) == 0) {
				strncpy(TopicBuff, topic + len + 1, strlen(topic));
				Config.DevMgr->UpdateThermoStatus(TopicBuff, payload);
			}
		}

	}
}
*/

/*
void Mqtt::updateConfig(const char* topic, const char* payload) {

	sprintf(TopicBuff, MQTT_MODE, Config.BoardId());
	if (strcmp(topic, TopicBuff) == 0) {
		Config.SetMode(payload);
	}
	else {
		sprintf(TopicBuff, MQTT_WEEKMODE, Config.BoardId());
		if (strcmp(topic, TopicBuff) == 0) {
			Config.SetWeekMode(payload);
		}
		else {
			sprintf(TopicBuff, MQTT_MANUAL_TEMP, Config.BoardId());
			if (strcmp(topic, TopicBuff) == 0) {
				Config.SetManualTemp(payload);
			}
			else {
				sprintf(TopicBuff, MQTT_HEAT_COLD, Config.BoardId());
				if (strcmp(topic, TopicBuff) == 0) {
					Config.SetHeatMode(payload);
				}
				else {
					sprintf(TopicBuff, MQTT_HYSTERESIS, Config.BoardId());
					if (strcmp(topic, TopicBuff) == 0) {
						Config.SetHysteresis(payload);
					}
					else {
						sprintf(TopicBuff, MQTT_TIMEZONE, Config.BoardId());
						if (strcmp(topic, TopicBuff) == 0) {
							Config.Clock->SetTimezone(payload);
						}
						else {
							sprintf(TopicBuff, MQTT_SIMULATOR, Config.BoardId());
							if (strcmp(topic, TopicBuff) == 0) {
								Config.SetSimulator(payload);
							}
							else {
								sprintf(TopicBuff, MQTT_COMMAND, Config.BoardId());
								if (strcmp(topic, TopicBuff) == 0) {
									Config.SetCommand(payload);
								}
							}
						}
					}
				}
			}
		}
	}
}
*/

/*
void Mqtt::updateSchedule(const char* topic, const char* payload)
{
	sprintf(TopicBuff, MQTT_SCHEDULE_WORKDAY, Config.BoardId());
	if (strncmp(topic, TopicBuff, strlen(TopicBuff)) == 0) {
		for (int i = 1; i <= CONFIG_NUMBER_SCHEDULES; i++) {
			sprintf(TopicBuff, MQTT_SCHEDULE_WORKDAY_SET, Config.BoardId(), i);
			if (strcmp(topic, TopicBuff) == 0) {
				Config.ScheduleMgr->Workdays[i].UpdateSchedule(payload);
			}
		}
	}
	else {
		sprintf(TopicBuff, MQTT_SCHEDULE_WEEKEND, Config.BoardId());
		if (strncmp(topic, TopicBuff, strlen(TopicBuff)) == 0) {
			for (int i = 1; i <= CONFIG_NUMBER_SCHEDULES; i++) {
				sprintf(TopicBuff, MQTT_SCHEDULE_WEEKEND_SET, Config.BoardId(), i);
				if (strcmp(topic, TopicBuff) == 0) {
					Config.ScheduleMgr->Weekdays[i].UpdateSchedule(payload);
				}
			}
		}
	}
}
*/
/*
void Mqtt::PublishConfig() {

	sprintf(TopicBuff, MQTT_MODE, Config.BoardId());
	PayloadBuff[0] = (byte)Config.GetMode() + '0';
	PayloadBuff[1] = 0;
	publish(TopicBuff, PayloadBuff);

	sprintf(TopicBuff, MQTT_WEEKMODE, Config.BoardId());
	PayloadBuff[0] = (byte)Config.GetWeekMode() + '0';
	PayloadBuff[1] = 0;
	publish(TopicBuff, PayloadBuff);

	sprintf(TopicBuff, MQTT_MANUAL_TEMP, Config.BoardId());
	sprintf(PayloadBuff, "%f", Config.GetManualTemp());
	publish(TopicBuff, PayloadBuff);

	sprintf(TopicBuff, MQTT_HEAT_COLD, Config.BoardId());
	PayloadBuff[0] = (byte)Config.GetHeatMode() + '0';
	PayloadBuff[1] = 0;
	publish(TopicBuff, PayloadBuff);

	sprintf(TopicBuff, MQTT_HYSTERESIS, Config.BoardId());
	PayloadBuff[0] = Config.GetHysteresis() + '0';
	PayloadBuff[1] = 0;
	publish(TopicBuff, PayloadBuff);

	sprintf(TopicBuff, MQTT_HYSTERESIS, Config.BoardId());
	publish(TopicBuff, Config.GetTimezone());

	sprintf(TopicBuff, MQTT_SIMULATOR, Config.BoardId());
	PayloadBuff[0] = (byte)Config.GetSimulator() + '0';
	PayloadBuff[1] = 0;
	publish(TopicBuff, PayloadBuff);

	sprintf(TopicBuff, MQTT_COMMAND, Config.BoardId());
	PayloadBuff[0] = (byte)Config.GetCommand() + '0';
	PayloadBuff[1] = 0;
	publish(TopicBuff, PayloadBuff);

}
*/

/*
void Mqtt::updateAlert(const char* topic, const char* payload) {
	// do nothing
}
*/