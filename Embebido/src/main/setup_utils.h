#pragma once
#include "timer_schedule.h"
#include <WiFi.h>
#include "time.h"

#include "fisical.h"
#include "state_machine.h"

#define HOUR_TO_SECONDS 3600
#define GMT_DIFFERENCE -4
#define TIMEOUT_SETUP 1000
#define TIMEOUT_GET_TIME 5
#define DEBOUNCE_SECONDS 200

//const char *ssid = "Wokwi-GUEST";
const char *ssid = "CLAROVLG";
const char *password = "F6BCA8C4";

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = GMT_DIFFERENCE * HOUR_TO_SECONDS;
const int daylightOffset_sec = HOUR_TO_SECONDS;

const char *weekDays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void handleTimerCallback(TimerHandle_t xTimer);
void createNewScheduledTimer();

struct tm timeinfo;

void setupWifi()
{
 Serial.println("Connecting to WiFi...");
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED)
 {
  delay(TIMEOUT_SETUP);
  Serial.println("Connecting to WiFi...");
 }
 Serial.println("Connected to WiFi!");
}

void setupTime()
{
 configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
 Serial.println("Time synchronized with NTP server.");
 while (!getLocalTime(&timeinfo, TIMEOUT_SETUP))
 {
  Serial.println("Failed to obtain time");
  delay(TIMEOUT_SETUP);
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

 getLocalTime(&timeinfo, TIMEOUT_GET_TIME);                                                      // Update RTC time
 searchNextSchedule(&timeinfo);                                                                  // Set nextPeriod to the next schedule based on the current time
 const int timeUntilNextScheduleValue = timeUntilNextSchedule(&timeinfo, &schedule[nextPeriod]); // Calculate time until next schedule

 Serial.println(String(timeUntilNextScheduleValue) + " ms until next schedule");
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
 if (xTimer != NULL)
 {
  xQueueSend(timeEventsQueue, &nextPeriod, 0);
  xTimerDelete(xTimer, 0);
  xTimer = NULL;
  createNewScheduledTimer();
 }
}

volatile unsigned long lastInterruptTime = 0;
volatile unsigned long lastButtonPressTime = 0;
void detectMovingLimitSwitch()
{
 unsigned long interruptTime = millis();
 if (interruptTime - lastInterruptTime > DEBOUNCE_SECONDS)
 {
  limitSwitchPassed++;
  lastInterruptTime = interruptTime;
 }
}

// TODO: not working fine
void detectButtonPress()
{
 // unsigned long interruptTime = millis();
 // if (interruptTime - lastButtonPressTime > 10)
 //  {
 // 200 ms de debounce
 short buttonState = readButton(); // Lee el estado del botón
 // lastButtonPressTime = interruptTime;
 xQueueSend(buttonEventsQueue, &buttonState, 0); // Enviar evento de botón a la cola
                                                 //  }
}

void queueSetup()
{
 timeEventsQueue = xQueueCreate(MAX_EVENTS, sizeof(events));
 buttonEventsQueue = xQueueCreate(MAX_EVENTS, sizeof(short));
}

void semaphoreSetup()
{
 showTimerSemaphore = xSemaphoreCreateMutex();
 lcdMutex = xSemaphoreCreateMutex();
 notificationSemaphore = xSemaphoreCreateMutex();
 noPillNotificationSemaphore = xSemaphoreCreateMutex();

 xSemaphoreTake(noPillNotificationSemaphore, 0);
 xSemaphoreTake(notificationSemaphore, 0);
}
