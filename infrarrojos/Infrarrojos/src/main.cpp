#include <Arduino.h>


uint8_t infratin = 3;
uint8_t val = 0;

void setup()
{
  pinMode(infratin, INPUT);
  pinMode(13, OUTPUT);
}

void loop()
{
  val = digital.Read(infratin);
  if(val == HIGH) digitalWrite(13, HIGH);
  else digitalWrite(13, LOW);
}