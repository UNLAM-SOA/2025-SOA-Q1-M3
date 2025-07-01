#pragma once
#include "timer_schedule.h"
#include <WiFi.h>
#include "time.h"

#include "fisical.h"
#include "state_machine.h"
#include "esp_timer.h"

#define HOUR_TO_SECONDS 3600
#define GMT_DIFFERENCE -4
#define TIMEOUT_SETUP 1000
#define TIMEOUT_GET_TIME 5
#define DEBOUNCE_MICROS 50000
#define BUTTON_QUEUE_SIZE 3

const char *ssid = "TP_LINK_97CC";
const char *password = "serafin1";

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

volatile uint64_t lastInterruptTime = 0;
volatile unsigned long lastButtonPressTime = 0;
void IRAM_ATTR detectMovingLimitSwitch()
{
 uint64_t interruptTime = esp_timer_get_time();
 if (interruptTime - lastInterruptTime > DEBOUNCE_MICROS)
 {
  if (new_event == 9)
  {
   xTaskNotifyGive(limitSwitchTaskHandler);
  }
  else if (new_event == 1)
  {
  }
  else
  {
   limitSwitchPassed++;
  }
  lastInterruptTime = interruptTime;
 }
}

static uint32_t buttonLastInterruptTime = 0; // Guarda el último tiempo de interrupción
// Umbral de debounce (ejemplo: 200ms = 200,000 µs)
const uint32_t buttonDebounceDelay = 50000;
void IRAM_ATTR detectButtonPress()
{

 uint32_t buttonInterruptTime = micros(); // Obtiene el tiempo actual en microsegundos

 // Si la interrupción ocurrió demasiado pronto, se ignora (debounce)
 if (buttonInterruptTime - buttonLastInterruptTime < buttonDebounceDelay)
 {
  return;
 }

 // Actualiza el último tiempo válido
 buttonLastInterruptTime = buttonInterruptTime;
 // Envía el evento a la cola (si está inicializada)
 BaseType_t xHigherPriorityTaskWoken = pdFALSE;
 if (buttonEventsQueue != NULL)
 {
  xQueueSendFromISR(buttonEventsQueue, &buttonInterruptTime, &xHigherPriorityTaskWoken);
 }

 // Si se debe hacer un context switch
 if (xHigherPriorityTaskWoken == pdTRUE)
 {
  portYIELD_FROM_ISR();
 }
}

volatile unsigned long startLt = 0; // Last time the limit switch was pressed
void limitSwitchStart()
{
 unsigned long ct = micros();
 if (ct - startLt > DEBOUNCE_MICROS)
 {
  if (!startPressed)
  {
   stopMotor();
  }
  startPressed = !startPressed;
  startLt = ct;
 }
}

void queueSetup()
{
 timeEventsQueue = xQueueCreate(MAX_EVENTS, sizeof(events));
 buttonEventsQueue = xQueueCreate(BUTTON_QUEUE_SIZE, sizeof(unsigned long));
}

void semaphoreSetup()
{
 showTimerSemaphore = xSemaphoreCreateMutex();
 lcdMutex = xSemaphoreCreateMutex();
 notificationSemaphore = xSemaphoreCreateMutex();
 noPillNotificationSemaphore = xSemaphoreCreateMutex();
 scanningCompletedSemaphore = xSemaphoreCreateBinary();

 xSemaphoreTake(noPillNotificationSemaphore, 0);
 xSemaphoreTake(notificationSemaphore, 0);
 xSemaphoreTake(scanningCompletedSemaphore, 0);
}
