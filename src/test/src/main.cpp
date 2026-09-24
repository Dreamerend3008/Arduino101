#include <Arduino.h>

// lectura
const uint8_t A_IN[] = {A5,A3,A0};
const uint8_t D_IN[] = {4};


// lectura valores
float l_temp;
float l_micro;
float l_pot;
bool l_pul;

// Salida
const uint8_t rgb_led_OUT[] = {9,10,11};
const uint8_t led_OUT[] = {3,5,6};
const uint8_t buzerPin = 7;

// variables normalizadas
float temp;

// variables de output
int rgb_led[] = {255,255,255};
int led[3] = {255,255,255};


void debug()
{
  Serial.print("l_temp (ADC): ");
  Serial.println(l_temp);
  Serial.print("temperatura: ");
  Serial.println(temp);
  Serial.print("Micro: ");
  Serial.println(l_micro);
  Serial.print("Pot: ");
  Serial.println(l_pot);
  Serial.print("Pulsa: ");
  if(l_pul)Serial.println("True");
  else Serial.println("False");
  Serial.flush();
}

void fade_in_fade_out() {
  digitalWrite(buzerPin, HIGH);
  delay(150);
  digitalWrite(buzerPin, LOW);

  for (int brightness = 255; brightness >= 0; brightness--) {
    for (int j = 0; j < 3; j++) {
      led[j] = brightness;
      analogWrite(led_OUT[j], led[j]);
    }
    delay(2); 
  }

  for (int brightness = 0; brightness <= 255; brightness++) {
    for (int j = 0; j < 3; j++) {
      led[j] = brightness;
      analogWrite(led_OUT[j], led[j]);
    }
    delay(2);
  }
}

void emergency(){
  // 1023, 0
  float time = map(l_pot, 0, 1023, 120, 500);
  digitalWrite(led_OUT[0], HIGH);
  digitalWrite(led_OUT[1], LOW);
  digitalWrite(led_OUT[2], LOW);

  bool sos[] = {1,1,1,0,0,0,1,1,1};

  for(int i = 0; i < 9; i++)
  {
    if(sos[i])
    {
      digitalWrite(led_OUT[1], HIGH);
      digitalWrite(buzerPin, HIGH);
      delay(50);
      digitalWrite(buzerPin, LOW);
    }
    else
    {
      digitalWrite(led_OUT[2], HIGH);
      digitalWrite(buzerPin, HIGH);
      delay(100);
      digitalWrite(buzerPin, LOW);
    }

    delay(time);
    digitalWrite(led_OUT[1], LOW);
    digitalWrite(led_OUT[2], LOW);
    delay(time);
  }
  // 111000111

}

void setup()
{
  pinMode(D_IN[0], INPUT_PULLUP);
  for(int i=0;i<3;i++)pinMode(led_OUT[i], OUTPUT);
  pinMode(buzerPin, OUTPUT);
  Serial.begin(9600);
}



void loop()
{
  // lectura de variables
  l_temp = analogRead(A_IN[0]);
  l_micro = analogRead(A_IN[1]);
  l_pot = analogRead(A_IN[2]);
  l_pul = !digitalRead(D_IN[0]);
  temp = (l_temp * 5000.0 / 1024.0) / 10.0;

  if(temp < 25)
  {
    // orange
    rgb_led[0] = 255;
    rgb_led[1] = 10;
    rgb_led[2] = 0;

  }
  else if(temp >= 25 && temp <= 35)
  {
    // purple
    rgb_led[0] = 85;
    rgb_led[1] = 0;
    rgb_led[2] = 255;
  }
  else
  {
    // verde azulado
    rgb_led[0] = 0;
    rgb_led[1] = 255;
    rgb_led[2] = 140;
    if(l_pul) emergency();
  }

  // output de variables
  for(int i=0;i<3;i++) analogWrite(rgb_led_OUT[i], rgb_led[i]);
  if(l_micro > 800) for(int i=0;i<3;i++)fade_in_fade_out();

  for(int i=0;i<4;i++)analogWrite(led_OUT[i], 0);
}