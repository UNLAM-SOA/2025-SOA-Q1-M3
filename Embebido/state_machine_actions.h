#include "timer_schedule.h"
#include <WiFi.h>
#include "time.h"

#include "fisical.h"
#include "state_machine.h"
#include "timer_schedule.h"

#define HOUR_UPDATE_INTERVAL 10000 // 10 seconds
const char *ssid = "Wokwi-GUEST";
const char *password = "";

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -4 * 3600;
const int daylightOffset_sec = 3600;

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

void setupWifi()
{
 Serial.println("Connecting to WiFi...");
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED)
 {
  delay(1000);
  Serial.println("Connecting to WiFi...");
 }
 Serial.println("Connected to WiFi!");
}

void setupTime()
{
 configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
 Serial.println("Time synchronized with NTP server.");
 while (!getLocalTime(&timeinfo, 1000))
 {
  Serial.println("Failed to obtain time");
  delay(1000);
 }
 Serial.println("Time succesfully setted");
}

void printLocalTime()
{
 Serial.print("Current time: ");
 Serial.print(timeinfo.tm_hour);
 Serial.print(":");
 Serial.print(timeinfo.tm_min);
 Serial.print(":");
 Serial.println(timeinfo.tm_sec);
}

void createNewScheduledTimer()
{

 getLocalTime(&timeinfo, 5); // Update RTC time

 searchNextSchedule(&timeinfo); // Set nextPeriod to the next schedule based on the current time

 const int timeUntilNextScheduleValue = timeUntilNextSchedule(&timeinfo, &schedule[nextPeriod]); // Calculate time until next schedule

 Serial.println(timeUntilNextScheduleValue);
 xTimer = xTimerCreate("ScheduleTime", pdMS_TO_TICKS(timeUntilNextScheduleValue), pdFALSE, NULL, handleTimerCallback);

 if (xTimer != NULL)
 {
  xTimerStart(xTimer, 0);
 }
 else
 {
  Serial.println("Failed to create timer.");
 }
}

void handleTimerCallback(TimerHandle_t xTimer)
{
 xTaskNotifyGive(eventTaskHandle);
 xTimerDelete(xTimer, 0);
 createNewScheduledTimer();
}

void testTimer(void *param)
{
 int schedulePos;
 while (1)
 {
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  schedulePos = *(int *)param;
  Serial.println("Timer triggered!");
  xQueueSend(timeEventsQueue, &schedulePos, 0); // Send the event to the queue
 }
}

void detectMovingLimitSwitch(void)
{
 limitSwitchPassed++;
}


typedef void (*action)();

// Actions
void initialize() 
{
    // Lee archivo con horarios y dias
    // Calcula el proximo horario y dia
    // Pasa al estado awaiting reminder time
    Serial.begin(9600);
    fisicalSetup();
    attachInterrupt(LIMIT_SWITCH_MOVIL, detectMovingLimitSwitch, RISING);
    setupWifi();
    setupTime();
    createNewScheduledTimer();
    xTaskCreate(testTimer, "testTimer", 2048, &nextPeriod, 1, &eventTaskHandle);
    timeEventsQueue = xQueueCreate(MAX_EVENTS_QUEUE, sizeof(events));

}
void noScheduleSet();
void settingSchedule();
void awaitingTimer();
void moving();
void scanning();
void pillDetected();
void noPillDetected();
void doseSkipped();
void returnToHome();
void error();
void none();

void error()
{
}
void none()
{
}