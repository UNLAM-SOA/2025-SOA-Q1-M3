
#include "setup_utils.h"

typedef void (*action)();

void showHourTimerLCD(void *)
{
  short displayingCurrentTime = 1;
  while (1)
  {
    if (uxSemaphoreGetCount(showTimerSemaphore) > 0)
    {
      getLocalTime(&timeinfo, 10); // Actualiza la hora
      // Semáforo disponible (no lo toma)
      displayingCurrentTime == 1 ? writeLCD("Hora: " + String(timeinfo.tm_hour) + ":" + String(timeinfo.tm_min)) : writeLCD("Proxima toma: " + String(schedule[nextPeriod].tm_hour) + ":" + String(schedule[nextPeriod].tm_min));
      displayingCurrentTime *= -1; // Cambia el estado de la variable
      Serial.println("Show hour timer LCD");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    else
    {
      // Semáforo ocupado: tomar con bloqueo
      xSemaphoreTake(showTimerSemaphore, portMAX_DELAY);
    }
  }
}
// Actions
void initialize()
{

  int scheduleSetup[MAX_DAYS][MAX_PILLS_PER_DAY] = {
    {6, 14, 21}, // Domingo Mañana, Domingo Tarde, Domingo Noche
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
  showTimerSemaphore = xSemaphoreCreateMutex();
  xTaskCreate(showHourTimerLCD, "showHourTimerLCD", 1024, NULL, 1, NULL);
}

void noScheduleSet();
void settingSchedule();
void awaitingTimer();
void moving()
{
  startMotorRight();
};
void scanning();
void pillDetected();
void noPillDetected();
void doseSkipped();
void returnToHome()
{
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

void awaitingTimer()
{
  Serial.println("Awaiting timer...");
  xSemaphoreGive(showTimerSemaphore);
}
void moving()
{
  Serial.println("Moving...");
  xSemaphoreTake(showTimerSemaphore, 0);
  setDayAndPeriod();
}