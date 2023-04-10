#pragma once

#include "Unit.h"
#include "SigmaClock.hpp"
#include <Arduino.h>

class RTC: public Unit {
public:
    RTC(const char* nm);
    void InitUnit();
    void UnitLoop(unsigned long timePeriod);
    bool IsSimulator() { return false; }
    bool UpdateEquipment(const char* line);
    void UpdateStatus(const char* payload) {};
    bool IsOk(){ return true;};
    void const print(const char* header, DebugLevel level) {};
    bool ProcessUnit(ActionType event){ return true;};
    //void SetTime(time_t t, int tz = 2 * ONE_HOUR);
    tm GetTime();
    void PublishDefaultEquipment();
    //void SetTime(tm& t);
    bool IsConnected(void) { return true; }; // there is no possibility to check if the RTC is connected

    DS1302_Pins GetPins() { return pins; };
    void SetPins(DS1302_Pins p) { pins = p; };
    RTCType GetType() { return type; };
    void SetType(RTCType t) { type = t; };
    int GetTz() { return tz; };
    void SetTz(int t) { tz = t; };
private:
    DS1302_Pins pins;
    RTCType type;
    int tz;
};