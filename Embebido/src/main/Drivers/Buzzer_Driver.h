
short writeBuzzer(short value);

short writeBuzzer(int pin, short value)
{
 digitalWrite(pin, value);
 return value;
}