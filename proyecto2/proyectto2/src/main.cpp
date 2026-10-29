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

// Pines (Corregidos a A3 y A5 para que no haya ambigüedad)
int p_pulsador = 2, p_infrarojo = 4, p_luz = A3, p_temp = A5;
int p_servo = 6, p_motor = 9, p_buzzer = 10;

// Variables
bool pulsador;
bool infrarojo; 
bool pulsador_ant = false;
bool infrarojo_ant = false;
float luz;
float temp;

int personas = 0;
int umbral_luz = 0, umbral_temp = 0;
int velocidad_motor = 0;
int maxVelocidad = 1275; // Aunque analogWrite llega maximo a 255

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
  delay(time);
}


String INFO() {
  String s = "";
  s += "Personas: "; s += String(personas); s += "/15\n";
  
  // Línea 2: Estado
  s += "Estado: ";
  if(personas <= 5) s += "Normal\n";
  else if (personas >= 13) s += "Lleno\n";
  else s += "Elevado\n";
  
  // Línea 3: Sensores
  s += "T:"; s += String((int)temp); 
  s += " L:"; s += String((int)luz); 
  s += " V:"; s += String(map(velocidad_motor, 0, maxVelocidad, 0, 100)); s += "%\n";
  
  return s;
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
  digitalWrite(p_buzzer, HIGH); delay(1000); digitalWrite(p_buzzer, LOW);
}

void mostrarMensaje_ALERTA(String mensaje){
  oled.fillRect(0, 41, 128, 10, SSD1306_WHITE); 
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_BLACK); 
  int posicionX = (128 - (mensaje.length() * 6)) / 2;
  if (posicionX < 0) posicionX = 0; // Evitar que se salga por la izquierda
  oled.setCursor(posicionX, 42); 
  oled.print(mensaje);   
  oled.display();
  delay(500); 
  oled.fillRect(0, 41, 128, 10, SSD1306_BLACK);
  oled.display();
}

void mostrarMensaje_INFO(String mensaje) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE); 
  oled.setCursor(0, 0);
  oled.println("   INFO LABORATORIO   "); 
  
  oled.setTextColor(SSD1306_WHITE); 
  oled.setCursor(0, 16); 
  oled.print(mensaje);   

  // Barra de progreso
  oled.drawRect(14, 52, 100, 8, SSD1306_WHITE); 
  int anchoBarra = map(personas, 0, 15, 0, 96); 
  if(anchoBarra > 96) anchoBarra = 96;
  oled.fillRect(16, 54, anchoBarra, 4, SSD1306_WHITE);
  oled.display();
}

void abrirPuerta(){
  servo.write(180); 
  delay(3000);
  servo.write(90); 
}

void encenderVentilador(int velocidad){
  int velocidad_PWM = map(velocidad, 0, maxVelocidad, 0, 255);
  // Evitamos pasarnos del límite de PWM de Arduino (0 a 255)
  if(velocidad_PWM > 255) velocidad_PWM = 255;
  if(velocidad_PWM < 0) velocidad_PWM = 0;
  
  analogWrite(p_motor, velocidad_PWM);
} 

void setup() {
  Serial.begin(9600);
  pinMode(p_pulsador, INPUT_PULLUP);
  pinMode(p_infrarojo, INPUT_PULLUP); 
  pinMode(p_luz, INPUT);
  pinMode(p_temp, INPUT);
  pinMode(p_buzzer, OUTPUT);
  pinMode(p_motor, OUTPUT);
  
  servo.attach(p_servo);
  servo.write(90); 

  if(!oled.begin(SSD1306_SWITCHCAPVCC, DIRECCION_OLED)){
    while(true) {}
  }
}

void loop() {
  pulsador = (digitalRead(p_pulsador) == LOW);
  infrarojo = (digitalRead(p_infrarojo) == LOW);

  luz = map(analogRead(p_luz), 0, 1023, 0, 255);
  temp = map(analogRead(p_temp), 0, 1023, 0, 255);

  // INGRESO: Agregado el control de flanco (!pulsador_ant)
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

  // SALIDA 
  if(infrarojo && personas > 0 && !infrarojo_ant){
    personas--;
    mostrarMensaje_ALERTA("Salida reg.");
  }

  // VENTILADOR: Arreglada la división usando float
  if(personas > 7 || luz > umbral_luz || temp > umbral_temp){
    float proporcion_personas = (float)personas / 15.0; // Ahora sí dará 0.33, 0.5, etc.
    velocidad_motor = (proporcion_personas * (luz + temp)) + (3 * temp);
  } else {
    velocidad_motor = 0;
  }
  encenderVentilador(velocidad_motor);

  mostrarMensaje_INFO(INFO());

  // Guardar estado anterior de los botones para que no sume de a 10 personas por segundo
  pulsador_ant = pulsador;
  infrarojo_ant = infrarojo;
  
  debug(500); // Reducido a 50ms para que la detección de botones sea fluida
}