#pragma once
#include "./Drivers/MQTT_Driver.h"
#include "Drivers/Queue.h"
#include "config.h"
#include "event_types.h"
#include "fisical.h"
#include "freeRTOS_Tasks.h"
#include "setup_utils.h"

typedef void (*action)();

// Actions
void initialize() {

  // Lee archivo con horarios y dias
  configSetup();

  fisicalSetup();
  attachInterrupt(LIMIT_SWITCH_PIN, detectMovingLimitSwitch, RISING);
  attachInterrupt(BUTTON_PIN, detectButtonPress,
                  RISING); // Configura la interrupción para el botón
  setupWifi();
  setupTime();

  // Calcula el proximo horario y dia
  createNewScheduledTimer();

  queueSetup();
  semaphoreSetup();

  xTaskCreate(showHourTimerLCD, "showHourTimerLCD", 2048, NULL, 1, NULL);
  xTaskCreate(notifyDoseAvailable, "notifyDoseAvailable", 2048, NULL, 1, NULL);
  xTaskCreate(notifyDoseUnnavailable, "notifyDoseUnnavailable", 2048, NULL, 1,
              NULL);

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
void processMessage();

void error();
void none();

void error() {}
void none() {}

bool awaiting = false;

void modifyVolume() {
  setVolumeBuzzer(potentiometerLastValue);

  Serial.print("New volume: ");
  DebugPrint(potentiometerLastValue);
}

void doseTaken() {
  xSemaphoreTake(noPillNotificationSemaphore, 0);
  xSemaphoreTake(notificationSemaphore, 0);
  setLeds[objetivePeriod](LOW);
  stopBuzzer();
  writeLCD("Dose taken\nReturning...");
  startMotorLeft();
}
void stopReturning() {
  stopMotor();
  xSemaphoreGive(showTimerSemaphore);
  objetiveDay = NO_PILL_TOOKING;
  objetivePeriod = NO_PILL_TOOKING;
  DebugPrint("Stop returning...");
}
void awaitingTimer() {

  if (!awaiting) {
    char payload[50];
    snprintf(payload, sizeof(payload), "Awaiting timer...");
    mqtt_publish_message(actual_status_topic, AWAITING, payload);
    awaiting = true;
  }

  
  DebugPrint("Awaiting timer...");
  xSemaphoreGive(showTimerSemaphore);
}
void moving() {
  DebugPrint("Moving...");
  xSemaphoreTake(showTimerSemaphore, 0);
  writeLCD("Moving...");


  if(awaiting){
   char payload[50];
   snprintf(payload, sizeof(payload), "Time to take the pill...");
   mqtt_publish_message(actual_status_topic, TIME_TO_TAKE_PILL, payload);
   awaiting = false;
  }
  
  setDayAndPeriod();
  startMotorRight();
}
void scanning() {
  setLeds[objetivePeriod](LOW);
  stopBuzzer();
  stopMotor();
  DebugPrint("Scanning");
}
void pillDetected() {
  char payload[50];
  snprintf(payload, sizeof(payload), "Pill detected");
  mqtt_publish_message(pill_status_topic, 0, payload);
  writeLCD("Pill detected");
  xSemaphoreGive(notificationSemaphore);
  DebugPrint("Pill detected...");
}
void noPillDetected() {
  char payload[50];
  snprintf(payload, sizeof(payload), "No pill detected");
  mqtt_publish_message(pill_status_topic, 0, payload);
  DebugPrint("No pill detected...");
  xSemaphoreGive(noPillNotificationSemaphore);
}
void doseSkipped() {
  xSemaphoreTake(notificationSemaphore, 0);
  xSemaphoreTake(noPillNotificationSemaphore, 0);
  setLeds[objetivePeriod](LOW);
  stopBuzzer();
  DebugPrint("Dose skipped...");
  writeLCD("Dose skipped\nReturning...");
  startMotorLeft();
}
void settingSchedule() {
  DebugPrint("Setting schedule...");
  xSemaphoreGive(showTimerSemaphore);
}
void noScheduleSet() {
  DebugPrint("No schedule set...");
  xSemaphoreGive(showTimerSemaphore);
}
void reanudeCycle() { xSemaphoreGive(showTimerSemaphore); }
void pauseCycle() {
  xSemaphoreTake(showTimerSemaphore, 0);
  writeLCD("Cycle paused\nPress button to resume...");
}
void processMessage() {
  StaticJsonDocument<JSON_DOC_SIZE> doc;

  json_queue_dequeue(&messagesQueue, doc);

  if (doc.containsKey("volume")) {
    long value = doc["volume"]["value"];

    if (value >= 0 && value <= 100) {
      setVolumeBuzzer(value);
    }
  } else if (doc.containsKey("buzzer")) {
    long value = doc["buzzer"]["value"];

    switch (value) {
    case 0:
      stopBuzzer();
      break;
    case 1:
      startBuzzer();
      break;
    default:
      Serial.print("Buzzer value not recognized");
    }
  }
}