#if !defined(SIM7600G_h)
#define SIM7600G_h

#include <Arduino.h>
#include <HardwareSerial.h>
#include "OLED/SSD1306.h"
#include "Data/DataHandler.h"

// Your GPRS credentials, if any
const char apn[] = "internet";
const char gprsUser[] = "";
const char gprsPass[] = "";

typedef struct
{
  String latitude = "0.00";
  String longitude = "0.00";
  float speed = 0.0;
} gpsReading;

typedef struct
{
  int signalStrength = 0;
  int errorRate = 0;
} SIM7600;

void SIM7600Gbegin();
void beginGPS();
SIM7600 getDeviceInfo();
gpsReading getGPS();
void publish(String payload);
String splitString(String input, char delimiter, int index);

#endif // SIM7600G_h