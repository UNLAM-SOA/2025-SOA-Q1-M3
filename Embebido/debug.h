// Enable debug states prints
//----------------------------------------------
#define SERIAL_DEBUG_ENABLED 1

#if SERIAL_DEBUG_ENABLED
#define DebugPrint(str)      \
    {                        \
        Serial.println(str); \
    }
#else
#define DebugPrint(str)
#endif

#define DebugPrintEstado(estado, evento)                               \
    {                                                                  \
        String est = estado;                                           \
        String evt = evento;                                           \
        String str;                                                    \
        str = "-----------------------------------------------------"; \
        DebugPrint(str);                                               \
        str = "EST-> [" + est + "]: " + "EVT-> [" + evt + "].";        \
        DebugPrint(str);                                               \
        str = "-----------------------------------------------------"; \
        DebugPrint(str);                                               \
    }
//----------------------------------------------