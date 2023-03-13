#include "SigmaEEPROM.h"

#include "EEPROM.h"

#include "Configuration.h"

extern Configuration Config;

uint16_t SigmaEEPROM::Read16(uint16_t addr) {
	uint16_t res;
	res = (((uint16_t)EEPROM.read(addr)) << 8 & 0xFF00) + EEPROM.read(addr + 1);
	return res;
}

byte SigmaEEPROM::Read8(uint16_t addr) {
	return EEPROM.read(addr);
}


void SigmaEEPROM::Write16(uint16_t addr, uint16_t val) {
	EEPROM.write(addr, (byte)((val >> 8) & 0x00FF));
	EEPROM.write(addr + 1, (byte)((val & 0x00FF)));

}

char* SigmaEEPROM::ReadTimezone(char* tz)
{
	int len = Read8(EEPROM_ADDR_TIMEZONE);
	if (len >= TIMEZONE_LEN) {
		strcpy(tz, "XXX");
	}
	else {
		for (int i = 0; i < len; i++) {
			tz[i] = Read8(EEPROM_ADDR_TIMEZONE + 1 + i);
		}
		tz[len] = 0;
	}
	return tz;
}

void SigmaEEPROM::WriteTimezone(const char* tz)
{
	int len = strlen(tz);
	Write8(EEPROM_ADDR_TIMEZONE, len);

	for (int i = 0; i < len; i++) {
		Write8(EEPROM_ADDR_TIMEZONE + 1 + i, tz[i]);
	}
}

void SigmaEEPROM::Write8(uint16_t addr, byte val) {
	EEPROM.write(addr, val);
}

byte SigmaEEPROM::ReadBoardId() {
	return Read8(EEPROM_ADDR_ID);
}


void SigmaEEPROM::WriteIp(IPAddress& ip, uint16_t addr) {
	bool res = (ip[0] + ip[1] + ip[2] + ip[3]) != 0;
	if (res) {
		for (int i = 0; i < 4;i++) {
			Write8(addr + i, ip[i]);
		}
	}
}


void SigmaEEPROM::ReadIp(IPAddress& ip, uint16_t addr) {
	for (int i = 0; i < 4; i++) {
		ip[i] = Read8(addr + i);
	}
}