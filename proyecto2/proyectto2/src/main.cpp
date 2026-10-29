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

// pins entrada
int p_pulsador = 2 , p_infrarojo = 4, p_luz=3, p_temp=5;

// pins salida
int p_servo = 6, p_motor=9,p_buzzer=10;

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
int estado_lab = 0; // 0 normal 1 elevado 2 lleno
int umbral_luz=0, umbral_temp=0;
int velocidad_motor = 0;
int maxVelocidad = 1275;
bool infrarojo_ant = false;


// Classes
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
  s += "Personas lab: "; 
  s += String(personas, HEX);
  s += '\n';
  s += "Capacidad max: ";
  s += String(15);
  s += '\n';
  s += "Estado: ";
  if(personas <= 5) s += "Normal";
  else if (personas >= 13) s += "Lleno";
  else s += "Elevado";
  s += '\n';
  s += "Temperatura: ";
  s += String(temp);
  s += '\n';
  s += "Luz: ";
  s += String(luz);
  s += '\n';
  s += "Ventilacion: ";
  s += String(velocidad_motor);
  s += '\n';
  // falta formas con base al estado
  return s;
}

void musica1(){
  digitalWrite(p_buzzer, HIGH);
  delay(5);
  digitalWrite(p_buzzer, LOW);
}

void musica2(){
  digitalWrite(p_buzzer, HIGH);
  delay(1000);
  digitalWrite(p_buzzer, LOW);
}

void mostrarMensaje_ALERTA(String mensaje){
  oled.fillRect(8, 16, 112, 32, SSD1306_BLACK); 
  oled.fillRect(10, 18, 108, 28, SSD1306_WHITE); 
  
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_BLACK); 
  
  oled.setCursor(46, 21);
  oled.print("ALERTA"); 
  
  oled.setCursor(14, 33); 
  oled.print(mensaje);   
  
  // Actualizamos la pantalla para que aparezca la ventana
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

  // Barra vacia 
  oled.drawRect(14, 52, 100, 8, SSD1306_WHITE); 
  int anchoBarra = map(personas, 0, 15, 0, 96); 

  // Rellenamos la barra por dentro (dejando 2 píxeles de margen)
  oled.fillRect(16, 54, anchoBarra, 4, SSD1306_WHITE);
  oled.display();
}

void abrirPuerta(){
  servo.write(180);  // Move to 90 degrees
  delay(3000);
  servo.write(90); // Move to 180 degrees
}

void encenderVentilador(int velocidad){
  analogWrite(p_motor, map(velocidad, 0, maxVelocidad, 0, 255));
} 

void setup() {
  Serial.begin(9600);

  // pulsador
  pinMode(p_pulsador, INPUT_PULLUP);
  // infrarojo
  pinMode(p_infrarojo, INPUT_PULLUP); // tomamos por ahora el infrarojo como un boton
  // Luz
  pinMode(p_luz, INPUT);
  // Temperatura
  pinMode(p_temp, INPUT);

  // Buzzer
  pinMode(p_buzzer, OUTPUT);
  // Servo
  servo.attach(p_servo);
  // motor
  pinMode(p_motor, OUTPUT);

  // Setup de pantalla
  if(!oled.begin(SSD1306_SWITCHCAPVCC, DIRECCION_OLED)){
    while(true) {}
  }
}

void loop() {
  // definicion de entradas

  //pulsador
  if(digitalRead(p_pulsador) == LOW)pulsador=true;
  else pulsador = false;

  // Infrarojo
  if(digitalRead(p_infrarojo) == LOW)infrarojo=true;
  else infrarojo= false;
  infrarojo = !infrarojo;

  // Luz
  luz = analogRead(p_luz);
  luz = map(luz, 0, 1023, 0, 255);

  // temperatura
  temp = analogRead(p_temp);
  temp = map(temp, 0, 1023, 0, 255);

  // funcionamiento

  // ingreso
  if(pulsador && personas < 15){
    musica1();
    abrirPuerta();
    personas++;
    mostrarMensaje_ALERTA("Entrada registrada");
  }else if(pulsador && personas == 15){
    // laboratorio lleno
    musica2();
    mostrarMensaje_ALERTA("Laboratorio lleno");
  }

  // Salida 
  if(infrarojo && personas > 0 && !infrarojo_ant){
    mostrarMensaje_ALERTA("Salida registrada");
    personas--;
  }

  // Ventilador
  if(personas > 7 || luz  > umbral_luz || temp > umbral_temp){
    // funcion para calcular la velocidad 
    // f(n,l,t) = (n/15) * (luz+temp)
    // personas 0 -> 15
    // luz 0 -> 255 
    // temp 0 -> 255
    velocidad_motor = (personas/15)*(luz+temp) + 3*temp;
  }else{
    velocidad_motor = 0;
  }
  encenderVentilador(velocidad_motor);

  // pantalla debe siempre mostrar personas presentes, capacidad, estado del lab
  mostrarMensaje_INFO(INFO());

  infrarojo_ant = infrarojo;
  delay(1000);
}
