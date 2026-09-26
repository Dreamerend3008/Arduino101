#include <Arduino.h>

// Pins de lectura
const uint8_t A_IN[] = {A5,A3,A0};
const uint8_t D_IN[] = {4};


// Valores de lectura 
float l_temp;
float l_micro;
float l_pot;
bool l_pul;

// Pins de Salida
const uint8_t rgb_led_OUT[] = {9,10,11};
const uint8_t led_OUT[] = {3,5,6};
const uint8_t buzerPin = 7;

// variables de salida 
int rgb_led[] = {255,255,255};
int led[3] = {255,255,255};

// variables normalizadas
float temp;


void debug()
{
  // Funcion para debuggear las variables de lectura 
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
  // Funcion que hace que un led haga efecto fade in -> fade out

  // Enciende el buzzer por 150ms
  digitalWrite(buzerPin, HIGH);
  delay(150);
  digitalWrite(buzerPin, LOW);

  // Fade out
  for (int brightness = 255; brightness >= 0; brightness--) {
    for (int j = 0; j < 3; j++) {
      led[j] = brightness;
      analogWrite(led_OUT[j], led[j]);
    }
    delay(2); 
  }

  // Fade in 
  for (int brightness = 0; brightness <= 255; brightness++) {
    for (int j = 0; j < 3; j++) {
      led[j] = brightness;
      analogWrite(led_OUT[j], led[j]);
    }
    delay(2);
  }
}

void emergency(){
  // Funcion que hace que el led parpadee y suene el buzzer en un patron de SOS

  // Normalizacion del tiempo de delay entre cada parpadeo, usando el valor del potenciómetro
  float time = map(l_pot, 0, 1023, 120, 500);
  digitalWrite(led_OUT[0], HIGH);
  digitalWrite(led_OUT[1], LOW);
  digitalWrite(led_OUT[2], LOW);

  // Arreglo que contiene el patron de SOS, 1 = encendido, 0 = apagado
  bool sos[] = {1,1,1,0,0,0,1,1,1};

  // Funcion que recorre sos, diferenciando entre los valores 1 y 0 para representar en codigo morse punto y raya
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
}

void setup()
{
  // Inicidalizamos los pines de lectura y salida
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

  // Si la temperatura es menor a 25 grados, el led se pone naranja
  if(temp < 25)
  {
    // Naranja 
    rgb_led[0] = 255;
    rgb_led[1] = 10;
    rgb_led[2] = 0;

  }
  // Si la temperatura esta entre 25 y 35 grados, el led se pone morado
  else if(temp >= 25 && temp <= 35)
  {
    // Morado 
    rgb_led[0] = 85;
    rgb_led[1] = 0;
    rgb_led[2] = 255;
  }
  // Si la temperatura es mayor a 35 grados, el led se pone verde azulado
  else
  {
    // Verde azulado
    rgb_led[0] = 0;
    rgb_led[1] = 255;
    rgb_led[2] = 140;
    // Si el boton esta presionado y la temperatura es mayor a 35 grados, se activa la funcion de emergencia
    if(l_pul) emergency(); // La funcion de emergencia corresponde a la funcion con el patron creativo de encendido de leds
  }

  // output de variables
  for(int i=0;i<3;i++) analogWrite(rgb_led_OUT[i], rgb_led[i]);

  // Si el valor del microfono es mayor a 800, se activa la funcion de fade in fade out
  if(l_micro > 800) for(int i=0;i<3;i++)fade_in_fade_out();

  for(int i=0;i<4;i++)analogWrite(led_OUT[i], 0);
}