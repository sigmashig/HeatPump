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
	/*
	for (int i = 0; i < CONFIG_NUMBER_SCHEDULES; i++) {
		AllSchedule[i] = workDays[i];
		AllSchedule[i + CONFIG_NUMBER_SCHEDULES] = weekEnd[i];
	}
*/
}

void ScheduleManager::FinalInit()
{
	/*
	Config.Log->Debug("Before sort:");
	for (int i = 0; i < 2 * CONFIG_NUMBER_SCHEDULES; i++) {
		Config.Log->append(F("Schedule ")).append(i).append(F(": time:"))
			.append(AllSchedule[i].GetHour()).append(":").append(AllSchedule[i].GetMinute()).append(" t=").append(AllSchedule[i].GetTemperature()).Debug();
	}
	sortSchedule(0);
	sortSchedule(CONFIG_NUMBER_SCHEDULES);
	sort = true;
	Config.Log->Debug("After sort:");
	for (int i = 0; i < 2 * CONFIG_NUMBER_SCHEDULES; i++) {
		Config.Log->append(F("Schedule ")).append(i).append(F(": time:"))
			.append(AllSchedule[i].GetHour()).append(":").append(AllSchedule[i].GetMinute()).append(" t=").append(AllSchedule[i].GetTemperature()).Debug();
	}
	*/
}


void ScheduleManager::SubscribeSchedules()
{
	for (int i = 0; i < 2 * CONFIG_NUMBER_SCHEDULES; i++) {
		Config.SubscribeSchedule(i);
	}
	Config.Transfer(2 * CONFIG_NUMBER_SCHEDULES);
}


void ScheduleManager::UpdateSchedule(byte shift, byte setNumber, const char* payload)
{
	AllSchedule[shift + setNumber].UpdateSchedule(payload);
	if (sort) {
		sortSchedule(shift);
	}
}

byte ScheduleManager::wdOrWe(_WEEK_DAYS_ day) {
	byte shift = 0;


	if (Config.GetWeekMode() == WEEKMODE_7_0) {
		shift = 0;
	} else if (Config.GetWeekMode() == WEEKMODE_5_2) {
		if (day == SATURDAY || day == SUNDAY) {
			shift = CONFIG_NUMBER_SCHEDULES;
		} else {
			shift = 0;
		}
	} else if (Config.GetWeekMode() == WEEKMODE_6_1) {
		if (day == SUNDAY) {
			shift = CONFIG_NUMBER_SCHEDULES;
		} else {
			shift = 0;
		}
	}

	return shift;
}



double ScheduleManager::GetDesiredTemperature() {
	tm tm0 = Config.GetTime();
	//Config.Log->append(F("GetDesiredTemperature: ")).append(dt.hour).append(":").append(dt.minute).append(" day=").append(dt.day).Debug();
	return GetDesiredTemperature(tm0.tm_hour,tm0.tm_min,tm0.tm_wday);
}

double ScheduleManager::GetDesiredTemperature(byte h, byte m, byte day) {

	byte shift = wdOrWe((_WEEK_DAYS_)day);
	Config.Log->append(F("GetDesiredTemperature: ")).append(h).append(":").append(m).append(" day=").append(day).append(" shift=").append(shift).Debug();
	if (AllSchedule[shift + 0].CompareTime(h, m) < 0) {
		// take last schedule for yesterday
		Config.Log->Debug("GetDesiredTemperature: yesterday");

		_WEEK_DAYS_ yDay = SigmaClock::DayYesterday((_WEEK_DAYS_)day);
		byte sh = wdOrWe(yDay);
		return AllSchedule[sh + CONFIG_NUMBER_SCHEDULES - 1].GetTemperature();
		//return GetDesiredTemperature(23,59, yDay);
	} else {
		for (int i = 0; i < CONFIG_NUMBER_SCHEDULES - 1; i++) {
			if (AllSchedule[i + shift].CompareTime(h, m) >= 0
				&& AllSchedule[i + shift + 1].CompareTime(h, m) < 0) {
				return AllSchedule[i + shift].GetTemperature();
			}
		}
		return AllSchedule[CONFIG_NUMBER_SCHEDULES - 1 + shift].GetTemperature();
	}
	return 0.0;
}

bool ScheduleManager::sortSchedule(byte shift) {
	bool res = false;
	
	for (int i = 0; i < CONFIG_NUMBER_SCHEDULES; i++) {
		for (int j = i + 1; j < CONFIG_NUMBER_SCHEDULES; j++) {
			if (AllSchedule[i + shift].CompareTime(AllSchedule[j + shift].GetHour(), AllSchedule[j + shift].GetMinute()) < 0) {
				Schedule temp = AllSchedule[i + shift];
				AllSchedule[i + shift] = AllSchedule[j + shift];
				AllSchedule[j + shift] = temp;
				res = true;
			}
		}
	}
	return res;
}
/*
void ScheduleManager::publishSchedules(byte shift) {
	for (int i = 1; i <= CONFIG_NUMBER_SCHEDULES; i++) {
		Config.PublishSchedule(i + shift);
	}
	Config.Transfer(CONFIG_NUMBER_SCHEDULES);
}
*/
