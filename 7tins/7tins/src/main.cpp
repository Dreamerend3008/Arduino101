#include <Arduino.h>

const int pines[] = {2, 3, 4, 5, 6, 7, 8};

int tiempo = 500;

const byte numerosDisplay[] = {
  0b1000000,
  0b1111010,
  0b0100100,
  0b0101000,
  0b0011010,
  0b0001001,
  0b0000001,
  0b1111000,
  0b0000000,
  0b0011000,
  0b0010000, // A
  0b0000011, // b
  0b1000101, // C
  0b0100010, // d
  0b0000101, // E
  0b0010101  // F
};

const byte letrasDisplay[] = {
  0b0010000, // A
  0b0000011, // b
  0b1000101, // C
  0b0100010, // d
  0b0000101, // E
  0b0010101  // F
};

void encender(int numeros)
{
  byte numeroBit = numerosDisplay[numeros];
  Serial.println(numeros);

  for(int i = 0; i < 7; i++)
  {
    int bit = bitRead(numeroBit, i);
    digitalWrite(pines[i], !bit);
  }
}

void setup()
{
  Serial.begin(9600);
  for(int i = 0; i < 7; i++)
  {
    pinMode(pines[i], OUTPUT);
  }
  encender(0);
}

void loop()
{
  for(int i = 0; i < 16; i++)
  {
    encender(i);
    delay(tiempo);
  }
}
