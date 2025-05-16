#include "DHT22.h"

DHT dht(DATA, DHTType);

void dhtBegin()
{
  dht.begin();
  println("DHT22 Started.");
}

dhtReading getDHT()
{
  dhtReading reading;

  float temp = dht.readTemperature();
  float humi = dht.readHumidity();

  if (isnan(temp) || isnan(humi))
  {
    reading.temperature = -100; // Or any value to indicate error
    reading.humidity = -1;      // Or any value to indicate error
  }
  else
  {
    float heat = dht.computeHeatIndex();
    reading.temperature = temp;
    reading.humidity = humi;
    reading.heatIndex = heat;
  }
  return reading;
}