
#include "timer_schedule.h"
#include "setup_utils.h"
#include "fisical.h"
#include "freeRTOS_Objects.h"
#include "./Drivers/MQTT_Driver.h"

#define GET_TIME_TIMEOUT 10

#define LCD_BLINK_TIME 5000
#define NOTIFICATION_FRECUENCY_ALERT 1000
#define NOTIFICATION_UNNAVAILABLE_ALERT 500

long last_time = 0;

void (*setLeds[MAX_PILLS_PER_DAY])(short) = {setLedPresence_TM, setLedPresence_TT, setLedPresence_TN};

bool isBelowTime(int frecuency)
{
  return (millis() % (frecuency * 2)) < frecuency;
}

void showHourTimerLCDCallback(void *)
{
  getLocalTime(&timeinfo, GET_TIME_TIMEOUT); // Actualiza la hora
  char mensaje[LCD_COLUMNS * LCD_ROWS] = {0};
  char payload[300] = {0};

  sprintf(payload, "{\"value\":0, \"context\":{\"next_dose_time\":\"%s\"}}", mensaje);

  if (isBelowTime(LCD_BLINK_TIME))
  {
    snprintf(mensaje, sizeof(mensaje), "Time: %02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
  }
  else
  {
    snprintf(mensaje, sizeof(mensaje), "Next dose: \n%02d:%02d %s", schedule[nextPeriod].tm_hour, schedule[nextPeriod].tm_min, weekDays[schedule[nextPeriod].tm_wday]);
    long now = millis();
    if (now - last_time > 50000)
    {
      snprintf(payload, sizeof(payload), "%02d:%02d %s", schedule[nextPeriod].tm_hour, schedule[nextPeriod].tm_min, weekDays[schedule[nextPeriod].tm_wday]);
      mqtt_publish_message(next_dose_time_topic, NO_VALUE, payload);
      last_time = now;
    }
  }

  writeLCD(mensaje);

  vTaskDelay(LCD_BLINK_TIME);
}

void notifyDoseAvailableCallback(void *)
{
  if (isBelowTime(NOTIFICATION_FRECUENCY_ALERT))
  {
    setLeds[objetivePeriod](HIGH);
    writeLCD("Dose available");
    stopBuzzer();
  }
  else
  {
    setLeds[objetivePeriod](LOW);
    writeLCD("Take it now!");
    startBuzzer();
  }
  vTaskDelay(NOTIFICATION_FRECUENCY_ALERT);
}
void notifyDoseUnavailableCallback(void *)
{
  if (isBelowTime(NOTIFICATION_UNNAVAILABLE_ALERT))
  {
    setLeds[objetivePeriod](HIGH);
    writeLCD("No pill detected...");
    stopBuzzer();
  }
  else
  {
    setLeds[objetivePeriod](LOW);
    writeLCD("Fill the dispenser");
    startBuzzer();
  }
  vTaskDelay(NOTIFICATION_UNNAVAILABLE_ALERT);
}

void notifyDoseAvailable(void *)
{
  while (1)
  {
    waitForSemaphore(notifyDoseAvailableCallback, NULL, notificationSemaphore); // Espera a que el semáforo esté disponible
  }
}

void showHourTimerLCD(void *)
{
  while (1)
  {
    waitForSemaphore(showHourTimerLCDCallback, NULL, showTimerSemaphore); // Espera a que el semáforo esté disponible
  }
}

void notifyDoseUnnavailable(void *)
{
  while (1)
  {
    waitForSemaphore(notifyDoseUnavailableCallback, NULL, noPillNotificationSemaphore);
  }
}

void scanAllPills(void *)
{
  while (1)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Espera a que se notifique la tarea
    Serial.println("Scanning all pills...");
    short scanStatus[MAX_PILLS_PER_DAY * MAX_DAYS] = {0}; // Array para almacenar el estado de cada pastilla
    startMotorRight();
    for (int i = 0; i < MAX_DAYS; i++)
    {
      Serial.println("aca0");
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
      Serial.print("Scanning day ");
      Serial.println(i + 1);
      stopMotor();
      vTaskDelay(100); // Espera un poco para evitar lecturas erróneas
      Serial.println("aca1");
      for (int j = 0; j < MAX_PILLS_PER_DAY; j++)
      {
        Serial.println("aca2");
        short value = presenceSensorsArray[j]();
        scanStatus[i * MAX_PILLS_PER_DAY + j] = value > PRECENSE_THRESHOLD ? 1 : 0; // 1 si hay pastilla, 0 si no
      }
      Serial.println("aca3");
      startMotorRight();
      Serial.println("aca4");
    }
    startMotorLeft();
    for (int i = 0; i < MAX_DAYS; i++)
    {
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
      Serial.print("Returning from day ");
      Serial.println(i + 1);
    }
    stopMotor();
    // Send scanStatus to MQTT
    // TODO
    Serial.print("Scan Status: ");
    for (int i = 0; i < MAX_PILLS_PER_DAY * MAX_DAYS; i++)
    {
      Serial.print(scanStatus[i]);
      if (i < MAX_PILLS_PER_DAY * MAX_DAYS - 1)
        Serial.print(", ");
    }
    Serial.println();
  }
}