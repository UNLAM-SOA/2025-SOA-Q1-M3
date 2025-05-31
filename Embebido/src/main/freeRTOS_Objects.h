#ifndef FREERTOS_OBJECTS_H
#define FREERTOS_OBJECTS_H

extern TimerHandle_t xTimer;
extern QueueHandle_t timeEventsQueue;
extern QueueHandle_t buttonEventsQueue;
extern SemaphoreHandle_t showTimerSemaphore;
extern SemaphoreHandle_t lcdMutex;
extern SemaphoreHandle_t notificationSemaphore;
extern SemaphoreHandle_t noPillNotificationSemaphore;

void waitForSemaphore(void (*callback)(void *), void *params, SemaphoreHandle_t semaphore);

#endif