#if !defined(DHT22_h)
#define DHT22_h

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "OLED/SSD1306.h"

#define DATA 18 // Change this to the connected data pin for DHT22
#define DHTType DHT22

typedef struct
{
  float temperature;
  float humidity;
  float heatIndex;
} dhtReading;

void dhtBegin();
dhtReading getDHT();

#endif // DHT22_h
