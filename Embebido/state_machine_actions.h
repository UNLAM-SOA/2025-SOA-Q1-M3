#include "setup_utils.h"

typedef void (*action)();

// Actions
void initialize()
{

   int scheduleSetup[MAX_DAYS][MAX_PILLS_PER_DAY] = {
        {6,14,21}, // Domingo Mañana, Domingo Tarde, Domingo Noche
        {6, 0, 0},
        {6, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    }

 // Lee archivo con horarios y dias
 // Calcula el proximo horario y dia
 // Pasa al estado awaiting reminder time
 fisicalSetup();
 attachInterrupt(LIMIT_SWITCH_MOVIL, detectMovingLimitSwitch, RISING);
 setupWifi();
 setupTime();

 setupSchedule(scheduleSetup);

 createNewScheduledTimer();
 timeEventsQueue = xQueueCreate(MAX_EVENTS_QUEUE, sizeof(events));
 xTaskCreate(testTimer, "testTimer", 2048, &nextPeriod, 1, &eventTaskHandle);

}

void noScheduleSet();
void settingSchedule();
void awaitingTimer();
void moving() {
    startMotorRight();
};
void scanning();
void pillDetected();
void noPillDetected();
void doseSkipped();
void returnToHome(){
    startMotorLeft();
};
void error();
void none();

void error()
{
}
void none()
{
}