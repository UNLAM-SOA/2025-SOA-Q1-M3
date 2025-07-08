#pragma once
#include "SD.h"
#include "../debug.h"

#define SCK_PIN 18  // GPIO18
#define MISO_PIN 19 // GPIO19
#define MOSI_PIN 23 // GPIO23
#define CS_PIN 5    // GPIO5

String readSD(const char *);
bool writeFileToSD(const char, const String &);
bool overwriteFileToSD(const char, const String &);

File file;

void setupSD()
{
 DebugPrintln("Initializing SD card...");
 SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
 if (!SD.begin(CS_PIN))
 {
  DebugPrintln("Error in SD module has occurred!");
  // ESP.restart();
 }
 DebugPrintln("SD module initialized");
}

String readSD(const char *path)
{
 file = SD.open(path);
 if (!file)
 {
  DebugPrintln("Error open file");
  return "";
 }
 String content;
 while (file.available())
 {
  content += (char)file.read();
 }
 file.close();
 DebugPrintln(content);
 return content;
}

bool writeFileToSD(const char *filename, const String &content)
{
 File file = SD.open(filename, FILE_WRITE); // Abre en modo escritura (append)
 if (!file)
 {
  DebugPrint("No se pudo abrir el archivo para escribir: ");
  DebugPrintln(filename);
  return false;
 }

 file.println(content); // Escribe el string con salto de línea
 file.close();
 return true;
}

bool overwriteFileToSD(const char *filename, const String &content)
{
 // Eliminar archivo anterior si existe
 if (SD.exists(filename))
 {
  SD.remove(filename);
 }

 // Crear nuevo archivo
 File file = SD.open(filename, FILE_WRITE);
 if (!file)
 {
  DebugPrint("Error al crear el archivo: ");
  DebugPrintln(filename);
  return false;
 }

 file.print(content);
 file.close();
 return true;
}