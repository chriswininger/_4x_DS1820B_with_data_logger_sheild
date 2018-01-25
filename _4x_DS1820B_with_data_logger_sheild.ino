#include <OneWire.h> 
#include <DallasTemperature.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"

#define LOG_INTERVAL 1000
#define ONE_WIRE_BUS 5

float temp1;
float temp2;
float temp3;
float temp4;

// setup to read sensor data on Port1 D
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

RTC_PCF8523 RTC;

// the logging file
File logfile;

const int chipSelect = 10;

void setup() {
   while (!Serial) {
    delay(1);  // for Leonardo/Micro/Zero
  }
  Serial.begin(57600);
  Serial.println("Running");

  if (!RTC.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (!RTC.initialized()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // initialse sd card
  if (!SD.begin(chipSelect)) {
    error("Card failed, or not present");
  }

  // create a new file
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
  
  if (! logfile) {
    error("couldnt create file");
  }

  Serial.print("Logging to: ");
  Serial.println(filename);

  // log headers for csv
  // stamp, datetime, temp1, temp2, temp3, temp4, temp5, temp6
  logfile.println("field1,field2,field3,field4,field5,field6");    

  // initialize temperature sensor library
  sensors.begin(); 

}

void loop() {
  DateTime now = RTC.now();
    
  debugTimeStamp(now);

  // read temps
  sensors.requestTemperatures();
  temp1 = celsiusToFarenheit(sensors.getTempCByIndex(0));
  temp2 = celsiusToFarenheit(sensors.getTempCByIndex(1));
  temp3 = celsiusToFarenheit(sensors.getTempCByIndex(2));
  temp4 = celsiusToFarenheit(sensors.getTempCByIndex(3));

  // === wrtie log entry ===
  // - stamp
  logfile.print(now.unixtime());
  // - date
  logfile.print(", \"");
  logfile.print(now.year(), DEC);
  logfile.print("/");
  logfile.print(now.month(), DEC);
  logfile.print("/");
  logfile.print(now.day(), DEC);
  logfile.print(" ");
  logfile.print(now.hour(), DEC);
  logfile.print(":");
  logfile.print(now.minute(), DEC);
  logfile.print(":");
  logfile.print(now.second(), DEC);
  logfile.print("\", ");
  logfile.print(temp1);
  logfile.print(", "); 
  logfile.print(temp2);
  logfile.print(", ");
  logfile.print(temp3);
  logfile.print(", ");
  logfile.print(temp4);
  logfile.print(", ");
  logfile.println();
  // =====

  // log to sserial
  Serial.print("temp1: ");
  Serial.print(temp1);
  Serial.print(", temp2: ");
  Serial.print(temp2);
  Serial.print(", temp3: ");
  Serial.print(temp3);
  Serial.print(", temp4: ");
  Serial.print(temp4);
  Serial.println("");

  logfile.flush();

  delay(LOG_INTERVAL);
}

float celsiusToFarenheit(float c) {
  return c * 1.8 + 32.0;  
}


void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);

  while(1);
}

void debugTimeStamp(DateTime now) {
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");
    
    // calculate a date which is 7 days, 12 hours and 30 seconds into the future
    DateTime future (now + TimeSpan(7,12,30,6));
    
    Serial.print(" now + 7d + 30s: ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();
}
