short readCNY70(int pin);

short readCNY70(int pin)
{
 int value = analogRead(pin); // Lee el valor del sensor CNY70
 return value;                // Devuelve el valor leído (0-1023)
}
