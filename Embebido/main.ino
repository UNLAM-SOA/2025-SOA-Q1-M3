#include <WiFi.h>
#include "time.h"

#include "fisical.h"
#include "state_machine.h"

#define HOUR_UPDATE_INTERVAL 10000 // 10 seconds
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -3 * 3600;
const int   daylightOffset_sec = 3600;


/*
  ---struct tm---
  Member	Type	Meaning	Range
  tm_sec	int	seconds after the minute	0-60*
  tm_min	int	minutes after the hour	0-59
  tm_hour	int	hours since midnight	0-23
  tm_mday	int	day of the month	1-31
  tm_mon	int	months since January	0-11
  tm_year	int	years since 1900
  tm_wday	int	days since Sunday	0-6
  tm_yday	int	days since January 1	0-365
  tm_isdst	int	Daylight Saving Time flag
*/

tm shchedule[MAX_PERIODS] = {
  {0, 0, 6, 0, 0, 0, 0, 0}, //Domingo, 6:00am
  {0, 0, 6, 0, 0, 0, 1, 0}, //Lunes, 6:00am
  {0, 0, 6, 0, 0, 0, 2, 0}, //Martes, 6:00am
  {0, 0, 6, 0, 0, 0, 3, 0}, //Miercoles, 6:00am
  {0, 0, 6, 0, 0, 0, 4, 0}, //Jueves, 6:00am
  {0, 0, 6, 0, 0, 0, 5, 0}, //Viernes 6:00am
  {0, 0, 6, 0, 0, 0, 6, 0} //Sabado 7:00pm
};
struct tm timeinfo;
void setupWifi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
}
void setupTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Time synchronized with NTP server.");
  while (!getLocalTime(&timeinfo, 1000)) {
    Serial.println("Failed to obtain time");
    delay(1000);
  }
}
void printLocalTime() {
  Serial.print("Current time: ");
  Serial.print(timeinfo.tm_hour);
  Serial.print(":");
  Serial.print(timeinfo.tm_min);
  Serial.print(":");
  Serial.println(timeinfo.tm_sec);
}
void updateHourT(void* param) {
  while (1)
  {
    getLocalTime(&timeinfo, 5);
    printLocalTime();
    vTaskDelay(HOUR_UPDATE_INTERVAL);
  }
}
void setup() {
  Serial.begin(9600);
  fisicalSetup();
  setupWifi();
  setupTime();
  xTaskCreate(updateHourT, "updateHour", 1024, NULL, 1, NULL);
}

void loop() {
  state_machine();
}