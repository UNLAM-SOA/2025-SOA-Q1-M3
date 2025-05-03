
void setup()
{
 current_state = ST_INIT;
 Serial.begin(9600);
 fisicalSetup();
 attachInterrupt(LIMIT_SWITCH_MOVIL, detectMovingLimitSwitch, RISING);
 setupWifi();
 setupTime();
 createNewScheduledTimer();
 xTaskCreate(testTimer, "testTimer", 2048, &nextPeriod, 1, &eventTaskHandle);
 timeEventsQueue = xQueueCreate(MAX_EVENTS_QUEUE, sizeof(events));
}

void loop()
{
 state_machine();
}