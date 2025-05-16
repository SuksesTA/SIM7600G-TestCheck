#include <Arduino.h>
#include "SIM7600G.h"

HardwareSerial SerialAT(2);

String broker_ip = "34.162.162.155";
bool gps_state = false;
bool ready = false;
bool sim_ready = true;

unsigned long startTime = 0;

String sendAT(String command, String expected = "")
{
  SerialAT.print(command + "\r");

  String response = "";
  bool ok_status = false;
  bool error_status = false;
  bool accept_response = false;
  startTime = millis();
  while (SerialAT.available() || (!ok_status && !error_status) && millis() - startTime < 120000)
  {
    String temp = SerialAT.readStringUntil('\n');
    temp.trim();
    Serial.println(temp);

    response += temp;

    if (expected.length() > 0 && temp.indexOf(expected) != -1)
    {
      response = temp;
      ok_status = true;
    }

    if (temp.indexOf(command) != -1)
    {
      accept_response = true;
    }

    if ((temp.indexOf("OK") != -1 && accept_response))
    {
      ok_status = true;
    }
    else if (temp.indexOf("ERROR") != -1 && accept_response)
    {
      error_status = true;
    }
    else if (temp.indexOf("RDY") != -1)
    {
      response = "";
      ready = true;
      ok_status = true;
    }
    else if (response.indexOf("+CME ERROR: SIM not inserted") != -1)
    {
      sim_ready = false;
    }

    response += "\n";
  }

  if (millis() - startTime > 120000)
  {
    SerialAT.print("AT+CFUN=6\r");
    ready = false;
    while (!ready)
    {
      while (SerialAT.available())
      {
        String temp = SerialAT.readStringUntil('\n');
        temp.trim();
        Serial.println(temp);
        if (temp.indexOf("RDY") != -1)
        {
          response = "";
          ready = true;
        }
      }
    }
    delay(500);
  }

  return response;
}

void beginGPS()
{
  int mode = 1;
  String _mode[] = {"", "1", "2", "3"};
  String check = sendAT("AT+CGPS?");
  if (check.indexOf("+CGPS: 0") != -1)
  {
    Serial.println("GPS Disabled");
    Serial.println("Starting GPS...");
    sendAT("AT+CGPS=1," + _mode[mode]);
  }
  else if (check.indexOf("+CGPS: 1,1") != -1)
  {
    Serial.println("GPS Enabled");
    gps_state = true;
  }
}

void SIM7600Gbegin()
{
  Serial.println("Starting SIM7600");
  startTime = millis();
  Serial.println("Start time : " + String(startTime));
  SerialAT.begin(115200);

  if (!sim_ready)
  {
    return;
  }

  clearScreen();
  println("GPS TRACKER");
  println("FOR DOWN SYNDROME");
  for (int i = 0; i < 21; i++)
  {
    print("=");
  }
  print("\n");
  println("SIM7600G");

  sendAT("ATI");

  sendAT("AT+CMEE=2");

  unsigned long endTime = millis() - startTime;
  float duration = endTime / 1000;
  Serial.print("Ready status : ");
  Serial.println(ready ? "Ready" : "Failure");
  println("Duration : " + String(duration) + " Second");
  Serial.println("=== End of SIM7600 Initialization ===");
}

String splitString(String input, char delimiter, int index = 0)
{
  input.trim();
  String result;
  int prev_delimiter_index = 0;
  int temp_index = 0;

  for (int i = 0; i < input.length(); i++)
  {
    if (input[i] == delimiter)
    {
      if (temp_index == index)
      {
        if (prev_delimiter_index == 0)
        {
          result = input.substring(prev_delimiter_index, i);
        }
        else
        {
          result = input.substring(prev_delimiter_index + 1, i);
        }
      }
      temp_index++;
      prev_delimiter_index = i;
    }
  }

  if (result.isEmpty())
  {
    result = input.substring(prev_delimiter_index + 1, input.length());
  }

  return result;
}

gpsReading getGPS()
{
  /**
   * https://electronics.stackexchange.com/questions/601046/assisted-gps-agps-on-sim7600-module-does-not-work
   */
  gpsReading gps;
  if (!gps_state)
  {
    beginGPS();
  }

  /* AT+CGSPINFO - response
  AT+CGPSINFO

  +CGPSINFO: 0614.354283,S,10651.371415,E,230624,104328.0,2.9,0.0,

  OK


  EXPECTED OUTCOME: -6.9692767,107.6255821
  */
  String gps_data = sendAT("AT+CGPSINFO");
  bool complete = false;

  gps_data.replace("AT+CGPSINFO", "");
  gps_data.replace("+CGPSINFO: ", "");
  gps_data.replace("OK", "");
  gps_data.trim();

  String _data = splitString(gps_data, '\n', 1);

  String lat = splitString(_data, ',');
  String lon = splitString(_data, ',', 2);

  if ((lat.isEmpty() && lon.isEmpty()) || (lat.length() < 7 && lon.length() < 7))
  {
    return gps;
  }

  String NS = splitString(_data, ',', 1);
  String startLat;
  if (NS.indexOf("S") != -1)
  {
    startLat = "-";
  }
  else if (NS.indexOf("N") != -1)
  {
    startLat = "+";
  }

  // Latitude || 0614.354283
  String lat_deg = lat.substring(0, 1);
  lat_deg.replace("0", "");
  String lat_min = lat.substring(1);
  lat_min.replace(".", "");
  float lat_min_dec = lat_min.toInt() / 6;
  String lat_min_5dec = String(lat_min_dec).substring(0, 5); // 5 decimal

  // Longitude || 10651.371415
  String lon_deg = lon.substring(0, 3);
  String lon_min = lon.substring(3);
  lon_min.replace(".", "");
  float lon_min_dec = lon_min.toInt() / 6;
  String lon_min_5dec = String(lon_min_dec).substring(0, 5); // 5 decimal

  // Cleaning
  lat_min_5dec.replace(".", "");
  lon_min_5dec.replace(".", "");

  gps.latitude = startLat + lat_deg + "." + lat_min_5dec;
  gps.longitude = lon_deg + "." + lon_min_5dec;

  String data = gps.latitude + "," + gps.longitude;
  saveData(data, "gps.txt");

  return gps;
}

void MQTTStart()
{
  unsigned long seed = millis();
  const char charSet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

  Serial.println("Starting Randomize Topic.");

  String client_id;
  for (int i = 0; i < 10; i++)
  {
    int randomIndex = (seed >> i) % (sizeof(charSet) - 2);
    client_id += charSet[randomIndex];
  }

  String start = sendAT("AT+CMQTTSTART", "+CMQTTSTART: 0");

  if (start.indexOf("+CMQTTSTART: 23") != -1)
  {
    Serial.println("MQTT Opened");

    String client_acquired = sendAT("AT+CMQTTACCQ=0,\"" + client_id + "\",0,4");
    Serial.println("Client id: " + client_id);

    String connection = sendAT("AT+CMQTTCONNECT=0,\"tcp://" + broker_ip + "\",60,1");
    if (connection.indexOf("+CMQTTCONNECT: 0,13") != -1)
    {
      sendAT("AT+CMQTTDISC=0, 120"); // Disconnect
      sendAT("AT+CMQTTREL=0");       // Client Release
      sendAT("AT+CMQTTSTOP");        // Stop MQTT
    }

    return;
  }

  return;
}

void publish(String payload)
{
  // String topic = "test";
  String topic = getData("/topic.txt");
  if (topic.length() == 0)
  {
    randomizeMQTTTopic();
    topic = getData("/topic.txt");
  }

  String response;
  bool set_topic = false;
  bool set_payload = false;
  bool publish = false;
  bool error = false;
  SerialAT.print("AT+CMQTTTOPIC=0," + String(topic.length()) + "\r");
  while (!set_topic && !error)
  {
    while (SerialAT.available())
    {
      String temp = SerialAT.readStringUntil('\n');
      Serial.println(temp);
      if (temp.indexOf("+CMQTTTOPIC: 0,11") != -1 || temp.indexOf("+CMQTTTOPIC: 0,14") != -1)
      {
        Serial.println("ERROR: no connection\nStarting MQTT on SIM7600...");
        MQTTStart();
        error = true;
      }
      if (temp.indexOf("OK") != -1)
      {
        set_topic = true;
      }
      else if (temp.indexOf("ERROR") != -1)
      {
        error = true;
      }
      SerialAT.print(topic + "\r");
    }
  }

  if (error)
  {
    return;
  }

  SerialAT.print("AT+CMQTTPAYLOAD=0," + String(payload.length()) + "\r");
  while (!set_payload && !error)
  {
    while (SerialAT.available())
    {
      String temp = SerialAT.readStringUntil('\n');
      Serial.println(temp);
      if (temp.indexOf("OK") != -1)
      {
        set_payload = true;
      }
      SerialAT.print(payload + "\r");
    }
  }

  if (error)
  {
    return;
  }

  sendAT("AT+CMQTTPUB=0,0,120,1", "+CMQTTPUB: 0,0");
}

SIM7600 getDeviceInfo()
{
  SIM7600 device;

  String signal_data = sendAT("AT+CSQ");
  String raw_signal_data = splitString(signal_data, ':');
  raw_signal_data.replace(" ", "");
  String rssi = splitString(raw_signal_data, ',');
  String ber = splitString(raw_signal_data, ',', 1);

  device.signalStrength = rssi.toInt();
  device.errorRate = ber.toInt();

  return device;
}