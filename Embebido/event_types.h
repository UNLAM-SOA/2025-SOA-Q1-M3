#include "fisical.h"

#define MAX_EVENTS 32
#define MAX_TYPE_EVENTS 7
#define MAX_PRESENCE_SENSORS 3
#define MAX_DAYS 7
#define MAX_PERIODS (MAX_PRESENCE_SENSORS * MAX_DAYS)
#define PRECENSE_THRESHOLD 100 // Valor de umbral para detectar la presencia de pastillas

enum events
{
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
 EV_TIME_SUNDAY_MORNING,
 EV_TIME_SUNDAY_AFTERNOON,
 EV_TIME_SUNDAY_NIGHT,
 EV_BUTTON_1_TAP,
 EV_BUTTON_2_TAP,
 EV_BUTTON_3_TAP,
 EV_BUTTON_1_LONG_PRESS,
 EV_BUTTON_2_LONG_PRESS,
 EV_BUTTON_3_LONG_PRESS,
 EV_LIMIT_SWITCH_MOVING,
 EV_LIMIT_SWITCH_START,
 EV_PILL_DETECTED,
 EV_PILL_NOT_DETECTED,
 EV_CONT,
} new_event;

String events_s[] = {
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
    "EV_TIME_SUNDAY_MORNING",
    "EV_TIME_SUNDAY_AFTERNOON",
    "EV_TIME_SUNDAY_NIGHT",
    "EV_BUTTON_1_TAP",
    "EV_BUTTON_2_TAP",
    "EV_BUTTON_3_TAP",
    "EV_BUTTON_1_LONG_PRESS",
    "EV_BUTTON_2_LONG_PRESS",
    "EV_BUTTON_3_LONG_PRESS",
    "EV_LIMIT_SWITCH_MOVING",
    "EV_LIMIT_SWITCH_START",
    "EV_PILL_DETECTED",
    "EV_PILL_NOT_DETECTED",
    "EV_CONT",
};

bool time_sensor();
bool button_1_sensor();
bool button_2_sensor();
bool button_3_sensor();
bool limit_switch_moving_sensor();
bool limit_switch_start_sensor();
bool presence_sensor();

//----------------------------------------------
// The setDayAndPeriod function calculates and sets the objectiveDay and objectivePeriod based on the value of new_event. If new_event exceeds the MAX_PERIODS threshold, it resets both values to -1; otherwise, it determines the day and period using division and modulo operations with MAX_PRESENCE_SENSORS.
void setDayAndPeriod();

typedef bool (*eventType)();
eventType event_type[MAX_TYPE_EVENTS] = {time_sensor, button_1_sensor, button_2_sensor, button_3_sensor, limit_switch_moving_sensor, limit_switch_start_sensor, presence_sensor};

short objetiveDay = -1;
short objetivePeriod = -1;

const short presenceSensorsArray[MAX_PRESENCE_SENSORS] = {PRESENCE_PIN_1, PRESENCE_PIN_2, PRESENCE_PIN_3};

bool time_sensor()
{
 // TODO: Implementar la función para detectar el tiempo
 return false;
}
bool button_1_sensor()
{
 // TODO: Implementar la función para detectar el botón 1
 return false;
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
 // TODO: Implementar la función para detectar el interruptor de límite en movimiento
 return false;
}
bool limit_switch_start_sensor()
{
 // TODO: Implementar la función para detectar el interruptor de límite de inicio
 return false;
}
bool presence_sensor()
{
 if (objetivePeriod == -1) // Si no hay un ciclo de recordatorio activo, no se detecta la presencia de pastillas
  return false;

 short value = readPresenceSensor(presenceSensorsArray[objetivePeriod]);
 new_event = (value > PRECENSE_THRESHOLD) ? EV_PILL_DETECTED : EV_PILL_NOT_DETECTED;
 return true;
}

void setDayAndPeriod()
{
 if (new_event >= MAX_PERIODS)
 {
  objetiveDay = -1;
  objetivePeriod = -1;
  return;
 }
 objetiveDay = new_event / MAX_PRESENCE_SENSORS;
 objetivePeriod = new_event % MAX_PRESENCE_SENSORS;
}