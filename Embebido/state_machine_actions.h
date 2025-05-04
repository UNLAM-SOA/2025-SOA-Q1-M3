#include "setup_utils.h"

typedef void (*action)();

// Actions
void initialize()
{

 // Lee archivo con horarios y dias
 // Calcula el proximo horario y dia
 // Pasa al estado awaiting reminder time

 fisicalSetup();
 attachInterrupt(LIMIT_SWITCH_MOVIL, detectMovingLimitSwitch, RISING);
 setupWifi();
 setupTime();
 createNewScheduledTimer();
 timeEventsQueue = xQueueCreate(MAX_EVENTS_QUEUE, sizeof(events));
}

void noScheduleSet();
void settingSchedule();
void awaitingTimer();
void moving();
void scanning();
void pillDetected();
void noPillDetected();
void doseSkipped();
void returnToHome();
void error();
void none();

void error()
{
}
void none()
{
}