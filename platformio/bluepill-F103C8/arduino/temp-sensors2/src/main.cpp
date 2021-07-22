/*

  sample for stm32 with ds18b20, enc28j60, serial, external trigger

  see stm32f103c8.dwg for wirings

*/

#include <Arduino.h>

#include <UIPEthernet.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include "util-mem.h"

/*
#define PUSHINGBOX_IP 213,186,33,19
#define PUSHINGBOX_HOSTNAME "api.pushingbox.com"
#define PUSHINGBOX_QUERY "/pushingbox?devid=XXXXXXXXXX"
*/
//
// security consideration : instead to connect the webapi using http directly to external site
// consider to use a local server that will forward the call using https ; other options would to use
// https://github.com/OPEnSLab-OSU/SSLClient but flash size consuming
//
#include "/home/devel0/security/pushingbox_videophone_bell.h"

#define EXT_TRIGGER_PIN PA1
#define TEMP_SENSOR_ONEWIRE_PIN PB0
#define ETH_RESET_PIN PB6
#define LED_PIN PC13

// default serial rx:PA_3 tx:PA_2

/**
 * @brief must great than minval depending on resolution
 * 9  bit ( min:100 ms ) ds:73.75ms
 * 10 bit ( min:200 ms ) ds:187.5ms
 * 11 bit ( min:400 ms ) ds:375ms
 * 12 bit ( min:800 ms ) ds:750ms
 * 
 * consider round up values from ds(datasheet) if more thermomether on same bus.
 * note that isConversionComplete() works evaluating on the first temp sensor
 */
#define TEMPERATURE_UPDATE_INTERVAL_MS 1000

#define TEMPERATURE_ADDRESS_BYTES 8
OneWire tempOneWire(TEMP_SENSOR_ONEWIRE_PIN);
DallasTemperature DS18B20(&tempOneWire);
int tempDevCount = 0;
uint32_t temperatureReadTimestamp = 0;
bool temperatureWaitConversion = false;
float *temperatures = NULL;    // current temp
DeviceAddress *tempDevAddress; // DeviceAddress defined as uint8_t[8]
char **tempDevHexAddress;

#define MACADDRESS 0x02, 0xca, 0xfe, 0x62, 0x2e, 0x92
#define LISTENPORT 80
unsigned char *header;
EthernetServer server = EthernetServer(LISTENPORT);

volatile bool extTrigger = false;
bool inhibitTrigger = false;
uint32_t triggerTimestamp = 0;
#define TRIGGER_DEBOUNCE_MS 500
bool inhibitPushing = false;
uint32_t triggerPushTimestamp = 0;
#define TRIGGER_PUSH_MIN_INTERVAL_MS 60000

int freeram_min = -1;
uint32_t freeramTimestamp = 0;
#define FREERAM_UPD_INTERVAL_MS 5000

void extTriggerEvt()
{
  extTrigger = true;
}

void setupPins()
{
  //pinMode(LED_PIN, OUTPUT);
  pinMode(EXT_TRIGGER_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(EXT_TRIGGER_PIN), extTriggerEvt, CHANGE);
}

void resetEth()
{
  pinMode(ETH_RESET_PIN, OUTPUT);
  digitalWrite(ETH_RESET_PIN, LOW);
  delay(500);
  digitalWrite(ETH_RESET_PIN, HIGH);
  pinMode(ETH_RESET_PIN, INPUT);
  delay(1000);
}

void setupEth()
{
  uint8_t mac[6] = {MACADDRESS};

  Ethernet.begin(mac);

  Serial.printf("my ip : ");
  for (int i = 0; i < 4; ++i)
  {
    Serial.printf("%d", Ethernet.localIP()[i]);
    if (i != 3)
      Serial.print(".");
  }
  Serial.println();
}

void setupTemperature()
{
  DS18B20.begin();

  tempDevCount = DS18B20.getDeviceCount();

  Serial.printf("Temperature devices count: %d\n", tempDevCount);

  if (tempDevCount > 0)
  {
    temperatures = new float[tempDevCount];
    tempDevAddress = new DeviceAddress[tempDevCount];
    tempDevHexAddress = (char **)malloc(sizeof(char *) * tempDevCount);

    DS18B20.setResolution(12);

    for (int i = 0; i < tempDevCount; ++i)
    {
      tempDevHexAddress[i] = (char *)malloc(sizeof(char) * (TEMPERATURE_ADDRESS_BYTES * 2 + 1));
      DS18B20.getAddress(tempDevAddress[i], i);
      sprintf(tempDevHexAddress[i], "%02x%02x%02x%02x%02x%02x%02x%02x",
              tempDevAddress[i][0],
              tempDevAddress[i][1],
              tempDevAddress[i][2],
              tempDevAddress[i][3],
              tempDevAddress[i][4],
              tempDevAddress[i][5],
              tempDevAddress[i][6],
              tempDevAddress[i][7]);

      Serial.printf("sensor [%d] address = %s\n", i, tempDevHexAddress[i]);
    }
  }
}

void clientEnd(EthernetClient &client)
{
#if ENABLE_CORS == 1
  client.println("Access-Control-Allow-Origin: *");
#endif

  client.println("Connection: close");
  client.println();
}

enum CCTypes
{
  HTML,
  JSON,
  TEXT,
  JAVASCRIPT
};

void clientOk(EthernetClient &client, CCTypes type)
{
  client.println("HTTP/1.1 200 OK");
  switch (type)
  {
  case HTML:
    client.println("Content-Type: text/html");
    break;

  case JSON:
    client.println("Content-Type: application/json");
    break;

  case TEXT:
    client.println("Content-Type: text/plain");
    break;

  case JAVASCRIPT:
    client.println("Content-Type: text/javascript");
    break;
  }

  clientEnd(client);
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Temperature and trigger gateway");

  setupPins();

  Serial.println("Initializing temperature devices");
  setupTemperature();

  Serial.println("Initializing ethernet devices");
  resetEth();

  setupEth();

  server.begin();
}

void loop()
{
  if (millis() - freeramTimestamp > FREERAM_UPD_INTERVAL_MS)
  {
    auto freeram = GetMemFree();
    if (freeram_min == -1 || freeram < freeram_min)
      freeram_min = freeram;
    freeramTimestamp = millis();
  }

  if (tempDevCount > 0 && !temperatureWaitConversion)
  {
    DS18B20.requestTemperatures();
    temperatureWaitConversion = true;
  }

  if (temperatureWaitConversion &&
      temperatureReadTimestamp - millis() > TEMPERATURE_UPDATE_INTERVAL_MS &&
      DS18B20.isConversionComplete())
  {
    for (int i = 0; i < tempDevCount; ++i)
    {
      auto temp = DS18B20.getTempC(tempDevAddress[i]);
      temperatures[i] = temp;

      // Serial.printf("temperature dev [%d] = %fC\n", i, temp);
    }
    temperatureWaitConversion = false;
    temperatureReadTimestamp = millis();
  }

  if (inhibitTrigger && millis() - triggerTimestamp > TRIGGER_DEBOUNCE_MS)
  {
    inhibitTrigger = false;
  }

  if (inhibitPushing && millis() - triggerPushTimestamp > TRIGGER_PUSH_MIN_INTERVAL_MS)
  {
    inhibitPushing = false;
  }

  if (extTrigger && !inhibitTrigger)
  {
    Serial.println("TRIGGER");

    if (!inhibitPushing)
    {
      Serial.println("PUSHING");

      EthernetClient client;
      IPAddress addr(PUSHINGBOX_IP);
      if (client.connect(addr, 80))
      {
        client.print("GET ");
        client.print(PUSHINGBOX_QUERY);
        client.println(" HTTP/1.1");

        client.print("Host: ");
        client.println(PUSHINGBOX_HOSTNAME);
        client.println();

        client.stop();

        inhibitPushing = true;
        triggerPushTimestamp = millis();
      }
    }

    extTrigger = false;
    inhibitTrigger = true;
    triggerTimestamp = millis();
  }
  else if (EthernetClient client = server.available())
  {
    auto str = client.readStringUntil('\n');
    str.trim();

    Serial.printf("read [%s]\n", str.c_str());

    //--------------------------
    // /temp/{id}
    //--------------------------
    if (str.startsWith("GET /temp/"))
    {
      bool found = false;

      auto hexaddr = str.substring(10, 10 + 2 * TEMPERATURE_ADDRESS_BYTES);

      if (hexaddr.length() > 0)
      {
        clientOk(client, TEXT);

        for (int i = 0; i < tempDevCount; ++i)
        {
          if (hexaddr == tempDevHexAddress[i])
          {
            client.printf("%f", temperatures[i]);

            found = true;
            break;
          }
        }
      }
      if (!found)
        client.print(F("not found"));

      client.stop();
    }
    //--------------------------
    // /tempdevices
    //--------------------------
    else if (str.startsWith("GET /tempdevices "))
    {
      Serial.println("temp devices");
      clientOk(client, CCTypes::JSON);

      client.print("{\"tempdevices\":[");
      for (int i = 0; i < tempDevCount; ++i)
      {
        client.printf("{\"id\":\"%s\",\"name\":\"", tempDevHexAddress[i]);

        auto name = String("unk");
        auto id = String(tempDevHexAddress[i]);
        if (id == "28b5742407000084")
          name = "external";
        else if (id == "2833bf3a050000ec")
          name = "bedroom";
        else if (id == "28cc5d3a050000e3")
          name = "bathroom";
        else if (id == "288aef140500008d")
          name = "lab";
        else if (id == "2872685a05000031")
          name = "living-room";

        client.printf("%s\"}", name.c_str());

        if (i != tempDevCount - 1)
          client.print(',');
      }
      client.print("]}");

      client.stop();
    }
    //--------------------------
    // /info
    //--------------------------
    else if (str.startsWith("GET /info "))
    {
      clientOk(client, JSON);

      client.printf("{\"freeram\":%lu,\"freeram_min\":%lu}", (long)GetMemFree(), (long)freeram_min);

      client.stop();
    }
    //--------------------------
    // /index.htm
    //--------------------------
    else if (str.startsWith("GET / ") || str.startsWith("GET /index.htm"))
    {
      clientOk(client, HTML);

      client.print(
#include "index.htm.h"
      );

      client.stop();
    }
  }
}