
short writeBuzzer(short value);

short writeBuzzer(int pin, short value)
{
 digitalWrite(pin, value); // Enciende el zumbador con el volumen especificado
 return value;             // Devuelve el valor escrito (0 o 1)
}