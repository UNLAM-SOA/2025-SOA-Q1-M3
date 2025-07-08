#pragma once
// Enable debug states prints
//----------------------------------------------
#define SERIAL_DEBUG_ENABLED 1

#if SERIAL_DEBUG_ENABLED
#define DebugPrint(str) \
 {                      \
  Serial.print(str);    \
 }
#define DebugPrintln(str) \
 {                        \
  Serial.println(str);    \
 }
#else
#define DebugPrintln(str)
#endif

#define DebugPrintEstado(estado, evento)                         \
 {                                                               \
  String est = estado;                                           \
  String evt = evento;                                           \
  String str;                                                    \
  str = "-----------------------------------------------------"; \
  DebugPrintln(str);                                             \
  str = "EST-> [" + est + "]: " + "EVT-> [" + evt + "].";        \
  DebugPrintln(str);                                             \
  str = "-----------------------------------------------------"; \
  DebugPrintln(str);                                             \
 }
//----------------------------------------------