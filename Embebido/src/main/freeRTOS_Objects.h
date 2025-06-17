#define IS_FISICAL 1

#ifndef FREERTOS_OBJECTS_H
#define FREERTOS_OBJECTS_H

extern TimerHandle_t xTimer;
extern TaskHandle_t limitSwitchTaskHandler;
extern QueueHandle_t timeEventsQueue;
extern QueueHandle_t buttonEventsQueue;
extern SemaphoreHandle_t showTimerSemaphore;
extern SemaphoreHandle_t scanningCompletedSemaphore;
extern SemaphoreHandle_t lcdMutex;
extern SemaphoreHandle_t notificationSemaphore;
extern SemaphoreHandle_t noPillNotificationSemaphore;

void waitForSemaphore(void (*callback)(void *), void *params, SemaphoreHandle_t semaphore);

#if IS_FISICAL

TimerHandle_t xTimer = NULL;
TaskHandle_t limitSwitchTaskHandler = NULL;
QueueHandle_t timeEventsQueue = NULL;
QueueHandle_t buttonEventsQueue = NULL;
SemaphoreHandle_t showTimerSemaphore = NULL;
SemaphoreHandle_t lcdMutex = NULL;
SemaphoreHandle_t notificationSemaphore = NULL;
SemaphoreHandle_t scanningCompletedSemaphore = NULL;
SemaphoreHandle_t noPillNotificationSemaphore = NULL;

void waitForSemaphore(void (*callback)(void *), void *params, SemaphoreHandle_t semaphore)
{
 if (uxSemaphoreGetCount(semaphore) > 0)
 {
  callback(params);
 }
 else
 {
  xSemaphoreTake(semaphore, portMAX_DELAY);
  xSemaphoreGive(semaphore);
 }
}

#endif

#endif // FREERTOS_OBJECTS_H
