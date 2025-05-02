#include <WiFi.h>
#include "time.h"

#include "fisical.h"
#include "state_machine.h"
#include "timer_schedule.h"

#define HOUR_UPDATE_INTERVAL 10000 // 10 seconds
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -4 * 3600;
const int   daylightOffset_sec = 3600;

TimerHandle_t xTimer = NULL;
TaskHandle_t eventTaskHandle = NULL;


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
  Serial.println("Time succesfully setted");
}
void printLocalTime() {
  Serial.print("Current time: ");
  Serial.print(timeinfo.tm_hour);
  Serial.print(":");
  Serial.print(timeinfo.tm_min);
  Serial.print(":");
  Serial.println(timeinfo.tm_sec);
}

void createNewScheduledTimer() {
  Serial.println("1");
  getLocalTime(&timeinfo, 5); // Update RTC time
  Serial.println("2");
  searchNextSchedule(&timeinfo); // Set nextPeriod to the next schedule based on the current time

  Serial.println("3");
  Serial.println(nextPeriod);
  const int timeUntilNextScheduleValue = timeUntilNextSchedule(&timeinfo, &schedule[nextPeriod]); // Calculate time until next schedule
  Serial.println("3.5");
  Serial.println(timeUntilNextScheduleValue);
  xTimer = xTimerCreate("ScheduleTime", pdMS_TO_TICKS(timeUntilNextScheduleValue), pdFALSE, NULL, handleTimerCallback);
  Serial.println("4");
  if (xTimer != NULL) {
    xTimerStart(xTimer, 0);
    Serial.println("5");
  } else {
    Serial.println("Failed to create timer.");
  }
}

void handleTimerCallback(TimerHandle_t xTimer) {
 xTaskNotifyGive(eventTaskHandle);
 xTimerDelete(xTimer, 0);
 createNewScheduledTimer();
}
void testTimer(void* param){
 const int schedulePos = *(int*)param;
 while(1){
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  Serial.println("Timer triggered!");
  new_event = (events)schedulePos;
 }
}
void detectMovingLimitSwitch(void){
 limitSwitchPassed++;
}
void setup() {
  Serial.begin(9600);
  fisicalSetup();
  attachInterrupt(LIMIT_SWITCH_MOVIL, detectMovingLimitSwitch,RISING);
  setupWifi();
  setupTime();
  createNewScheduledTimer();
  xTaskCreate(testTimer, "testTimer", 2048, &nextPeriod, 1, &eventTaskHandle);
}

void loop() {
  state_machine();
}