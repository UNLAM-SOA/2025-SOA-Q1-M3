short readLimitSwitch(int pin);
short readLimitSwitch(int pin)
{
 int value = digitalRead(pin);
 return value;
}