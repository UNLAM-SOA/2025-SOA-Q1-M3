#include "freeRTOS_Objects.h"

TimerHandle_t xTimer = NULL;
TaskHandle_t limitSwitchTaskHandler = NULL;
QueueHandle_t timeEventsQueue = NULL;
QueueHandle_t buttonEventsQueue = NULL;
SemaphoreHandle_t showTimerSemaphore = NULL;
SemaphoreHandle_t lcdMutex = NULL;
SemaphoreHandle_t notificationSemaphore = NULL;
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