#if !defined(DataHandler)
#define DataHandler

#include <Arduino.h>
#include <SPIFFS.h>
#include <cstdlib>
#include "OLED/SSD1306.h"

#define MAX_BUFFER_SIZE 256

/*
DO NOT USE SAVE DATA FUNCTION TO UPDATE A DATA

Please read the data first and then save it using save data function to ensure no missing data.

List of filename:
- "topic.txt" : topic to sub and pub
 */

bool SPIFFSBegin();
bool saveData(String data, String filename);
String getData(String filename);
bool randomizeMQTTTopic(int length = 6);

#endif // DataHandler
