#include <Arduino.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA 64
#define OLED_RESET -1
#define DIRECCION_OLED 0x3c

Adafruit_SSD1306 oled(
  ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, OLED_RESET
);

// pins
int p_pulsador, p_infrarojo, p_luz, p_temp;

// entradas
bool pulsador;
bool infrarojo; // puede cambiar a float
float luz;
float temp;

// salidas
int angulo_servo;
float pot_motor;
float buzzer;

// variables de estado
int personas = 0;

void debug(){
  Serial.print("l_pulsador: ");
  if(pulsador)Serial.println("True");
  else Serial.println("False");
  Serial.print("l_infra: ");
  if(infrarojo)Serial.println("True");
  else Serial.println("False");
  Serial.print("l_luz: ");
  Serial.println(luz);
  Serial.print("l_temp: ");
  Serial.println(temp);
}

void musica1(){

}

void musica2(){

}

void mostrarMensaje(String mensaje){

}


void abrirPuerta(){
  delay(3000); // delay de 3 segundos 
}

void setup() {
  Serial.begin(9600);
  // Setup de pantalla
  if(!oled.begin(SSD1306_SWITCHCAPVCC, DIRECCION_OLED)){
    while(true) {}
  }

  // pulsador
  pinMode(p_pulsador, INPUT_PULLUP);
  // infrarojo
  pinMode(p_infrarojo, INPUT_PULLUP); // tomamos por ahora el infrarojo como un boton
  // Luz
  pinMode(p_luz, INPUT);
  // Temperatura
  pinMode(p_temp, INPUT);
}

void loop() {
  // definicion de entradas

  //pulsador
  if(digitalRead(p_pulsador) == LOW)pulsador=true;
  else pulsador = false;
  // Infrarojo
  if(digitalRead(p_infrarojo) == LOW)infrarojo=true;
  else infrarojo= false;
  // Luz
  luz = analogRead(p_luz);
  // temperatura
  temp = analogRead(p_temp);

  // ingreso
  if(pulsador && personas < 15){
    musica1();
    abrirPuerta();
    personas++;
  }else{
    // laboratorio lleno
    musica2();
    mostrarMensaje("Laboratorio lleno");
  }
  // salida
  if(infrarojo && personas > 0){
    personas--;
  }
  // pantalla debe siempre mostrar personas presentes, capacidad, estado del lab
  mostrarMensaje("lo de arriba");
  // color de fondo que permita diferenciar el estado del laboratioro, normal, mucha gente, lleno
}
