#pragma once // Avoid multiple redefinitions of the same file
// This file encapsulates the physical layer of the system, including the buttons, presence sensors, limit switches, and buzzer.
//  It provides functions to read the state of these components and to write to the buzzer.
// #include <Wire.h> // Librería para comunicación I2C
#include "debug.h"
// Variables y valores
#define BUTTON_1 2
#define BUTTON_2 3
#define BUTTON_3 4

#define PRESENCE_PIN_1 5
#define PRESENCE_PIN_2 6
#define PRESENCE_PIN_3 7

#define PRESENCE_LED_1 10
#define PRESENCE_LED_2 11
#define PRESENCE_LED_3 12

// TODO: Agregar pines necesarios para el girarMotor

// Fines de carrera
#define LIMIT_SWITCH_MOVIL 8
#define LIMIT_SWITCH_START 9

#define BUZZER_PIN 13
#define POTENTIOMETER_PIN 14

#define LCD_SDA_PIN 15
#define LCD_SCL_PIN 16

#define LCD_ADDRESS 0x27 // Dirección I2C del LCD (ajustar según el modelo)
#define LCD_COLUMNS 16   // Número de columnas del LCD
#define LCD_ROWS 2       // Número de filas del LCD

short readPotentiometer();
short readPresenceSensor(int pin);
short readLimitSwitch(int pin);
short readButton(int pin);
short writeBuzzer(short value);
void writeLCD(const char *message);
void writeLCD(const char *message);
void setupLCD();
void clearLCD();
void startMotorLeft();
void startMotorRight();
void stopMotor();
void fisicalSetup();

short readPotentiometer()
{
 int value = analogRead(POTENTIOMETER_PIN); // Lee el valor del potenciómetro
 return map(value, 0, 1023, 0, 100);        // Mapea el valor a un rango de 0 a 100
}
short readPresenceSensor(int pin)
{
 if (pin != PRESENCE_PIN_1 && pin != PRESENCE_PIN_2 && pin != PRESENCE_PIN_3)
 {
  DebugPrint("Error: Pin no válido para el sensor de presencia.");
  return -1; // Retorna un valor de error si el pin no es válido
 }
 int value = digitalRead(pin); // Lee el valor del sensor de presencia
 return value;                 // Devuelve el valor leído (0 o 1)
}
short readLimitSwitch(int pin)
{
 if (pin != LIMIT_SWITCH_MOVIL && pin != LIMIT_SWITCH_START)
 {
  DebugPrint("Error: Pin no válido para el fin de carrera.");
  return -1; // Retorna un valor de error si el pin no es válido
 }
 int value = digitalRead(pin); // Lee el valor del fin de carrera
 return value;                 // Devuelve el valor leído (0 o 1)
}
short readButton(int pin)
{
 if (pin != BUTTON_1 && pin != BUTTON_2 && pin != BUTTON_3)
 {
  DebugPrint("Error: Pin no válido para el botón.");
  return -1; // Retorna un valor de error si el pin no es válido
 }
 int value = digitalRead(pin); // Lee el valor del botón
 return value;                 // Devuelve el valor leído (0 o 1)
}
short writeBuzzer(short value)
{
 if (value != 0 && value != 1)
 {
  DebugPrint("Error: Valor no válido para el zumbador.");
  return -1; // Retorna un valor de error si el valor no es válido
 }
 digitalWrite(BUZZER_PIN, value); // Escribe el valor en el zumbador
 return 0;                        // Retorna 0 si la operación fue exitosa
}

void writeLCD(const char *message)
{
 // Wire.beginTransmission(LCD_ADDRESS); // Inicia la transmisión I2C
 // Wire.write(message);                 // Envía el mensaje al LCD
 // Wire.endTransmission();              // Finaliza la transmisión
}
void setupLCD()
{
 // Wire.begin(LCD_SDA_PIN, LCD_SCL_PIN); // Inicia la comunicación I2C con el LCD
 // Wire.beginTransmission(LCD_ADDRESS);  // Inicia la transmisión I2C
 // Wire.write(0x00);                     // Envía un comando al LCD (ajustar según el modelo)
 // Wire.endTransmission();               // Finaliza la transmisión
}
void clearLCD()
{
 // Wire.beginTransmission(LCD_ADDRESS); // Inicia la transmisión I2C
 // Wire.write(0x01);                    // Envía un comando para limpiar el LCD
 // Wire.endTransmission();              // Finaliza la transmisión
}
void startMotorLeft()
{
 // TODO: Implementar la función para iniciar el motor a la izquierda
}
void startMotorRight()
{
 // TODO: Implementar la función para iniciar el motor a la derecha
}
void stopMotor()
{
 // TODO: Implementar la función para detener el motor
}

void fisicalSetup()
{
 pinMode(BUTTON_1, INPUT_PULLUP); // Configura el botón 1 como entrada con resistencia pull-up
 pinMode(BUTTON_2, INPUT_PULLUP); // Configura el botón 2 como entrada con resistencia pull-up
 pinMode(BUTTON_3, INPUT_PULLUP); // Configura el botón 3 como entrada con resistencia pull-up

 pinMode(PRESENCE_PIN_1, INPUT); // Configura el sensor de presencia 1 como entrada
 pinMode(PRESENCE_PIN_2, INPUT); // Configura el sensor de presencia 2 como entrada
 pinMode(PRESENCE_PIN_3, INPUT); // Configura el sensor de presencia 3 como entrada

 pinMode(LIMIT_SWITCH_MOVIL, INPUT_PULLUP); // Configura el fin de carrera 1 como entrada con resistencia pull-up
 pinMode(LIMIT_SWITCH_START, INPUT_PULLUP); // Configura el fin de carrera 2 como entrada con resistencia pull-up

 pinMode(BUZZER_PIN, OUTPUT);       // Configura el zumbador como salida
 pinMode(POTENTIOMETER_PIN, INPUT); // Configura el potenciómetro como entrada

 setupLCD(); // Inicializa la pantalla LCD
}