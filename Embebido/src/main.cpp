#include <Arduino.h>
#include "./main/state_machine.h"

void setup()
{
 Serial.begin(9600);
 initialize();
}

void loop()
{
 state_machine();
}