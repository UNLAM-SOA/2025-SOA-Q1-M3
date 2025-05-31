#pragma once
#include "setup_utils.h"
#include "config.h"
#include "fisical.h"
#include "event_types.h"
#include "freeRTOS_Tasks.h"
#include "./Drivers/MQTT_Driver.h"

typedef void (*action)();

// Actions
void initialize()
{

 // Lee archivo con horarios y dias
 configSetup();

 fisicalSetup();
 attachInterrupt(LIMIT_SWITCH_PIN, detectMovingLimitSwitch, RISING);
 attachInterrupt(BUTTON_PIN, detectButtonPress, RISING); // Configura la interrupción para el botón
 setupWifi();
 setupTime();

 // Calcula el proximo horario y dia
 createNewScheduledTimer();

 queueSetup();
 semaphoreSetup();

 xTaskCreate(showHourTimerLCD, "showHourTimerLCD", 2048, NULL, 1, NULL);
 xTaskCreate(notifyDoseAvailable, "notifyDoseAvailable", 2048, NULL, 1, NULL);
 xTaskCreate(notifyDoseUnnavailable, "notifyDoseUnnavailable", 2048, NULL, 1, NULL);

 mqtt_setup();
}

void noScheduleSet();
void settingSchedule();
void awaitingTimer();
void scanning();
void pillDetected();
void noPillDetected();
void doseTaken();
void stopReturning();
void doseSkipped();
void reanudeCycle();
void pauseCycle();

void error();
void none();

void error()
{
}
void none()
{
}

void modifyVolume()
{
 setVolumeBuzzer(potentiometerLastValue);

 Serial.print("New volume: ");
 DebugPrint(potentiometerLastValue);
}

void doseTaken()
{
 xSemaphoreTake(noPillNotificationSemaphore, 0);
 xSemaphoreTake(notificationSemaphore, 0);
 setLeds[objetivePeriod](LOW);
 stopBuzzer();
 writeLCD("Dose taken\nReturning...");
 startMotorLeft();
}
void stopReturning()
{
 stopMotor();
 xSemaphoreGive(showTimerSemaphore);
 objetiveDay = NO_PILL_TOOKING;
 objetivePeriod = NO_PILL_TOOKING;
 DebugPrint("Stop returning...");
}
void awaitingTimer()
{
 DebugPrint("Awaiting timer...");
 xSemaphoreGive(showTimerSemaphore);
}
void moving()
{
 DebugPrint("Moving...");
 xSemaphoreTake(showTimerSemaphore, 0);
 writeLCD("Moving...");
 setDayAndPeriod();
 startMotorRight();
}
void scanning()
{
 setLeds[objetivePeriod](LOW);
 stopBuzzer();
 stopMotor();
 DebugPrint("Scanning");
}
void pillDetected()
{
 writeLCD("Pill detected");
 xSemaphoreGive(notificationSemaphore);
 DebugPrint("Pill detected...");
}
void noPillDetected()
{
 DebugPrint("No pill detected...");
 xSemaphoreGive(noPillNotificationSemaphore);
}
void doseSkipped()
{
 xSemaphoreTake(notificationSemaphore, 0);
 xSemaphoreTake(noPillNotificationSemaphore, 0);
 setLeds[objetivePeriod](LOW);
 stopBuzzer();
 DebugPrint("Dose skipped...");
 writeLCD("Dose skipped\nReturning...");
 startMotorLeft();
}
void settingSchedule()
{
 DebugPrint("Setting schedule...");
 xSemaphoreGive(showTimerSemaphore);
}
void noScheduleSet()
{
 DebugPrint("No schedule set...");
 xSemaphoreGive(showTimerSemaphore);
}
void reanudeCycle()
{
 xSemaphoreGive(showTimerSemaphore);
}
void pauseCycle()
{
 xSemaphoreTake(showTimerSemaphore, 0);
 writeLCD("Cycle paused\nPress button to resume...");
}