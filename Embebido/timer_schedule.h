#pragma once
#include "time.h"
#include "event_types.h"
void searchNextSchedule(tm *timeinfo);
int timeUntilNextSchedule(tm *timeinfo, tm *schedule);

int nextPeriod = -1;
tm schedule[MAX_PERIODS]; 

bool isScheduleAvailable(tm *scheduleRecord)
{
 return scheduleRecord->tm_yday != -1;
}
void setScheduleUnnavailable(tm *schedulePos)
{
 schedule->tm_yday = -1; // Set the schedule as unavailable
}
void setScheduleAvailable(tm *schedulePos)
{
 schedule->tm_yday = 0; // Set the schedule as available
}

int firstScheduleAvailable()
{
 for (int i = 0; i < MAX_PERIODS; i++)
 {
  if (isScheduleAvailable(&schedule[i]))
   return i;
 }
 return -1;
}
void searchNextSchedule(tm *timeinfo)
{

 int currentWeekDay = timeinfo->tm_wday; // Día actual de la semana (0-6, donde 0 es domingo)
 int currentHour = timeinfo->tm_hour;    // Hora actual (0-23)
 int currentMinute = timeinfo->tm_min;   // Minuto actual (0-59)

 for (int i = 0; i < MAX_PERIODS; i++)
 {
  if (isScheduleAvailable(&schedule[i]) && schedule[i].tm_wday >= currentWeekDay && (schedule[i].tm_hour > currentHour || (schedule[i].tm_hour == currentHour && schedule[i].tm_min > currentMinute)))
  {
   nextPeriod = i;
   return;
  }
 }
 nextPeriod = firstScheduleAvailable();
 Serial.print("nextPeriod: ");
 Serial.println(nextPeriod);
}

int timeUntilNextSchedule(tm *timeinfo, tm *schedule)
{
 int currentHour = timeinfo->tm_hour;  // Hora actual (0-23)
 int currentMinute = timeinfo->tm_min; // Minuto actual (0-59)
 int currentSecond = timeinfo->tm_sec; // Segundo actual (0-59)
 int currentMillisecond = 0;           // Milisegundos actuales (asumimos 0 si no se tiene precisión)

 int scheduleHour = schedule->tm_hour;  // Hora programada (0-23)
 int scheduleMinute = schedule->tm_min; // Minuto programado (0-59)
 int scheduleSecond = schedule->tm_sec; // Segundo programado (0-59)
 int scheduleMillisecond = 0;           // Milisegundos programados (asumimos 0 si no se tiene precisión)

 int millisecondsUntilNextSchedule = ((scheduleHour - currentHour) * 3600000) +
                                     ((scheduleMinute - currentMinute) * 60000) +
                                     ((scheduleSecond - currentSecond) * 1000) +
                                     (scheduleMillisecond - currentMillisecond);
 if (millisecondsUntilNextSchedule < 0)
 {
  millisecondsUntilNextSchedule += 24 * 3600000; // Agregar un día en milisegundos si el horario programado ya pasó
 }
 return millisecondsUntilNextSchedule;
}

void setupSchedule(int[][] scheduleSetup) 
{
 for(int i=0; i<MAX_DAYS; i++){
  for(int j=0; j<MAX_PILLS_PER_DAY; j++){
    if(scheduleSetup[i][j] != 0)
    {
      schedule[i+j] = (struct tm) {
        .tm_hour = scheduleSetup[i][j];
        .tm_wday = i;
        .tm_isdst = 0;
      };
    } else 
    {
      schedule[i+j] = (struct tm) {
        .tm_isdst = -1;
      };
    }
  }
 }
}
