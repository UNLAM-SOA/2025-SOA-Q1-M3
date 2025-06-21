#pragma once

#include "Drivers/Queue.h"
#include "fisical.h"
#include "freeRTOS_Objects.h"
#include "Drivers/MQTT_Driver.h"

#define MAX_EVENTS 36

#define MAX_TYPE_EVENTS 7
#define INVERSE_PRESENCE_SENSOR 1 // 0-->Hay pastilla, 1-->No hay pastilla

#define MAX_DAYS 7
#define MAX_PILLS_PER_DAY 3
#define MAX_PERIODS (MAX_PILLS_PER_DAY * MAX_DAYS)
#define PRECENSE_THRESHOLD 100 // Valor de umbral para detectar la presencia de pastillas
#define PONTECIOMETER_THRESHOLD 5
#define NO_PILL_TOOKING -1
#define LONG_PRESS_TIME 500 // Tiempo de presión larga en milisegundos

#define ENABLE_PERIODICAL_TIME_EVENTS 1   // Para testear: Habilitar eventos de tiempo periódicos (0: deshabilitado, 1: habilitado)
#define PERIODICAL_TIME_EVENTS_TIME 50000 // Para testear: Tiempo en milisegundos entre eventos de tiempo periódicos
enum events
{
 EV_TIME_SUNDAY_MORNING,
 EV_TIME_SUNDAY_AFTERNOON,
 EV_TIME_SUNDAY_NIGHT,
 EV_TIME_MONDAY_MORNING,
 EV_TIME_MONDAY_AFTERNOON,
 EV_TIME_MONDAY_NIGHT,
 EV_TIME_TUESDAY_MORNING,
 EV_TIME_TUESDAY_AFTERNOON,
 EV_TIME_TUESDAY_NIGHT,
 EV_TIME_WEDNESDAY_MORNING,
 EV_TIME_WEDNESDAY_AFTERNOON,
 EV_TIME_WEDNESDAY_NIGHT,
 EV_TIME_THURSDAY_MORNING,
 EV_TIME_THURSDAY_AFTERNOON,
 EV_TIME_THURSDAY_NIGHT,
 EV_TIME_FRIDAY_MORNING,
 EV_TIME_FRIDAY_AFTERNOON,
 EV_TIME_FRIDAY_NIGHT,
 EV_TIME_SATURDAY_MORNING,
 EV_TIME_SATURDAY_AFTERNOON,
 EV_TIME_SATURDAY_NIGHT,
 EV_BUTTON_1_TAP,
 EV_BUTTON_2_TAP,
 EV_BUTTON_3_TAP,
 EV_BUTTON_1_LONG_PRESS,
 EV_BUTTON_2_LONG_PRESS,
 EV_BUTTON_3_LONG_PRESS,
 EV_POT_INCREASED,
 EV_POT_DECREASED,
 EV_MESSAGE_RECEIVED,
 EV_LIMIT_SWITCH_MOVING,
 EV_LIMIT_SWITCH_START,
 EV_PILL_DETECTED,
 EV_PILL_NOT_DETECTED,
 EV_SCAN_ALL,
 EV_CONT,
} new_event = EV_CONT;

String events_s[] = {
    // Los primeros 21 eventos corresponden a los generados al alcanzar cierto dia y horario
    "EV_TIME_SUNDAY_MORNING",
    "EV_TIME_SUNDAY_AFTERNOON",
    "EV_TIME_SUNDAY_NIGHT",
    "EV_TIME_MONDAY_MORNING",
    "EV_TIME_MONDAY_AFTERNOON",
    "EV_TIME_MONDAY_NIGHT",
    "EV_TIME_TUESDAY_MORNING",
    "EV_TIME_TUESDAY_AFTERNOON",
    "EV_TIME_TUESDAY_NIGHT",
    "EV_TIME_WEDNESDAY_MORNING",
    "EV_TIME_WEDNESDAY_AFTERNOON",
    "EV_TIME_WEDNESDAY_NIGHT",
    "EV_TIME_THURSDAY_MORNING",
    "EV_TIME_THURSDAY_AFTERNOON",
    "EV_TIME_THURSDAY_NIGHT",
    "EV_TIME_FRIDAY_MORNING",
    "EV_TIME_FRIDAY_AFTERNOON",
    "EV_TIME_FRIDAY_NIGHT",
    "EV_TIME_SATURDAY_MORNING",
    "EV_TIME_SATURDAY_AFTERNOON",
    "EV_TIME_SATURDAY_NIGHT",
    "EV_BUTTON_1_TAP",
    "EV_BUTTON_2_TAP",
    "EV_BUTTON_3_TAP",
    "EV_BUTTON_1_LONG_PRESS",
    "EV_BUTTON_2_LONG_PRESS",
    "EV_BUTTON_3_LONG_PRESS",
    "EV_POT_INCREASED",
    "EV_POT_DECREASED",
    "EV_MESSAGE_RECEIVED",
    "EV_LIMIT_SWITCH_MOVING",
    "EV_LIMIT_SWITCH_START",
    "EV_PILL_DETECTED",
    "EV_PILL_NOT_DETECTED",
    "EV_SCAN_ALL",
    "EV_CONT",
};

bool time_sensor();
bool button_1_sensor();
bool button_2_sensor();
bool button_3_sensor();
bool limit_switch_moving_sensor();
bool presence_sensor();
bool potentiometer_sensor();

//----------------------------------------------
// The setDayAndPeriod function calculates and sets the objectiveDay and objectivePeriod based on the value of new_event. If new_event exceeds the MAX_PERIODS threshold, it resets both values to -1; otherwise, it determines the day and period using division and modulo operations with MAX_PILLS_PER_DAY.
void setDayAndPeriod();

typedef bool (*eventType)();
eventType event_type[MAX_TYPE_EVENTS] = {time_sensor, button_1_sensor, button_2_sensor, button_3_sensor, limit_switch_moving_sensor, presence_sensor, potentiometer_sensor};

short objetiveDay = NO_PILL_TOOKING;
short objetivePeriod = NO_PILL_TOOKING;

bool movingForward = true; // It starts moving forward

short (*presenceSensorsArray[MAX_PILLS_PER_DAY])() = {readPresenceSensor_TM, readPresenceSensor_TT, readPresenceSensor_TN};
short limitSwitchPassed = 0; // How many limit switches have been passed
long lct_time = 0;           // Last cycle time

bool time_sensor()
{
 if (ENABLE_PERIODICAL_TIME_EVENTS) // If periodic time events are enabled
 {
  if ((millis() - lct_time > PERIODICAL_TIME_EVENTS_TIME)) // If the time since the last event is greater than the defined time
  {
   lct_time = millis();                // Update the last cycle time
   new_event = EV_TIME_THURSDAY_NIGHT; // Set the event to continue
   return true;
  }
 }
 int queueValue;
 if (timeEventsQueue != NULL && xQueueReceive(timeEventsQueue, &queueValue, 0) == pdTRUE) // If there is a value in the queue
 {
  new_event = (events)queueValue;
  return true;
 }
 return false;
}
long ctStartPressed = LOW;
short previousButtonState = LOW;
unsigned long lastButtonPressed = LOW;
short buttonAmountPressed = 0;
bool button_1_sensor()
{
 unsigned long currentTime = micros();                                                                               // Get the current time
 if (!xQueuePeek(buttonEventsQueue, &ctStartPressed, 0) && currentTime - lastButtonPressed > LONG_PRESS_TIME * 1000) // Peek the previous button state from the queue
 {
  if (buttonAmountPressed == 1)
   new_event = EV_BUTTON_1_TAP;
  else if (buttonAmountPressed == 2)
   new_event = EV_BUTTON_1_LONG_PRESS;
  else if (buttonAmountPressed >= 3)
   new_event = EV_SCAN_ALL;
  lastButtonPressed = LOW;
  buttonAmountPressed = 0;
  return true;
 }
 if (xQueueReceive(buttonEventsQueue, &lastButtonPressed, 0) == pdTRUE)
  buttonAmountPressed++;

 if (currentTime - lastButtonPressed < LONG_PRESS_TIME * 1000)
 {
  return false;
 }
}
bool button_2_sensor()
{
 // TODO: Implementar la función para detectar el botón 2
 return false;
}
bool button_3_sensor()
{
 // TODO: Implementar la función para detectar el botón 3
 return false;
}
bool limit_switch_moving_sensor()
{
 //Serial.println(String("Limit switch passed: ") + String(limitSwitchPassed));
 if (xSemaphoreTake(scanningCompletedSemaphore, 0) == pdTRUE) // Si se puede tomar el semáforo, se ha alcanzado el final del recorrido
 {
  new_event = EV_LIMIT_SWITCH_START;
  return true; // Se ha alcanzado el final del recorrido
 }
 if (objetiveDay == NO_PILL_TOOKING) // Si no hay un ciclo de recordatorio activo, no se detecta el interruptor de límite en movimiento
  return false;
 if (limitSwitchPassed == objetiveDay) // Si el número de interruptores de límite pasados es igual al día objetivo, se ha alcanzado el final del recorrido
 {
  limitSwitchPassed = -limitSwitchPassed; // Reiniciar el contador de interruptores de límite pasados
  new_event = EV_LIMIT_SWITCH_MOVING;
  movingForward = false; // Establecer el evento de interruptor de límite en movimiento
  return true;           // Se ha alcanzado el final del recorrido
 }

 // Alcanza el principio
 if (limitSwitchPassed == LOW && !movingForward)
 {
  new_event = EV_LIMIT_SWITCH_START;
  return true;
 }
 return false;
}

bool potentiometer_sensor()
{
 long potentiometerNewValue = readPotentiometer();

 if (potentiometerNewValue > (potentiometerLastValue + PONTECIOMETER_THRESHOLD))
 {
  potentiometerLastValue = potentiometerNewValue;
  new_event = EV_POT_INCREASED;
  return true;
 }

 if (potentiometerNewValue < (potentiometerLastValue - PONTECIOMETER_THRESHOLD))
 {
  potentiometerLastValue = potentiometerNewValue;
  new_event = EV_POT_DECREASED;
  return true;
 }

 return false;
}

bool message_sensor()
{
 if (!json_queue_is_empty(&messagesQueue))
 {

  StaticJsonDocument<JSON_DOC_SIZE> doc;

  json_queue_peek(&messagesQueue, doc);

  if (doc.containsKey("context"))
  {
   JsonObject context = doc["context"];
   String type = context["type"];
   if (type == "scan")
   {
    json_queue_dequeue(&messagesQueue, doc);
    new_event = EV_SCAN_ALL;
    return true;
   }
  }

  new_event = EV_MESSAGE_RECEIVED;
  return true;
 }
 return false;
}

bool presence_sensor()
{
 if (objetivePeriod == NO_PILL_TOOKING) // Si no hay un ciclo de recordatorio activo, no se detecta la presencia de pastillas
  return false;

 short value = presenceSensorsArray[objetivePeriod]();
 new_event = INVERSE_PRESENCE_SENSOR ? ((value > PRECENSE_THRESHOLD) ? EV_PILL_DETECTED : EV_PILL_NOT_DETECTED)
                                     : ((value < PRECENSE_THRESHOLD) ? EV_PILL_DETECTED
                                                                     : EV_PILL_NOT_DETECTED);

 return true;
}

void setDayAndPeriod()
{
 if (new_event >= MAX_PERIODS)
 {
  return;
 }
 objetiveDay = ((new_event / MAX_PILLS_PER_DAY) * 2) + 4;
 objetivePeriod = new_event % MAX_PILLS_PER_DAY;
}
