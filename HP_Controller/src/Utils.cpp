#include "Utils.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

void Utils::Double2Str(char* str, double d, int precision) {

    unsigned long factor = pow(10, precision);
    long p1 = (long)d;
    unsigned long p2 = (unsigned long)(((double)(d - p1)) * factor);
    sprintf(str, "%ld.%*lu", p1, precision, p2);
}

double Utils::Str2Double(const char* str)
{
    double d=0.0;
    int sign = 1;
    bool isFraction = false;
    bool isDigit = false;
    bool isSign = false;
    double fract = 0.1;

    for (unsigned i = 0; i < strlen(str); i++) {
        char c = str[i];

        if (c >= '0' && c <= '9') {
            isDigit = true;
            if (!isFraction) {
                d = d * 10.0 + (c - '0');
            }
            else {
                d = d + fract * (c - '0');
                fract *= 0.1;
            }
            d *= sign;
            sign = 1;
        }
        else if (c == '+' || c == '-') {
            if (isDigit || isFraction || isSign) { //error
                return d;
            }
            else {
                isSign = true;
                if (c == '-') {
                    sign = -1;
                }
            }
        }
        else if (c == '.') {
            if (isFraction) { // error
                return d;
            }
            else {
                isFraction = true;
            }
        }
        else {
            // just ignore any symbols 
        }
    }
    return d;
}

bool Utils::IsIpValid(IPAddress ip) {
    bool res = true;
    res = (ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0);
    return res;
}
