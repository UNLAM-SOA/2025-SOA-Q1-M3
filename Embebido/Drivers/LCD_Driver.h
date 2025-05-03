#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDRESS 0x27 // Dirección I2C común para displays 16x2
#define LCD_COLUMNS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// Inicializa el display LCD
void setupLCD()
{
 Wire.begin(); // Inicializa I2C con pines específicos
 lcd.init();
 lcd.backlight();
 lcd.clear();
}

// Limpia el display
void clearLCD()
{
 lcd.clear();
}

// Escribe texto con manejo de saltos de línea (\n)
void writeLCD(const char *message)
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
}

// Versión sobrecargada para String
void writeLCD(const String &message)
{
 writeLCD(message.c_str());
}

// Mueve el cursor a posición específica (0-based)
void setCursorLCD(uint8_t col, uint8_t row)
{
 if (col >= LCD_COLUMNS)
  col = LCD_COLUMNS - 1;
 if (row >= LCD_ROWS)
  row = LCD_ROWS - 1;
 lcd.setCursor(col, row);
}

// Escribe en posición específica
void writeAtPosition(uint8_t col, uint8_t row, const char *text)
{
 setCursorLCD(col, row);
 lcd.print(text);
}