// Enable debug states prints
//----------------------------------------------
#define SERIAL_DEBUG_ENABLED 1

#if SERIAL_DEBUG_ENABLED
#define DebugPrint(str) \
 {                      \
  Serial.println(str);  \
 }
#else
#define DebugPrint(str)
#endif

#define DebugPrintEstado(estado, evento)                         \
 {                                                               \
  String est = estado;                                           \
  String evt = evento;                                           \
  String str;                                                    \
  str = "-----------------------------------------------------"; \
  DebugPrint(str);                                               \
  str = "EST-> [" + est + "]: " + "EVT-> [" + evt + "].";        \
  DebugPrint(str);                                               \
  str = "-----------------------------------------------------"; \
  DebugPrint(str);                                               \
 }
//----------------------------------------------

#include "event_types.h"
#include "state_machine_actions.h"
#define MAX_STATES 14
#define TIME_DIFF_BETWEEN_EXEC_CYCLES 50 // 50 milisegundos

short last_index_type_sensor = 0;
long lct = 0;

void (*funcReset)(void) = 0;

enum states
{
 ST_DUMMY,
 ST_INIT,
 ST_NO_SCHEDULE_SET,
 ST_SETTING_SCHEDULE,
 ST_AWAITING_REMINDER_TIME,
 ST_MOVING_TO_PILL_POS,
 ST_SCANNING_AT_PILL_POS,
 ST_PILLS_DETECTED,
 ST_NO_PILLS_DETECTED,
 ST_DOSE_TAKEN,
 ST_DOSE_SKIPPED,
 ST_RETURNING_TO_NEUTRAL,
 ST_REMINDER_CYCLE_CANCELLED,
 ST_REMINDER_CYCLE_ON_HOLD,
 ST_REMINDER_CYCLE_COMPLETED
} current_state;

String states_s[MAX_STATES] = {
    "ST_DUMMY",
    "ST_INIT",
    "ST_NO_SCHEDULE_SET",
    "ST_SETTING_SCHEDULE",
    "ST_AWAITING_REMINDER_TIME",
    "ST_MOVING_TO_PILL_POS",
    "ST_SCANNING_AT_PILL_POS",
    "ST_PILLS_DETECTED",
    "ST_NO_PILLS_DETECTED",
    "ST_DOSE_TAKEN",
    "ST_DOSE_SKIPPED",
    "ST_RETURNING_TO_NEUTRAL",
    "ST_REMINDER_CYCLE_CANCELLED",
    "ST_REMINDER_CYCLE_ON_HOLD",
    "ST_REMINDER_CYCLE_COMPLETED",
};

action state_table_action[MAX_STATES][MAX_EVENTS] = {
    {none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none},  /*ST_INIT*/
    {none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none},  /*ST_NO_SCHEDULE_SET*/
    {none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none},  /*ST_SETTING_SCHEDULE*/
    {none, error, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none}, /*ST_AWAITING_REMINDER_TIME*/
    {none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none},  /*ST_MOVING_TO_PILL_POS*/
    {none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none},  /*ST_SCANNING_AT_PILL_POS*/
    {none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none},  /*ST_PILLS_DETECTED*/
    {none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none},  /*ST_NO_PILLS_DETECTED*/
    {none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none},  /*ST_DOSE_TAKEN*/
    {none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none},  /*ST_DOSE_SKIPPED*/
    {none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none},  /*ST_RETURNING_TO_NEUTRAL*/
    {none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none},  /*ST_REMINDER_CYCLE_CANCELLED*/
    {none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none},  /*ST_REMINDER_CYCLE_ON_HOLD*/
    {none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none, none}   /*ST_REMINDER_CYCLE_COMPLETED*/
};

states state_table_next_state[MAX_STATES][MAX_EVENTS] = {
    {ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY, ST_DUMMY},                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              /*ST_DUMMY*/
    {ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_INIT, ST_NO_SCHEDULE_SET},                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   /*ST_INIT*/
    {ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_SETTING_SCHEDULE, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET, ST_NO_SCHEDULE_SET},                                                                                                                                                                                                                                                                                             /*ST_NO_SCHEDULE_SET*/
    {ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_AWAITING_REMINDER_TIME, ST_NO_SCHEDULE_SET, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE, ST_SETTING_SCHEDULE},                                                                                                                                                                                                                                                         /*ST_SETTING_SCHEDULE*/
    {ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_REMINDER_CYCLE_ON_HOLD, ST_AWAITING_REMINDER_TIME, ST_AWAITING_REMINDER_TIME, ST_REMINDER_CYCLE_CANCELLED, ST_AWAITING_REMINDER_TIME, ST_AWAITING_REMINDER_TIME, ST_AWAITING_REMINDER_TIME, ST_AWAITING_REMINDER_TIME, ST_AWAITING_REMINDER_TIME, ST_AWAITING_REMINDER_TIME, ST_AWAITING_REMINDER_TIME},                                                                                                                                                /*ST_AWAITING_REMINDER_TIME*/
    {ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS, ST_MOVING_TO_PILL_POS},                                                                                                                                                                                            /*ST_MOVING_TO_PILL_POS*/
    {ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_SCANNING_AT_PILL_POS, ST_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_SCANNING_AT_PILL_POS},                                                                                                                                       /*ST_SCANNING_AT_PILL_POS*/
    {ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_DOSE_TAKEN, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_DOSE_SKIPPED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED, ST_PILLS_DETECTED},                                                                                                                                                                                                                                                                                                                                    /*ST_PILLS_DETECTED*/
    {ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_SCANNING_AT_PILL_POS, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_DOSE_SKIPPED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED, ST_NO_PILLS_DETECTED},                                                                                                                                                                                                                                /*ST_NO_PILLS_DETECTED*/
    {ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_DOSE_TAKEN, ST_RETURNING_TO_NEUTRAL},                                                                                                                                                                                                                                                                                                                                                                                                                                                    /*ST_DOSE_TAKEN*/
    {ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_DOSE_SKIPPED, ST_RETURNING_TO_NEUTRAL},                                                                                                                                                                                                                                                                                                                                                                                      /*ST_DOSE_SKIPPED*/
    {ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_AWAITING_REMINDER_TIME, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL, ST_RETURNING_TO_NEUTRAL},                                                                                                                            /*ST_RETURNING_TO_NEUTRAL*/
    {ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_REMINDER_CYCLE_CANCELLED, ST_NO_SCHEDULE_SET},       /*ST_REMINDER_CYCLE_CANCELLED*/
    {ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_AWAITING_REMINDER_TIME, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD, ST_REMINDER_CYCLE_ON_HOLD},                                                              /*ST_REMINDER_CYCLE_ON_HOLD*/
    {ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_AWAITING_REMINDER_TIME, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED, ST_REMINDER_CYCLE_COMPLETED} /*ST_REMINDER_CYCLE_COMPLETED*/
};

void get_new_event()
{
 short index = 0;
 long ct = millis();
 long diff = (ct - lct);
 bool timeout = (diff > TIME_DIFF_BETWEEN_EXEC_CYCLES) ? (true) : (false);

 if (timeout)
 {
  timeout = false;
  lct = ct;

  index = (last_index_type_sensor % MAX_TYPE_EVENTS);

  last_index_type_sensor++;

  if (event_type[index]())
  {
   return;
  }
 }
 new_event = EV_CONT;
}

void state_machine()
{
 get_new_event();

 if ((new_event >= 0) && (new_event < MAX_EVENTS) && (current_state >= 0) && (current_state < MAX_STATES))
 {
  if (new_event != EV_CONT)
  {
   DebugPrintEstado(states_s[current_state], events_s[new_event]);
  }

  setDayAndPeriod();

  state_table_actions[current_state][new_event]();
  current_state = state_table_next_state[current_state][new_event];
 }
 else
 {
  DebugPrint("Error: Estado o evento fuera de rango." + String(current_state) + " " + String(new_event));
 }
}
