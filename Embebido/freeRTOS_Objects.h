#pragma once

#define MAX_EVENTS_QUEUE 1

TimerHandle_t xTimer = NULL;
QueueHandle_t timeEventsQueue = NULL;
SemaphoreHandle_t showTimerSemaphore = NULL;
