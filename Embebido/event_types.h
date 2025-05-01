#define MAX_EVENTS 30
#define MAX_TYPE_EVENTS 6

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
    "EV_LIMIT_SWITCH_START"};

bool time_sensor();
bool button_1_sensor();
bool button_2_sensor();
bool button_3_sensor();
bool limit_switch_moving_sensor();
bool limit_switch_start_sensor();
typedef bool (*eventType)();
eventType event_type[MAX_TYPE_EVENTS] = {time_sensor, button_1_sensor, button_2_sensor, button_3_sensor, limit_switch_moving_sensor, limit_switch_start_sensor};

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
