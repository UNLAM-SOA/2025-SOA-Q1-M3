#pragma once

#define MAX_EVENTS_QUEUE 1

TimerHandle_t xTimer = NULL;
TaskHandle_t limitSwitchTaskHandler = NULL;
QueueHandle_t timeEventsQueue = NULL;
QueueHandle_t buttonEventsQueue = NULL;
SemaphoreHandle_t showTimerSemaphore = NULL;
SemaphoreHandle_t lcdMutex;
SemaphoreHandle_t notificationSemaphore;
SemaphoreHandle_t noPillNotificationSemaphore;

void waitForSemaphore(void (*callback)(void *), void *params, SemaphoreHandle_t semaphore)
{
 if (uxSemaphoreGetCount(semaphore) > 0)
 {
  callback(params);
 }
 else
 {
  xSemaphoreTake(semaphore, portMAX_DELAY); // Wait for the semaphore to be available
  xSemaphoreGive(semaphore);                // Release the semaphore
 }
}