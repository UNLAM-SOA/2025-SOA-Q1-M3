#include <Wire.h>
#include "rgb_lcd.h"
#include "../freeRTOS_Objects.h"

// Configuración del LCD
#define LCD_ADDRESS 0x27 // Dirección I2C común para displays 16x2
#define LCD_COLUMNS 16
#define LCD_ROWS 2
#define PIN_SCL 22
#define PIN_SDA 21

// Colores iniciales (RGB)
const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

rgb_lcd lcd; // Objeto LCD

// Inicializa el display LCD
void setupLCD()
{
 Wire.begin(PIN_SDA, PIN_SCL); // Inicializa I2C con pines específicos
 lcd.begin(LCD_COLUMNS, LCD_ROWS);
 lcd.setRGB(colorR, colorG, colorB);

 // Inicializar el mutex si no está creado
 if (lcdMutex == NULL)
 {
  lcdMutex = xSemaphoreCreateMutex();
 }
}

// Limpia el display de forma segura
void clearLCD()
{
 if (xSemaphoreTake(lcdMutex, pdMS_TO_TICKS(100)))
 {
  lcd.clear();
  xSemaphoreGive(lcdMutex);
 }
}

// Escribe texto con manejo de saltos de línea (\n) de forma segura
void writeLCD(const char *message)
{
 if (xSemaphoreTake(lcdMutex, pdMS_TO_TICKS(100)))
 {
  int currentLine = 0;
  int currentPos = 0;
  char buffer[LCD_COLUMNS + 1]; // Buffer para cada línea
  int bufferIndex = 0;

  lcd.clear();

  for (int i = 0; message[i] != '\0' && currentLine < LCD_ROWS; i++)
  {
   if (message[i] == '\n' || currentPos >= LCD_COLUMNS)
   {
    // Fin de línea o salto de línea encontrado
    buffer[bufferIndex] = '\0';
    lcd.setCursor(0, currentLine);
    lcd.print(buffer);

    currentLine++;
    currentPos = 0;
    bufferIndex = 0;

    if (message[i] == '\n')
     continue; // Saltamos el carácter \n
   }

   if (currentLine < LCD_ROWS)
   {
    buffer[bufferIndex++] = message[i];
    currentPos++;
   }
  }

  // Escribir la última línea si queda contenido
  if (bufferIndex > 0 && currentLine < LCD_ROWS)
  {
   buffer[bufferIndex] = '\0';
   lcd.setCursor(0, currentLine);
   lcd.print(buffer);
  }

  xSemaphoreGive(lcdMutex);
 }
}

// Mueve el cursor a posición específica (0-based) de forma segura
void setCursorLCD(uint8_t col, uint8_t row)
{
 if (xSemaphoreTake(lcdMutex, pdMS_TO_TICKS(100)))
 {
  if (col >= LCD_COLUMNS)
   col = LCD_COLUMNS - 1;
  if (row >= LCD_ROWS)
   row = LCD_ROWS - 1;
  lcd.setCursor(col, row);
  xSemaphoreGive(lcdMutex);
 }
}

// Escribe en posición específica de forma segura
void writeAtPosition(uint8_t col, uint8_t row, const char *text)
{
 if (xSemaphoreTake(lcdMutex, pdMS_TO_TICKS(100)))
 {
  setCursorLCD(col, row);
  lcd.print(text);
  xSemaphoreGive(lcdMutex);
 }
}

// Cambia el color del backlight RGB
void setLCDColor(uint8_t r, uint8_t g, uint8_t b)
{
 if (xSemaphoreTake(lcdMutex, pdMS_TO_TICKS(100)))
 {
  lcd.setRGB(r, g, b);
  xSemaphoreGive(lcdMutex);
 }
}