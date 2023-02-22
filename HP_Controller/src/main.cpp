/*
 Name:		HPController.ino
 Created:	26.12.21 13:29:05
 Author:	Igor Shevchenko
*/

// the setup function runs once when you press reset or power the board
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
#include <SerialComs.h>
#include <SafeStringStream.h>
#include <SafeStringReader.h>
#include <SafeStringNameSpaceStart.h>
#include <SafeStringNameSpaceEnd.h>
#include <SafeStringNameSpace.h>
#include <SafeString.h>
#include <millisDelay.h>
#include <loopTimer.h>
#include <BufferedOutput.h>
#include <BufferedInput.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#include "definitions.h"
#include "Configuration.h"

Configuration Config;
DebugLevel DLevel = DebugLevel::D_DEBUG;

void setup() {
	Serial.begin(115200);
	while (!Serial) {
		delay(10);
	}
	SafeString::setOutput(Serial);
	//init random generator
	randomSeed(analogRead(0));

	//initialization of config
	Config.Init();
	Config.Log->Info(F("Board is ready"));
	Config.Log->append(F("Board Id#:")).append(Config.BoardId()).Info();
	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);
	Config.Log->Info("Enjoy!");
}

// the loop function runs over and over again until power down or reset
void loop() {
	/*
	static unsigned long tp60 = 0;
	static unsigned long tp30 = 0;
	static unsigned long tp10 = 0;
	static unsigned long tp1 = 0;
	unsigned long now = millis();
	if ((now - tp60) > (unsigned long)60 * 1000) { //1 min 
		//MEMFREE("StartLoop1m");
		Config.Loop(60000);
		Config.Counter60++;
		tp60 = now;
	}

	if ((now - tp30) > (unsigned long)30 * 1000) { //30 sec 
		Config.Loop(30000);
		Config.Counter30++;
		tp30 = now;
	}
	if ((now - tp10) > (unsigned long)10 * 1000) { //10 sec 
		Config.Loop(10000);
		Config.Counter10++;
		tp10 = now;
	}
	if ((now - tp1) > (unsigned long)1 * 1000) { //1 sec 
		Config.Loop(1000);
		Config.Counter1++;
		tp1 = now;
	}

	Config.Loop(0);
	*/
}
