#pragma once

#include <IPAddress.h>

class Utils
{
public:
    static void Double2Str(char* str, double d, int precision = 1);
    static double Str2Double(const char* str);
    //static void Ip2Str(char* str, IPAddress ip);
    static bool IsIpValid(IPAddress ip);
    static char* PrintIp(IPAddress ip);
    static char* PrintMac(uint8_t* mac);
    static constexpr void(*Reset) (void) = 0;
};

