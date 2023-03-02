#pragma once
#include "definitions.h"

//EEPROM STRUCTURE

//addr   |len|value
//--------------
// 0		|1  |BoardId

#define EEPROM_ADDR_ID	0  //1 bytes

// 1-4	|4	|IP address
#define EEPROM_ADDR_IP	(EEPROM_ADDR_ID+1)  //4 bytes

// 5-8	|4	|MQTT IP
// 9-10	|2	|MQTT Port

#define EEPROM_ADDR_MQTT	(EEPROM_ADDR_IP + 4)  //4+2=6 bytes


//Modes
//11 - 109 
// 11		|1	|Mode (0-manual, 1-schedule)
// 12		|1	|Manual temp
// 13		|1	|Heat/Cold
// 14		|1	|Hysteresis
// 15		|1	|Weekmode  5-2/6-1/7-0
// 16		|1	|CMD Stop/Run/...

#define EEPROM_ADDR_CONFIG	(EEPROM_ADDR_MQTT + 6)
#define EEPROM_ADDR_CONFIG_MODE EEPROM_ADDR_CONFIG
#define EEPROM_ADDR_CONFIG_MANUALTEMP EEPROM_ADDR_CONFIG+1
#define EEPROM_ADDR_CONFIG_HEATCOLD EEPROM_ADDR_CONFIG+2
#define EEPROM_ADDR_CONFIG_HYSTERESIS EEPROM_ADDR_CONFIG+3
#define EEPROM_ADDR_CONFIG_WEEKMODE EEPROM_ADDR_CONFIG+4
#define EEPROM_ADDR_CONFIG_CMD EEPROM_ADDR_CONFIG+5

#define EEPROM_ADDR_CONFIG_END EEPROM_ADDR_CONFIG_CMD


 //Days schedule
 //Workdays 
 //0		|1	| hour
 //1		|1	| minute		
 //2		|1	| temp
//Weekends   128 + 3 * 6 : 146
//0 | 1 | hour
//1 | 1 | minute
//2 | 1 | temp

#define EEPROM_ADDR_SCHED	EEPROM_ADDR_CONFIG_END+1
#define EEPROM_NUMB_SCHED	CONFIG_NUMBER_SCHEDULES * 2
#define EEPROM_LEN_SCHED	3

// 1-wire bus pin

#define EEPROM_ADDR_TBUS_PIN	(EEPROM_ADDR_SCHED + EEPROM_NUMB_SCHED * EEPROM_LEN_SCHED)


// Termometer settings
// 11 + 8*12 - 107 
// 0		|8	|T1 Address
// 8		|2	|MinTemp
// 10		|2	|MaxTemp

#define EEPROM_ADDR_THERM	(EEPROM_ADDR_TBUS_PIN + 1)
#define EEPROM_LEN_THERM	16
#define EEPROM_NUMB_THERM	12


// Relay Pins Settings
// Pin + lhOn
#define EEPROM_ADDR_RELAY	(EEPROM_ADDR_THERM + EEPROM_NUMB_THERM * EEPROM_LEN_THERM)
#define EEPROM_NUMB_RELAY	6
#define EEPROM_LEN_RELAY	2

// Contactor Pins Settings
// Pin + lhOn
#define EEPROM_ADDR_CONTACTOR	(EEPROM_ADDR_RELAY + EEPROM_NUMB_RELAY * EEPROM_LEN_RELAY)
#define EEPROM_NUMB_CONTACTOR	2
#define EEPROM_LEN_CONTACTOR	2



// Power Meter Pins Settings
// 
#define EEPROM_ADDR_POWERMETER	(EEPROM_ADDR_CONTACTOR + EEPROM_NUMB_CONTACTOR * EEPROM_LEN_CONTACTOR)
#define EEPROM_LEN_POWERMETER	4

// TimeZone Settings MUST BE THE LAST!!!!!
// 
#define EEPROM_ADDR_TIMEZONE	(EEPROM_ADDR_POWERMETER + EEPROM_LEN_POWERMETER)
// 0| Length of timezone
// 1-...| Timezone