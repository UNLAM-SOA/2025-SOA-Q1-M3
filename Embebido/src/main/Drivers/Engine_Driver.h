void startMotorLeft();
void startMotorRight();
void stopMotor();

void startMotorLeft(int pinPWM, int pinEngine1, int pinEngine2)
{
 digitalWrite(pinPWM, HIGH);
 digitalWrite(pinEngine1, HIGH);
 digitalWrite(pinEngine2, LOW);
}
void startMotorRight(int pinPWM, int pinEngine1, int pinEngine2)
{
 digitalWrite(pinPWM, HIGH);
 digitalWrite(pinEngine1, LOW);
 digitalWrite(pinEngine2, HIGH);
}
void stopMotor(int pinPWM, int pinEngine1, int pinEngine2)
{
 digitalWrite(pinPWM, LOW);
 digitalWrite(pinEngine1, LOW);
 digitalWrite(pinEngine2, LOW);
}