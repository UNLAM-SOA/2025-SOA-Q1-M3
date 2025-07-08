#pragma once // Avoid multiple redefinitions of the same file
// This file encapsulates the physical layer of the system, including the buttons, presence sensors, limit switches, and buzzer.
//  It provides functions to read the state of these components and to write to the buzzer.
#include "debug.h"
#include "Drivers/LCD_Driver.h"
#include "Drivers/SD_Driver.h"
#include "Drivers/CNY70_Driver.h"
#include "Drivers/Buzzer_Driver.h"
#include "Drivers/Potentiometer_Driver.h"
#include "Drivers/LimitSwitch_Driver.h"
#include "Drivers/Button_Driver.h"
#include "Drivers/Engine_Driver.h"

// Variables y valores
#define BUTTON_PIN 26
#define PRESENCE_PIN_1 34
#define PRESENCE_PIN_2 35
#define PRESENCE_PIN_3 32

#define PRESENCE_LED_1 12
#define PRESENCE_LED_2 14
#define PRESENCE_LED_3 27

// Fines de carrera
#define LIMIT_SWITCH_PIN 25
#define START_LIMIT_SWITCH_PIN 5

#define BUZZER_PIN 13
#define POTENTIOMETER_PIN 33

#define LCD_SDA_PIN 15
#define LCD_SCL_PIN 16

#define IN1_PIN_PUENTE_H 4
#define IN2_PIN_PUENTE_H 2
#define EN_PIN_PUENTE_H 15

#define MIN_VOL 0
#define MAX_VOL 255
void fisicalSetup();
short readPresenceSensor_TM();
short readPresenceSensor_TT();
short readPresenceSensor_TN();
short readLimitSwitch();
short readButton();
void startBuzzer();
void stopBuzzer();
short readPotentiometer();

short buzzerVolume = 255;
short potentiometerLastValue;
volatile short startPressed;

short readPresenceSensor_TM()
{
 return readCNY70(PRESENCE_PIN_1);
}
short readPresenceSensor_TT()
{
 return readCNY70(PRESENCE_PIN_2);
}
short readPresenceSensor_TN()
{
 return readCNY70(PRESENCE_PIN_3);
}

short readLimitSwitch()
{
 return readLimitSwitch(LIMIT_SWITCH_PIN);
}

short readButton()
{
 return readButton(BUTTON_PIN);
}

void startBuzzer()
{
 writeBuzzer(BUZZER_PIN, buzzerVolume);
}

void stopBuzzer()
{
 writeBuzzer(BUZZER_PIN, 0);
}

short readPotentiometer()
{
 return readPotentiometer(POTENTIOMETER_PIN);
}

void setLedPresence_TM(short value)
{
 digitalWrite(PRESENCE_LED_1, value);
}
void setLedPresence_TT(short value)
{
 digitalWrite(PRESENCE_LED_2, value);
}
void setLedPresence_TN(short value)
{
 digitalWrite(PRESENCE_LED_3, value);
}

void setVolumeBuzzer(long volume)
{
 if (volume < MIN_VOL || volume > MAX_VOL)
 {
  DebugPrintln("Error: Volumen fuera de rango (0-255).");
  return;
 }
 buzzerVolume = volume;
}

short isStartPressed()
{
 return digitalRead(START_LIMIT_SWITCH_PIN) == LOW;
}
void startMotorLeft()
{
 startMotorLeft(EN_PIN_PUENTE_H, IN1_PIN_PUENTE_H, IN2_PIN_PUENTE_H);
}

void startMotorRight()
{
 startMotorRight(EN_PIN_PUENTE_H, IN1_PIN_PUENTE_H, IN2_PIN_PUENTE_H);
}
void stopMotor()
{
 stopMotor(EN_PIN_PUENTE_H, IN1_PIN_PUENTE_H, IN2_PIN_PUENTE_H);
}

void potentiometerSetup()
{
 potentiometerLastValue = readPotentiometer(POTENTIOMETER_PIN);
}

void fisicalSetup()
{
 pinMode(BUTTON_PIN, INPUT_PULLDOWN);

 pinMode(PRESENCE_PIN_1, INPUT);
 pinMode(PRESENCE_PIN_2, INPUT);
 pinMode(PRESENCE_PIN_3, INPUT);

 pinMode(PRESENCE_LED_1, OUTPUT);
 pinMode(PRESENCE_LED_2, OUTPUT);
 pinMode(PRESENCE_LED_3, OUTPUT);

 pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);
 pinMode(START_LIMIT_SWITCH_PIN, INPUT_PULLUP);
 pinMode(BUZZER_PIN, OUTPUT);
 pinMode(POTENTIOMETER_PIN, INPUT);

 pinMode(IN1_PIN_PUENTE_H, OUTPUT);
 pinMode(IN2_PIN_PUENTE_H, OUTPUT);
 pinMode(EN_PIN_PUENTE_H, OUTPUT);
 setupLCD();

 potentiometerSetup();
}