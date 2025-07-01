
short readLimitSwitch(int pin);
short readLimitSwitch(int pin)
{
 int value = digitalRead(pin); // Lee el valor del fin de carrera
 return value;                 // Devuelve el valor leído (0 o 1)
}