#pragma once
#include "timer_schedule.h"
#include "event_types.h"
#include <WiFi.h>
#include "freeRTOS_Objects.h"
#include "./Drivers/LCD_Driver.h"

const char *ssid = "Wokwi-GUEST";
const char *password = "";

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -4 * 3600;
const int daylightOffset_sec = 3600;

void handleTimerCallback(TimerHandle_t xTimer);
void createNewScheduledTimer();
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

void handleTimerCallback(TimerHandle_t xTimer)
{
 if (xTimer != NULL)
 {
  xQueueSend(timeEventsQueue, &nextPeriod, 0);
  xTimerDelete(xTimer, 0);
  xTimer = NULL; // limpiar handle
  createNewScheduledTimer();
 }
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

void detectMovingLimitSwitch(void)
{
 limitSwitchPassed++;
}

typedef void (*action)();

void showHourTimerLCD(void *)
{
 short displayingCurrentTime = 1;
 while (1)
 {
  if (uxSemaphoreGetCount(showTimerSemaphore) > 0)
  {
   getLocalTime(&timeinfo, 10); // Actualiza la hora
   // Semáforo disponible (no lo toma)
   displayingCurrentTime == 1 ? writeLCD("Hora: " + String(timeinfo.tm_hour) + ":" + String(timeinfo.tm_min)) : writeLCD("Proxima toma: " + String(schedule[nextPeriod].tm_hour) + ":" + String(schedule[nextPeriod].tm_min));
   displayingCurrentTime *= -1; // Cambia el estado de la variable
   Serial.println("Show hour timer LCD");
   vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  else
  {
   // Semáforo ocupado: tomar con bloqueo
   xSemaphoreTake(showTimerSemaphore, portMAX_DELAY);
  }
 }
}
// Actions
void initialize()
{

 // Lee archivo con horarios y dias
 // Calcula el proximo horario y dia
 // Pasa al estado awaiting reminder time
 fisicalSetup();
 attachInterrupt(LIMIT_SWITCH_MOVIL, detectMovingLimitSwitch, RISING);
 setupWifi();
 setupTime();
 createNewScheduledTimer();
 timeEventsQueue = xQueueCreate(MAX_EVENTS_QUEUE, sizeof(events));
 showTimerSemaphore = xSemaphoreCreateMutex();
 xTaskCreate(showHourTimerLCD, "showHourTimerLCD", 1024, NULL, 1, NULL);
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

void awaitingTimer()
{
 Serial.println("Awaiting timer...");
 xSemaphoreGive(showTimerSemaphore);
}
void moving()
{
 Serial.println("Moving...");
 xSemaphoreTake(showTimerSemaphore, 0);
 setDayAndPeriod();
}