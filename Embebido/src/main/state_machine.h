#pragma once
#include "debug.h"
#include "event_types.h"
#include "state_machine_actions.h"
#include "./Drivers/MQTT_Driver.h"

#define MAX_STATES 8

#define TIME_DIFF_BETWEEN_EXEC_CYCLES 50

short last_index_type_sensor = 0;
long lct = 0;

void (*funcReset)(void) = 0;

enum states
{
 ST_INIT,
 ST_REMINDER_CYCLE_ON_HOLD,
 ST_AWAITING_REMINDER_TIME,
 ST_MOVING_TO_PILL_POS,
 ST_SCANNING_AT_PILL_POS,
 ST_PILLS_DETECTED,
 ST_NO_PILLS_DETECTED,
 ST_RETURNING_TO_NEUTRAL,
} current_state;

String states_s[MAX_STATES] = {
    "ST_INIT",
    "ST_REMINDER_CYCLE_ON_HOLD",
    "ST_AWAITING_REMINDER_TIME",
    "ST_MOVING_TO_PILL_POS",
    "ST_SCANNING_AT_PILL_POS",
    "ST_PILLS_DETECTED",
    "ST_NO_PILLS_DETECTED",
    "ST_RETURNING_TO_NEUTRAL",
};

action state_table_action[MAX_STATES][MAX_EVENTS] = {
    {none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               modifyVolume,      modifyVolume,      none,               none,               none,               none,               awaitingTimer},                                        /*ST_INIT*/
    {none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               reanudeCycle,       none,               none,               none,               none,               none,               modifyVolume,      modifyVolume,      none,               none,               none,               none,               none},                                         /*ST_REMINDER_CYCLE_ON_HOLD*/
    {moving,             moving,             moving,             moving,             moving,             moving,             moving,             moving,             moving,             moving,             moving,             moving,             moving,             moving,             moving,             moving,             moving,             moving,             moving,             moving,             moving,             none,               none,               none,               pauseCycle,         none,               none,               modifyVolume,      modifyVolume,      none,               none,               none,               none,               none}, /*ST_AWAITING_REMINDER_TIME*/
    {none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               modifyVolume,      modifyVolume,      scanning,           none,               none,               none,               none},                                             /*ST_MOVING_TO_PILL_POS*/
    {none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               modifyVolume,      modifyVolume,      none,               none,               pillDetected,       noPillDetected,     none},                               /*ST_SCANNING_AT_PILL_POS*/
    {none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               doseSkipped,        none,               none,               modifyVolume,      modifyVolume,      none,               none,               none,               doseTaken,          none},                                     /*ST_PILLS_DETECTED*/
    {none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               scanning,           none,               none,               doseSkipped,        none,               none,               modifyVolume,      modifyVolume,      none,               none,               none,               none,               none},                                      /*ST_NO_PILLS_DETECTED*/
    {none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               none,               modifyVolume,      modifyVolume,      none,               stopReturning,      none,               none,               none}                                         /*ST_RETURNING_TO_NEUTRAL*/
};

states state_table_next_state[MAX_STATES][MAX_EVENTS] = {
    {ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_INIT,                  ST_AWAITING_REMINDER_TIME},                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   /*ST_INIT*/
    {ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_AWAITING_REMINDER_TIME,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD,ST_REMINDER_CYCLE_ON_HOLD}, /*ST_REMINDER_CYCLE_ON_HOLD*/
    {ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_AWAITING_REMINDER_TIME,ST_AWAITING_REMINDER_TIME,ST_AWAITING_REMINDER_TIME,ST_REMINDER_CYCLE_ON_HOLD,ST_AWAITING_REMINDER_TIME,ST_AWAITING_REMINDER_TIME,ST_AWAITING_REMINDER_TIME,ST_AWAITING_REMINDER_TIME,ST_AWAITING_REMINDER_TIME,ST_AWAITING_REMINDER_TIME,ST_AWAITING_REMINDER_TIME,ST_AWAITING_REMINDER_TIME,ST_AWAITING_REMINDER_TIME},                                                                                     /*ST_AWAITING_REMINDER_TIME*/
    {ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_SCANNING_AT_PILL_POS,  ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS,    ST_MOVING_TO_PILL_POS},                                                                                                                                       /*ST_MOVING_TO_PILL_POS*/
    {ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_SCANNING_AT_PILL_POS,  ST_PILLS_DETECTED,        ST_NO_PILLS_DETECTED,     ST_SCANNING_AT_PILL_POS},                                                                              /*ST_SCANNING_AT_PILL_POS*/
    {ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_RETURNING_TO_NEUTRAL,  ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_PILLS_DETECTED,        ST_RETURNING_TO_NEUTRAL,  ST_PILLS_DETECTED},                                                                                                                                                                                                                                                                     /*ST_PILLS_DETECTED*/
    {ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_SCANNING_AT_PILL_POS,  ST_NO_PILLS_DETECTED,     ST_SCANNING_AT_PILL_POS,  ST_RETURNING_TO_NEUTRAL,  ST_NO_PILLS_DETECTED,     ST_PILLS_DETECTED,        ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED,     ST_NO_PILLS_DETECTED},                                                                                                                                                                     /*ST_NO_PILLS_DETECTED*/
    {ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_AWAITING_REMINDER_TIME,ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL,  ST_RETURNING_TO_NEUTRAL}                                                                    /*ST_RETURNING_TO_NEUTRAL*/
};
events last_event = EV_BUTTON_3_LONG_PRESS;
states last_state = ST_RETURNING_TO_NEUTRAL;
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

    if(!client.connected()) {
        mqtt_reconnect();
    }

    client.loop();

 get_new_event();
 if ((new_event >= 0) && (new_event < MAX_EVENTS) && (current_state >= 0) && (current_state < MAX_STATES))
 {
  if (last_event != new_event || last_state != current_state)
  {
   if (new_event != EV_CONT)
   {
    DebugPrintEstado(states_s[current_state], events_s[new_event]);
   }

   last_state = current_state;
   last_event = new_event;
   state_table_action[current_state][new_event]();
   current_state = state_table_next_state[current_state][new_event];
  }
 }
 else
 {
  DebugPrint("Error: Estado o evento fuera de rango." + String(current_state) + " " + String(new_event));
 }
}
