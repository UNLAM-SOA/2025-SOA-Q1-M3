#include "fisical.h"
#include "state_machine.h"
void setup() {
    Serial.begin(9600);
    fisicalSetup();
}

void loop(){
    state_machine();
}