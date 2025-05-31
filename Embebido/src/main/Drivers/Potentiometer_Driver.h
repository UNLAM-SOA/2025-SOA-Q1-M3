short readPotentiometer();

short readPotentiometer(int pin)
{
 int value = analogRead(pin);        // Lee el valor del potenciómetro
 return map(value, 0, 4096, 0, 255); // Mapea el valor a un rango de 0 a 100
}