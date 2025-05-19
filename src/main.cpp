#include <Arduino.h>
#include "SIM7600G/SIM7600G.h"
#include "OLED/SSD1306.h"
#include "DHT/DHT22.h"
#include "Data/DataHandler.h"
#include "MPU6050/MPU6050.h"

unsigned long mainStartTime = 0;

void setup()
{
  mainStartTime = millis();
  Serial.begin(115200);
  oledBegin();
  SIM7600Gbegin();
  SPIFFSBegin();
  dhtBegin();
  mpuBegin();

  clearScreen();
  header(getData("topic.txt"), false);
}

void loop()
{
  gpsReading gps = getGPS();
  dhtReading dht = getDHT();
  mpuReading mpu = getMPU(); 

  String speedStr = String(gps.speed, 2);
  

  if (gps.latitude != "0.00" && gps.longitude != "0.00")
  {
    header(getData("topic.txt"), true);
  }
  else
  {
    header(getData("topic.txt"), false);
    String saved_data = getData("gps.txt");
    Serial.println(saved_data);

    gps.latitude = splitString(saved_data, ',', 0);
    gps.longitude = splitString(saved_data, ',', 1);
  }

  // Tampilkan data ke layar OLED
  gpsDisplay(String(gps.latitude), String(gps.longitude));
  dhtDisplay(String(dht.temperature), String(dht.humidity));
  mpuDisplay(String(mpu.accel.x), String(mpu.accel.y), String(mpu.accel.z));
  speedDisplay(speedStr);
  delay(100);
  if (millis() - mainStartTime > 5000)
  {
    String payload = gps.latitude + "," + gps.longitude + "," + String(dht.temperature) + "," + String(dht.humidity) + "," + String(mpu.accel.x) + "," + String(mpu.accel.y) + "," + String(mpu.accel.z) + "," + String(speedStr);
    publish(payload);
    mainStartTime = millis();
  }
}
