#include <Arduino.h>
#include <EEPROM.h>
//#include <IPAddress.h>
#include "EEPROM_Structure.h"
/*
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
*/

void EepromWrite(unsigned int addr, byte value[8]) {

	for (int i = 0; i < 8; i++) {
		EEPROM.write(addr + i, value[i]);
	}
}


void EepromWrite(unsigned int addr, byte value) {

	EEPROM.write(addr, value);
}

void EepromWrite(unsigned int addr, unsigned int value) {

	EEPROM.write(addr + 1, value & 0xFF);
	EEPROM.write(addr, (value >> 8) & 0xFF);
}

byte EepromRead(unsigned int addr) {
	return EEPROM.read(addr);
}

unsigned int EepromRead2(unsigned int addr) {
	byte x;
	byte b;

	x = EEPROM.read(addr);
	b = EEPROM.read(addr + 1);
	return (unsigned)b << 8 | x;
}

void WriteTimezone(const char* tz)
{
	int len = strlen(tz);
	EepromWrite(EEPROM_ADDR_TIMEZONE, (byte)len);

	for (int i = 0; i < len; i++) {
		EepromWrite(EEPROM_ADDR_TIMEZONE + 1 + i, (byte)(tz[i]));
	}
}



void setup() {

	Serial.begin(115200);
	Serial.println("Start!");

	// Board ID
	EepromWrite(EEPROM_ADDR_ID, (byte)0x07);

	EepromWrite(EEPROM_ADDR_IP, (byte)192);
	EepromWrite(EEPROM_ADDR_IP+1, (byte)168);
	EepromWrite(EEPROM_ADDR_IP+2, (byte)0);
	EepromWrite(EEPROM_ADDR_IP+3, (byte)(90+7));

	EepromWrite(EEPROM_ADDR_MQTT, (byte)192);
	EepromWrite(EEPROM_ADDR_MQTT + 1, (byte)168);
	EepromWrite(EEPROM_ADDR_MQTT + 2, (byte)0);
	EepromWrite(EEPROM_ADDR_MQTT + 3, (byte)(98));
	
	EepromWrite(EEPROM_ADDR_MQTT + 4, (unsigned int)1883);
/*
	EepromWrite(EEPROM_ADDR_MODES, (byte)0x00); //manual
	EepromWrite(EEPROM_ADDR_MODES + 1, (byte)25); //desired temp
	EepromWrite(EEPROM_ADDR_MODES + 2, (byte)0x00); //5-2

	for (int i = 0; i < EEPROM_NUMB_SCHED; i++) {
		EepromWrite(EEPROM_ADDR_SCHED + 0 + i * EEPROM_LEN_SCHED, (byte)0);
		EepromWrite(EEPROM_ADDR_SCHED + 1 + i * EEPROM_LEN_SCHED, (byte)0);
		EepromWrite(EEPROM_ADDR_SCHED + 2 + i * EEPROM_LEN_SCHED, (byte)25);
	}
	*/

	//Thermometer Addresses
/*
	byte tAddr[7][8] = {
		{0x28, 0xFF, 0xDA,0x2F,0xB4,0x16,0x05,0x1A}, //28FFDA2FB416051A = 0
		{0x28,0xFF,0xB8,0x04,0xB4,0x16,0x05,0x6C},	//28FFB804B416056C = 1
		{0x28,0xFF,0xBC,0xA6,0x87,0x16,0x03,0x3A},	//28FFBCA68716033A = 2
		{0x28,0xFF,0x9C,0x95,0x87,0x16,0x03,0x02},	//28FF9C9587160302 = 3
		{0x28,0xFF,0xF7,0xF3,0xB3,0x16,0x05,0x67},	//28FFF7F3B3160567 = 4
		{0x28,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
		{0x28,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}
	};
*/
	
	byte tAddr[EEPROM_NUMB_THERM][EEPROM_LEN_THERM] = {
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
	};

	for (int i = 0; i < EEPROM_NUMB_THERM; i++) {
		EepromWrite(EEPROM_ADDR_THERM + i * EEPROM_LEN_THERM, tAddr[i]);
	}

	// Relay Pins Settings
	//

	EepromWrite(EEPROM_ADDR_RELAY + 0, (byte)14);
	EepromWrite(EEPROM_ADDR_RELAY + 1, (byte)15);
	EepromWrite(EEPROM_ADDR_RELAY + 2, (byte)16);
	EepromWrite(EEPROM_ADDR_RELAY + 3, (byte)17);
	EepromWrite(EEPROM_ADDR_RELAY + 4, (byte)18);
	EepromWrite(EEPROM_ADDR_RELAY + 5, (byte)19);

// Contactor Pins Settings
// 
	EepromWrite(EEPROM_ADDR_CONTACTOR + 0, (byte)20);
	EepromWrite(EEPROM_ADDR_CONTACTOR + 1, (byte)21);



// Power Meter Pins Settings
// 
	EepromWrite(EEPROM_ADDR_POWERMETER + 0, (byte)24);
	EepromWrite(EEPROM_ADDR_POWERMETER + 1, (byte)25);
	EepromWrite(EEPROM_ADDR_POWERMETER + 2, (byte)26);
	EepromWrite(EEPROM_ADDR_POWERMETER + 3, (byte)27);

	
	WriteTimezone("Europe/Kiev");


	Serial.println("Done!");
}

// the loop function runs over and over again until power down or reset
void loop() {

}
