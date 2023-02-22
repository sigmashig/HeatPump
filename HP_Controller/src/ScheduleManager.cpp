#include "ScheduleManager.h"
#include "SigmaEEPROM.h"
#include "Configuration.h"

extern Configuration Config;

ScheduleManager::ScheduleManager(){
	readFromEEPROM();
}

void ScheduleManager::readFromEEPROM() {
	for (int i = 0; i < CONFIG_NUMBER_SCHEDULES; i++) {
		byte h = SigmaEEPROM::Read8(EEPROM_ADDR_SCHED + EEPROM_LEN_SCHED * i);
		byte m = SigmaEEPROM::Read8(EEPROM_ADDR_SCHED + EEPROM_LEN_SCHED * i + 1);
		byte t = SigmaEEPROM::Read8(EEPROM_ADDR_SCHED + EEPROM_LEN_SCHED * i + 2);
		Workdays[i].UpdateSchedule(h, m, ((double)t) / 2.0);
	}
	for (int i = 0; i < CONFIG_NUMBER_SCHEDULES; i++) {
		byte h = SigmaEEPROM::Read8(EEPROM_ADDR_SCHED + EEPROM_LEN_SCHED * CONFIG_NUMBER_SCHEDULES + EEPROM_LEN_SCHED * i);
		byte m = SigmaEEPROM::Read8(EEPROM_ADDR_SCHED + EEPROM_LEN_SCHED * CONFIG_NUMBER_SCHEDULES + EEPROM_LEN_SCHED * i + 1);
		byte t = SigmaEEPROM::Read8(EEPROM_ADDR_SCHED + EEPROM_LEN_SCHED * CONFIG_NUMBER_SCHEDULES + EEPROM_LEN_SCHED * i + 2);
		Weekdays[i].UpdateSchedule(h, m, ((double)t) / 2.0);
	}

}

void ScheduleManager::Init()
{
}

void ScheduleManager::FinalInit()
{
	/*
	Config.Log->append("Scheduled Params:").append(numbInitSchedules).append(" of ").append(CONFIG_NUMBER_OF_SCHEDULED_PARAMS).append(" are ready").Info();
	int nTry = 0;
	while (nTry < 10 * CONFIG_NUMBER_OF_SCHEDULED_PARAMS && numbInitSchedules < CONFIG_NUMBER_OF_SCHEDULED_PARAMS) {
		nTry++;
		Config.MqttClient->MqttLoop();
	}
	Config.Log->append("Scheduled Params:").append(numbInitSchedules).append(" of ").append(CONFIG_NUMBER_OF_SCHEDULED_PARAMS).append(" are ready").Info();
	*/
}

/*
bool ScheduleManager::IsTempActionRequired()
{
	bool res = false;

	if (heatCold == MODE_HEAT) {
		if (GetDesiredTemperature() + GetHysteresis()/2.0 <= Config.DevMgr->TInside.GetTemperature()) {
			res = true;
		}
	}
	else
		if (GetDesiredTemperature() + GetHysteresis()/2.0 >= Config.DevMgr->TInside.GetTemperature()) {
			res = true;
		}
	return res;
}
*/

void ScheduleManager::SubscribeSchedules()
{
	for (int i = 1; i <= CONFIG_NUMBER_SCHEDULES; i++) {
		sprintf(Config.TopicBuff, MQTT_SCHEDULE_WORKDAY_SET, Config.BoardId(), i);
		Config.Subscribe(Config.TopicBuff);
		sprintf(Config.TopicBuff, MQTT_SCHEDULE_WEEKEND_SET, Config.BoardId(), i);
		Config.Subscribe(Config.TopicBuff);
		Config.Transfer(2);
	}
}


void ScheduleManager::UpdateSchedule(const char* topic, const char* payload)
{
	sprintf(Config.TopicBuff, MQTT_SCHEDULE_WORKDAY, Config.BoardId());
	if (strncmp(topic, Config.TopicBuff, strlen(Config.TopicBuff)) == 0) {
		for (int i = 1; i <= CONFIG_NUMBER_SCHEDULES; i++) {
			sprintf(Config.TopicBuff, MQTT_SCHEDULE_WORKDAY_SET, Config.BoardId(), i);
			if (strcmp(topic, Config.TopicBuff) == 0) {
				Workdays[i].UpdateSchedule(payload);
			}
		}
	}
	else {
		sprintf(Config.TopicBuff, MQTT_SCHEDULE_WEEKEND, Config.BoardId());
		if (strncmp(topic, Config.TopicBuff, strlen(Config.TopicBuff)) == 0) {
			for (int i = 1; i <= CONFIG_NUMBER_SCHEDULES; i++) {
				sprintf(Config.TopicBuff, MQTT_SCHEDULE_WEEKEND_SET, Config.BoardId(), i);
				if (strcmp(topic, Config.TopicBuff) == 0) {
					Weekdays[i].UpdateSchedule(payload);
				}
			}
		}
	}
}
