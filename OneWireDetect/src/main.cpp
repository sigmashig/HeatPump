#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>


OneWire bus = OneWire(19);
DallasTemperature sensors(&bus);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  sensors.begin();
  sensors.getDeviceCount();
  Serial.print("Found:");
  Serial.println(sensors.getDeviceCount());


  for (int i = 0; i < sensors.getDeviceCount(); i++) {
    Serial.print("Device:");
    Serial.print(i);
    DeviceAddress addr;
    sensors.getAddress(addr, i);
    
    Serial.print(" Address:");
    char buf[30];
    sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6], addr[7]);
    Serial.println(buf);
    Serial.print(" Resolution:");
    Serial.println(sensors.getResolution(addr));
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  sensors.requestTemperatures();
  delay(2000);
  for (int i = 0; i < sensors.getDeviceCount(); i++) {
    DeviceAddress addr;
    sensors.getAddress(addr, i);
    Serial.print("Device:");
    Serial.print(i);
    Serial.print(" Temp:");
    Serial.println(sensors.getTempC(addr));
  }
  Serial.println("==================================");
}