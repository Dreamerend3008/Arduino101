#include <Arduino.h>

uint8_t pin = 3;

void setup() 
{
  pinMode(pin, OUTPUT);
  Serial.begin(9600);
}

void loop() 
{
  
  digitalWrite(pin, HIGH);
  Serial.print("Hola\n");
  delay(1000);
  digitalWrite(pin, LOW);
  Serial.print("Hola'nt\n");
  delay(1000);
}