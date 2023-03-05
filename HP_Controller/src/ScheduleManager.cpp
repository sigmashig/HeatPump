#include "ScheduleManager.h"
#include "SigmaEEPROM.h"
#include "Configuration.h"

extern Configuration Config;

ScheduleManager::ScheduleManager() {
	
	readFromEEPROM();
}

void ScheduleManager::readFromEEPROM() {
	for (int i = 0; i < 2 * CONFIG_NUMBER_SCHEDULES; i++) {
		byte h = SigmaEEPROM::Read8(EEPROM_ADDR_SCHED + EEPROM_LEN_SCHED * i);
		byte m = SigmaEEPROM::Read8(EEPROM_ADDR_SCHED + EEPROM_LEN_SCHED * i + 1);
		byte t = SigmaEEPROM::Read8(EEPROM_ADDR_SCHED + EEPROM_LEN_SCHED * i + 2);
		AllSchedule[i].UpdateSchedule(h, m, ((double)t) / 2.0);
	}
}

void ScheduleManager::Init()
{
}

void ScheduleManager::FinalInit()
{
}


void ScheduleManager::SubscribeSchedules()
{
	for (int i = 1; i <= 2 * CONFIG_NUMBER_SCHEDULES; i++) {
		Config.SubscribeSchedule(i);
	}
	Config.Transfer(2 * CONFIG_NUMBER_SCHEDULES);
}


void ScheduleManager::UpdateSchedule(byte shift, byte setNumber, const char* payload)
{ 
	AllSchedule[shift + setNumber].UpdateSchedule(payload);
	//if (sortSchedule(shift)) {
	//	publishSchedules(shift);
	//}
}

double ScheduleManager::GetDesiredTemperature() {
	TimeElements tm =  Config.Clock->GetClock();
	byte shift = 0;
	
	if (Config.GetWeekMode() == WEEKMODE_7_0) {
		shift = 0;
	} else if (Config.GetWeekMode() == WEEKMODE_5_2) {
		if (tm.Wday == 1 || tm.Wday == 7) {
			shift = CONFIG_NUMBER_SCHEDULES;
		} else {
			shift = 0;
		}
	} else if (Config.GetWeekMode() == WEEKMODE_6_1) {
		if (tm.Wday == 1) {
			shift = CONFIG_NUMBER_SCHEDULES;
		} else {
			shift = 0;
		}
	}

	for (int i = 0; i < CONFIG_NUMBER_SCHEDULES; i++) {
		if (AllSchedule[i + shift].CompareTime(tm.Hour, tm.Minute) <= 0
			&& (AllSchedule[i + shift + 1].CompareTime(tm.Hour, tm.Minute) > 0 || i == CONFIG_NUMBER_SCHEDULES - 1)) {
			return AllSchedule[i + shift].GetTemperature();
		}
	}
	
	return 0.0;
}

bool ScheduleManager::sortSchedule(byte shift) {
	bool res = false;
	
	for (int i = 0; i < CONFIG_NUMBER_SCHEDULES; i++) {
		for (int j = i + 1; j < CONFIG_NUMBER_SCHEDULES; j++) {
			if (AllSchedule[i + shift].CompareTime(AllSchedule[j + shift].GetHour(), AllSchedule[j + shift].GetMinute()) > 0) {
				Schedule temp = AllSchedule[i + shift];
				AllSchedule[i + shift] = AllSchedule[j + shift];
				AllSchedule[j + shift] = temp;
				res = true;
			}
		}
	}
	return res;
}

void ScheduleManager::publishSchedules(byte shift) {
	for (int i = 1; i <= CONFIG_NUMBER_SCHEDULES; i++) {
		Config.PublishSchedule(i + shift);
	}
	Config.Transfer(CONFIG_NUMBER_SCHEDULES);
}
