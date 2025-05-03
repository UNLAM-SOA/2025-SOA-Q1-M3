#include "SD_MANAGER.h"

#define MAX_PERIODS 21

#define PATH_SCHEDULE "/schedule.csv"


// Declaraciones anticipadas
void setDefaultConfig();
void saveScheduleToSD();
void readCsvConfig();
String readSD(const char*);
void printSchedule();
void printCSVSchedule();

tm schedule[MAX_PERIODS];

void configSetup() {
  setupSD();
  if (!SD.exists("/schedule.csv")) {
    Serial.println("No config set, using defaults");
    setDefaultConfig();
    saveScheduleToSD();
  } else {
    readCsvConfig();
  }
  
  //printSchedule();
  //printCSVSchedule();
}

void setDefaultConfig() {
    // Domingo
    schedule[0] = {0, 6, 0, 0, 0, 0, 0, -1};  // Mañana
    schedule[1] = {0, 14, 0, 0, 0, 0, 0, -1}; // Tarde
    schedule[2] = {0, 21, 0, 0, 0, 0, 0, 0};  // Noche
    // Lunes
    schedule[3] = {0, 6, 0, 0, 0, 0, 1, 0};
    schedule[4] = {0, 0, 14, 0, 0, 0, 1, 0};
    schedule[5] ={0, 0, 21, 0, 0, 0, 1, 0};
    // Martes
    schedule[6] ={0, 0, 6, 0, 0, 0, 2, 0};
    schedule[7] ={0, 0, 14, 0, 0, 0, 2, 0};
    schedule[8] ={0, 0, 21, 0, 0, 0, 2, 0};
    // Miércoles
    schedule[9] ={0, 0, 6, 0, 0, 0, 3, 0};
    schedule[10] ={0, 0, 14, 0, 0, 0, 3, 0};
    schedule[11] ={0, 0, 21, 0, 0, 0, 3, 0};
    // Jueves
    schedule[12] ={0, 0, 6, 0, 0, 0, 4, 0};
    schedule[13] ={0, 0, 14, 0, 0, 0, 4, 0};
    schedule[14] ={0, 0, 21, 0, 0, 0, 4, 0};
    // Viernes
    schedule[15] ={0, 0, 6, 0, 0, 0, 5, 0};
    schedule[16] ={0, 3, 19, 0, 0, 0, 5, 0};
    schedule[17] ={0, 0, 21, 0, 0, 0, 5, 0};
    // Sábado
    schedule[18] ={0, 0, 6, 0, 0, 0, 6, 0};
    schedule[19] ={0, 16, 10, 0, 0, 0, 6, 0};
    schedule[20] ={0, 35, 11, 0, 0, 0, 6, -1};
}


void readCsvConfig() {
  String content = readSD(PATH_SCHEDULE);
  if (content.length() == 0) {
    Serial.println("Error: file empty or file doesn't exist");
    return;
  }

  int index = 0;
  int lineStart = 0;
  int lineEnd = content.indexOf('\n');  // Saltar el encabezado

  while (lineEnd != -1 && index < MAX_PERIODS) {
    lineStart = lineEnd + 1;  // Mover al inicio de la siguiente línea
    lineEnd = content.indexOf('\n', lineStart);
    
    String line;
    if (lineEnd == -1) {
      line = content.substring(lineStart);  // Última línea
    } else {
      line = content.substring(lineStart, lineEnd);
    }

    // Parsear los valores
    int values[4];
    int lastPos = 0;
    for (int i = 0; i < 4; i++) {
      int pos = line.indexOf(',', lastPos);
      if (pos == -1) pos = line.length();
      values[i] = line.substring(lastPos, pos).toInt();
      lastPos = pos + 1;
    }

    // Asignar a la estructura tm
    schedule[index] = {
      values[0],  // tm_min
      values[1],  // tm_hour
      0,         // tm_sec
      0,         // tm_mday (día del mes)
      0,         // tm_mon (mes)
      0,         // tm_year
      values[2],  // tm_wday (día de la semana)
      values[3]   // tm_isdst (disponibilidad)
    };
    
    index++;
  }

  Serial.print("Horario cargado: ");
  Serial.print(index);
  Serial.println(" registros");
}

void saveScheduleToSD() {
  String content = "min,hour,day,available\n";  // Encabezado
  
  for (int i = 0; i < MAX_PERIODS; i++) {
    content += String(schedule[i].tm_min) + ",";
    content += String(schedule[i].tm_hour) + ",";
    content += String(schedule[i].tm_wday) + ",";
    content += String(schedule[i].tm_isdst) + "\n";
  }
  
  overwriteFileToSD(PATH_SCHEDULE, content);
  Serial.println("Horario guardado en SD");
}

void printSchedule() {
  const char* days[] = {"Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab"};
  
  for (int i = 0; i < MAX_PERIODS; i++) {
    Serial.printf("%s %02d:%02d - %s\n", 
                 days[schedule[i].tm_wday],
                 schedule[i].tm_hour,
                 schedule[i].tm_min,
                 schedule[i].tm_isdst == -1 ? "Inactivo" : "Activo");
  }
}

void printCSVSchedule() {
  String content = readSD(PATH_SCHEDULE);
  Serial.println("\nContenido de schedule.csv:");
  Serial.println("--------------------------");

  // Leer y mostrar el archivo línea por línea
  Serial.println(content);

  file.close();
  Serial.println("--------------------------");
  Serial.println("Fin del archivo\n");
}