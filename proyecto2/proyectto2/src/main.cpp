#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA 64
#define OLED_RESET -1
#define DIRECCION_OLED 0x3c

Adafruit_SSD1306 oled(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, OLED_RESET);

// Pines 
int p_pulsador = 2, p_infrarojo = 4, p_luz = A5, p_temp = A0;
int p_leds[3] = {9,5,3};
int p_servo = 6, p_motor = 11, p_buzzer = 10;

// Variables
bool pulsador;
bool infrarojo; 
bool pulsador_ant = false;
bool infrarojo_ant = false;
float luz;
float temp;
int leds[3] = {0,0,0};
String estado = "Normal";

int personas = 12;
int umbral_luz = 178, umbral_temp = 17;
int velocidad_motor = 0;
int maxVelocidad = 8415; 

Servo servo;

void debug(int time){
  Serial.print("l_pulsador: ");
  if(pulsador)Serial.println("True");
  else Serial.println("Fals");
  Serial.print("l_infra: ");
  if(infrarojo)Serial.println("True");
  else Serial.println("False");
  Serial.print("l_luz: ");
  Serial.println(luz);
  Serial.print("l_temp: ");
  Serial.println(temp);
  Serial.print("Personas: ");
  Serial.println(personas);
  Serial.println(estado);
  Serial.println(leds[0]);
  delay(time);
}

void actualizar_estado(){
  if (personas <= 5) estado = "Normal";
  else if (personas >= 13) estado = "Lleno";
  else estado = "Elevado";
}

void actualizar_leds(){
  if(estado == "Normal"){
    leds[0] = 0;
    leds[1] = 255;
    leds[2] = 85;
  }else if(estado == "Elevado"){
    leds[0] = 255;
    leds[1] = 45;
    leds[2] = 140;
  }else{
    leds[0] = 255;
    leds[1] = 0;
    leds[2] = 0;
  }
}

void musica1(){
  // among us esta muy aguda
  tone(p_buzzer, 1046); 
  delay(250);
  tone(p_buzzer, 1244); 
  delay(250); 
  tone(p_buzzer, 1400); 
  delay(250); 
  tone(p_buzzer, 1510); 
  delay(250);
  tone(p_buzzer, 1400); 
  delay(250);
  tone(p_buzzer, 1244); 
  delay(250); 
  tone(p_buzzer, 1046); 
  delay(250); 
  noTone(p_buzzer); 
  delay(500); 
  tone(p_buzzer, 932); 
  delay(125);
  tone(p_buzzer, 1174); 
  delay(125); 
  tone(p_buzzer, 1046); 
  delay(250);
  // end of first
  noTone(p_buzzer); 
  delay(500); 
  tone(p_buzzer, 780); 
  delay(250); 
  tone(p_buzzer, 525); 
  delay(250); 
  noTone(p_buzzer); 
  delay(250);
  //secont part
  tone(p_buzzer, 1046); 
  delay(250);
  tone(p_buzzer, 1244); 
  delay(250); 
  tone(p_buzzer, 1400); 
  delay(250); 
  tone(p_buzzer, 1510); 
  delay(250);
  tone(p_buzzer, 1400); 
  delay(250);
  tone(p_buzzer, 1244); 
  delay(250);
  tone(p_buzzer, 1400); 
  delay(250);
  noTone(p_buzzer); 
  delay(750);
  //fast part
  tone(p_buzzer, 1510); 
  delay(125);
  tone(p_buzzer, 1400); 
  delay(125);
  tone(p_buzzer, 1244); 
  delay(125);
  tone(p_buzzer, 1510); 
  delay(125);
  tone(p_buzzer, 1400); 
  delay(125);
  tone(p_buzzer, 1244); 
  delay(125);
  tone(p_buzzer, 1510); 
  delay(125);
  tone(p_buzzer, 1400); 
  delay(125);
  tone(p_buzzer, 1244); 
  delay(125);
  tone(p_buzzer, 1510); 
  delay(125);
  tone(p_buzzer, 1400); 
  delay(125);
  tone(p_buzzer, 1244); 
  delay(125);
  tone(p_buzzer, 1510); 
  delay(125);
  noTone(p_buzzer); 
  delay(500);
}

void musica2(){
  // Primera melodía (escala descendente) convertida al formato tone/delay
  tone(p_buzzer, 466); // NOTE_AS4
  delay(125);          // 16 (semicorchea)
  tone(p_buzzer, 415); // NOTE_GS4
  delay(125);          // 16
  tone(p_buzzer, 349); // NOTE_F4
  delay(125);          // 16
  
  tone(p_buzzer, 494); // NOTE_B4
  delay(63);           // 32 (fusa)
  tone(p_buzzer, 466); // NOTE_AS4
  delay(63);           // 32
  
  tone(p_buzzer, 415); // NOTE_GS4
  delay(125);          // 16
  tone(p_buzzer, 349); // NOTE_F4
  delay(125);          // 16
  tone(p_buzzer, 311); // NOTE_DS4
  delay(125);          // 16
  tone(p_buzzer, 262); // NOTE_C4
  delay(125);          // 16
  tone(p_buzzer, 233); // NOTE_AS3
  delay(125);          // 16
  tone(p_buzzer, 208); // NOTE_GS3
  delay(125);          // 16
  tone(p_buzzer, 175); // NOTE_F3
  delay(125);          // 16
  tone(p_buzzer, 208); // NOTE_GS3
  delay(125);          // 16
  tone(p_buzzer, 233); // NOTE_AS3
  delay(125);          // 16
  tone(p_buzzer, 247); // NOTE_B3
  delay(125);          // 16
  tone(p_buzzer, 262); // NOTE_C4
  delay(125);          // 16
  tone(p_buzzer, 311); // NOTE_DS4
  delay(125);          // 16
  
  tone(p_buzzer, 330); // NOTE_E4
  delay(125);          // 16
  noTone(p_buzzer);    // Micro-pausa para separar las dos notas E4 seguidas
  delay(10);           
  
  tone(p_buzzer, 330); // NOTE_E4
  delay(63);           // 32
  
  tone(p_buzzer, 349); // NOTE_F4
  delay(750);          // -4 (negra con puntillo)
  
  noTone(p_buzzer);    // Apagar el buzzer al finalizar la melodía
  delay(500);
}

void mostrarMensaje_ALERTA(String mensaje){
  oled.fillRect(0, 41, 128, 10, SSD1306_WHITE); 
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_BLACK); 
  int posicionX = (128 - (mensaje.length() * 6)) / 2;
  if (posicionX < 0) posicionX = 0; 
  oled.setCursor(posicionX, 42); 
  oled.print(mensaje);   
  oled.display();
  delay(500); 
  oled.fillRect(0, 41, 128, 10, SSD1306_BLACK);
  oled.display();
}

void mostrarMensaje_INFO() {
  oled.clearDisplay();
  oled.setTextSize(1);
  
  // Título
  oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE); 
  oled.setCursor(0, 0);
  oled.println("   INFO LABORATORIO   "); 
  
  oled.setTextColor(SSD1306_WHITE); 
  
  // Línea 1: Personas
  oled.setCursor(0, 16); 
  oled.print("Personas: ");
  oled.println(String(personas,HEX));

  // Línea 2: Estado
  oled.setCursor(0, 26); 
  oled.print("Estado: ");
  oled.println(estado);

  // Línea 3: Sensores
  oled.setCursor(0, 36); 
  oled.print("T:"); oled.print((int)temp); 
  oled.print(" L:"); oled.print((int)luz); 
  oled.print(" V:"); oled.print(map(velocidad_motor, 0, maxVelocidad, 0, 100)); 
  oled.println("%");

  // Barra de progreso
  oled.drawRect(14, 52, 100, 8, SSD1306_WHITE); 
  int anchoBarra = map(personas, 0, 15, 0, 96); 
  if(anchoBarra > 96) anchoBarra = 96;
  oled.fillRect(16, 54, anchoBarra, 4, SSD1306_WHITE);
  
  oled.display();
}

void abrirPuerta(){
  // Mueve el servo espera 3 segundos y vuelve a la posicion inicial
  servo.write(180); 
  delay(3000);
  servo.write(90); 
}

void encenderVentilador(int velocidad){
  // Enciende el ventilador con la velocidad proporcional a la variable "velocidad"
  int velocidad_PWM = map(velocidad, 0, maxVelocidad, 67, 255);
  if(velocidad_PWM > 255) velocidad_PWM = 255;
  if(velocidad_PWM < 0) velocidad_PWM = 0;
  analogWrite(p_motor, velocidad_PWM);
} 

void setup() {
  // Setup inicial
  Serial.begin(9600);
  pinMode(p_pulsador, INPUT_PULLUP);
  pinMode(p_infrarojo, INPUT_PULLUP); 
  pinMode(p_temp, INPUT);
  pinMode(p_buzzer, OUTPUT);
  pinMode(p_motor, OUTPUT);
  
  // Inicializar Servo
  servo.attach(p_servo);
  servo.write(90); 

  // Inicializar pantalla OLED
  if(!oled.begin(SSD1306_SWITCHCAPVCC, DIRECCION_OLED)){
    while(true) {}
  }
}

void loop() {
  pulsador = (digitalRead(p_pulsador) == LOW);
  infrarojo = (digitalRead(p_infrarojo) == LOW);
  luz = map(analogRead(p_luz), 0, 1023, 0, 255);
  temp = map(analogRead(p_temp), 0, 1023, 0, 255);
  
  // INGRESO
  if(pulsador && !pulsador_ant){
    if(personas < 15){
      personas++;
      mostrarMensaje_ALERTA("Entrada reg.");
      musica1();
      abrirPuerta();
    } else {
      musica2();
      mostrarMensaje_ALERTA("Lab. Lleno!");
    }
  }

  // Salida
  if(infrarojo && personas > 0 && !infrarojo_ant){
    personas--;
    mostrarMensaje_ALERTA("Salida reg.");
  }

  // Ventilador
  if(personas > 7 || luz > umbral_luz || temp > umbral_temp){
    velocidad_motor = (personas * (luz + temp)) + (3 * temp);
  } else {
    velocidad_motor = 0;
  }
  encenderVentilador(velocidad_motor);

  // actualizar estado
  actualizar_estado();

  // Led con base al estado
  actualizar_leds();
  for(int i=0;i<3;i++)analogWrite(p_leds[i], leds[i]);
  mostrarMensaje_INFO();

  // Guardar estado anterior de los botones para que no cuente varias veces 
  pulsador_ant = pulsador;
  infrarojo_ant = infrarojo;
  
  debug(100); 
}