//
// ds18b20
//
// gnd -> gnd
// data -> D2
//      -> R4.7k -> 3v3 (pullup)
// vcc -> 3v3
//

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    Serial.print("0x");
    if (deviceAddress[i] < 0x10)
      Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i < 7)
      Serial.print(", ");
  }
}

void setup()
{  
  Serial.begin(9600);

  Serial.println("START");

  sensors.begin();

  int cnt = sensors.getDeviceCount();

  Serial.print("device count: ");
  Serial.println(cnt);

  for (int idx = 0; idx < cnt; ++idx)
  {
    DeviceAddress addr;
    sensors.getAddress(addr, idx);
    Serial.print("uint8_t sensor");
    Serial.print(idx);
    Serial.print("[8] = {");
    printAddress(addr);
    Serial.println("};");
  }
}

uint8_t sensor1[8] = {0x28, 0x65, 0xE0, 0x5A, 0x05, 0x00, 0x00, 0x1A};

void loop()
{  
  sensors.requestTemperatures();

  float tempC = sensors.getTempCByIndex(0);
  //float tempC = sensors.getTempC(sensor1);

  if (tempC != DEVICE_DISCONNECTED_C)
  {
    Serial.println(tempC);
  }
  else
  {
    Serial.println("Error: Could not read temperature data");
  }

  delay(1000);
}