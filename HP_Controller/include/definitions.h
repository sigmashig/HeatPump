#pragma once

#include <Arduino.h>

#define JSON_SIZE	200

#define CONFIG_NUMBER_SCHEDULES		6
#define CONFIG_NUMBER_RELAYS		6
#define CONFIG_NUMBER_CONTACTORS	2
#define CONFIG_NUMBER_THERMO		12

#define CONFIG_NUMBER_OF_DEVICES	(CONFIG_NUMBER_RELAYS + CONFIG_NUMBER_CONTACTORS + CONFIG_NUMBER_THERMO + 1) 	
// #define CONFIG_NUMBER_OF_PARAMS		6
#define CONFIG_NUMBER_OF_SCHEDULED_PARAMS		(CONFIG_NUMBER_SCHEDULES*2)

typedef enum {
	CMD_NOCMD = 0,
	CMD_RUN = 1,
	CMD_STOP = 2
} CMD;

typedef enum {
	ALERT_EMPTY = ' ',
	ALERT_TEMP_FLOOR = 'f',
	ALERT_STEP_TOO_LONG = 'S',
	ALERT_TEMP_IS_OUT_OF_RANGE = 'T'
} ALERTCODE;



typedef enum {
	D_OFF = 0,
	D_FATAL = 1,
	D_ERROR = 2,
	D_WARN = 3,
	D_INFO = 4,
	D_DEBUG = 5,
	D_ALL = 6
} DebugLevel;

typedef enum {
	NOTHING = 0,
	BUTTON = 'B',
	RELAY = 'R',
	ONE_WIRE_BUS = '1',
	ONE_WIRE_THERMO = 'T',
	POWER_METER = 'P',
	VIRTUAL_BUTTON = 'b',
	CONTACTOR = 'C',
	SHIFTOUT = 'S',
	SHIFTIN = 's'
} UnitType;

typedef enum {
	ACT_OFF = 0, //Off for any device
	ACT_ON = 1, //On for any device
	ACT_LONG = 2, //Long (for button)
	ACT_SWITCH = 3, //Switch device
	ACT_EXTRA_LONG = 4, //Button. Extra long
	ACT_SHORT_LONG = 5, //Button. Is pressed not released yet. And pressed time less than long
	ACT_RELAY_DELAY_OFF = 6, //Relay off with delay
	ACT_RELAY_DELAY_ON = 7, //Relay on with delay
	ACT_SENSOR_READY = 10 //Sensor ready
} ActionType;

typedef enum {
	MODE_MANUAL = 0,
	MODE_SCHEDULE = 1
} MODE;

typedef enum {
	MODE_WEEK_5_2 = 0,
	MODE_WEEK_6_1 = 1,
	MODE_WEEK_7_0 = 2
} WEEKMODE;


typedef enum {
	MODE_HEAT = 0,
	MODE_COLD = 1
} HEATMODE;

//extern DebugLevel DLevel;

