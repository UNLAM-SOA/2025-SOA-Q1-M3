#include "event_types.h"

#define MAX_STATES 6
#define TIME_DIFF_BETWEEN_EXEC_CYCLES 50 // 50 milisegundos

short last_index_type_sensor = 0;
long lct = 0;

void (*funcReset)(void) = 0;

typedef void (*transition)();

// Actions
void initialize();
void noScheduleSet();
void settingSchedule();
void awaitingTimer();
void moving();
void scanning();
void pillDetected();
void noPillDetected();
void doseSkipped();
void returnToHome();

enum states
{
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

String states_s[] = {
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
    "ST_REMINDER_CYCLE_COMPLETED"};

enum states last_state;
enum events last_event;

transition state_table_actions[MAX_STATES][MAX_EVENTS] =
    {
        {},
        {},
        {},
};

states state_table_next_state[MAX_STATES][MAX_EVENTS] =
    {
        {},
        {},
        {},
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

  state_table_actions[current_state][new_event]();
  current_state = state_table_next_state[current_state][new_event];

  last_event = new_event;
  last_state = current_state;
 }
}
