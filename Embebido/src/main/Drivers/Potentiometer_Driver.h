short readPotentiometer();

short readPotentiometer(int pin)
{
 int value = analogRead(pin);
 return map(value, 0, 4096, 0, 255);
}