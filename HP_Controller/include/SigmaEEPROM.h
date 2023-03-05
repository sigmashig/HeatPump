#pragma once

#include <Arduino.h>

#include "IPAddress.h"

#include "EEPROM_Structure.h"

class SigmaEEPROM  
{

 public:
	static byte ReadBoardId();
	//static unsigned int ReadMqtt(IPAddress& ip);

    static void WriteIp(IPAddress& ip, uint16_t addr);
	static void ReadIp(IPAddress& ip, uint16_t addr);

	static uint16_t Read16(uint16_t addr);
	static byte Read8(uint16_t addr);
	static void Write8(uint16_t addr, byte val);
	static void Write16(uint16_t addr, uint16_t val);

	static char* ReadTimezone(char* tz);
	static void WriteTimezone(const char* tz);

private:

};



