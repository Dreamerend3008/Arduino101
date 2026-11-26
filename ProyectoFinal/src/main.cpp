#include <Arduino.h>
#include <Wire.h>
#include <BluetoothSerial.h>
#include "pantalla.h"
#include "cancion.h"

#define DIRECCION_MPU 0x68

BluetoothSerial SerialBT;

// Pines verificados para ESP32 DevKit V1 de 30 pines.
// Usar los números GPIO impresos en la placa, no la posición física del pin.
const uint8_t p_luz      = 34; // ADC1, solo entrada
const uint8_t p_humedad  = 35; // ADC1, solo entrada
const uint8_t p_temp     = 32; // ADC1

const uint8_t p_pulsador = 14; // Pulsador a GND
const uint8_t p_sda      = 21;
const uint8_t p_scl      = 22;

// Salidas
const uint8_t p_leds[3]  = {26, 18, 19}; // {R, G, B}, LED de cátodo común
const uint8_t p_bomba    = 27;           // Solo señal al transistor/MOSFET
const uint8_t p_buzzer   = 25;

// Constantes para calibrar
const char* BT_NOMBRE        = "Tamagotchi_ESP32";
const int   LUZ_MIN_RAW      = 0;
const int   LUZ_MAX_RAW      = 4095;
const int   HUM_SECO_RAW     = 4095;
const int   HUM_MOJADO_RAW   = 1200;
const float ADC_VREF_MV      = 3300.0f;
const float TEMP_OFFSET_MV   = 0.0f;  
const float TEMP_MV_POR_C    = 10.0f;
const float MAGNITUD_ACELEROMETRO_MIN = 0.0f; // g
const float MAGNITUD_ACELEROMETRO_MAX = 2.0f; // g
const float MAGNITUD_GYROSCOPIO_MIN = 0.0f; // °/s
const float MAGNITUD_GYROSCOPIO_MAX = 200.0f; // °/s
const float MAGNITUD_TOTAL_MIN = 0.0f; // g + °/s
const float MAGNITUD_TOTAL_MAX = 200.0f; // g + °/s
const bool  BOMBA_ACTIVA_HIGH = true; 

// ===================== VARIABLES DE LECTURA Y ESTADO =====================
int   luz_raw = 0, luz = 0;         // luz: 0..100 %
int   humedad_raw = 0, humedad = 0; // humedad: 0..100 %
int   temp_raw = 0; float temp = 0.0f; // °C
bool  pulsador = false;
bool  pulsador_ant = false;
float ax = 0, ay = 0, az = 0;       // Acelerómetro (g)
float gx = 0, gy = 0, gz = 0;       // Giroscopio (°/s)
float magnitud_acelerometro_raw = 0.0f;
float magnitud_gyroscopio_raw = 0.0f;
float magnitud_total_raw = 0.0f;
float magnitud_acelerometro = 0.0f;
float magnitud_gyroscopio = 0.0f;
float magnitud_total = 0.0f;
String bt_msg = "";

int   leds[3] = {0, 0, 0};
bool  bomba_activa = false;
bool  mpu_ok  = false;
bool  mpu_valido = false;
bool  debug_activo = true;

// giroscopio y acelerómetro MPU6050
bool mpu_init() {
  Wire.beginTransmission(DIRECCION_MPU);
  Wire.write(0x6B); // PWR_MGMT_1
  Wire.write(0x00); // Despertar sensor
  return Wire.endTransmission() == 0;
}

bool leer_mpu() {
  Wire.beginTransmission(DIRECCION_MPU);
  Wire.write(0x3B); // ACCEL_XOUT_H
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom((int)DIRECCION_MPU, 14) != 14) return false;

  auto read16 = []() -> int16_t {
    uint8_t hi = Wire.read();
    uint8_t lo = Wire.read();
    return (int16_t)((hi << 8) | lo);
  };
  ax = read16() / 16384.0f;
  ay = read16() / 16384.0f;
  az = read16() / 16384.0f;
  (void)read16(); // Temp interna del MPU
  gx = read16() / 131.0f;
  gy = read16() / 131.0f;
  gz = read16() / 131.0f;
  magnitud_acelerometro_raw = fabs(sqrt(ax * ax + ay * ay + az * az) - 1.0f);
  magnitud_gyroscopio_raw = sqrt(gx * gx + gy * gy + gz * gz);
  magnitud_total_raw = sqrt(magnitud_acelerometro_raw * magnitud_acelerometro_raw + magnitud_gyroscopio_raw * magnitud_gyroscopio_raw);
  magnitud_acelerometro = constrain((magnitud_acelerometro_raw - MAGNITUD_ACELEROMETRO_MIN) * 100.0f / (MAGNITUD_ACELEROMETRO_MAX - MAGNITUD_ACELEROMETRO_MIN), 0.0f, 100.0f);
  magnitud_gyroscopio   = constrain((magnitud_gyroscopio_raw - MAGNITUD_GYROSCOPIO_MIN) * 100.0f / (MAGNITUD_GYROSCOPIO_MAX - MAGNITUD_GYROSCOPIO_MIN), 0.0f, 100.0f);
  magnitud_total        = constrain((magnitud_total_raw - MAGNITUD_TOTAL_MIN) * 100.0f / (MAGNITUD_TOTAL_MAX - MAGNITUD_TOTAL_MIN), 0.0f, 100.0f);
  return true;
}

void leer_entradas() {
  pulsador = (digitalRead(p_pulsador) == LOW);

  luz_raw = analogRead(p_luz);
  luz = constrain(map(luz_raw, LUZ_MIN_RAW, LUZ_MAX_RAW, 0, 100), 0, 100); // Funcion para sacar 0,100% que cool

  humedad_raw = analogRead(p_humedad);
  humedad = constrain(map(humedad_raw, HUM_SECO_RAW, HUM_MOJADO_RAW, 0, 100), 0, 100);

  temp_raw = analogRead(p_temp);
  float mv = (temp_raw / 4095.0f) * ADC_VREF_MV;
  temp = (mv - TEMP_OFFSET_MV) / TEMP_MV_POR_C;

  mpu_valido = mpu_ok && leer_mpu();

  if (SerialBT.available()) {
    bt_msg = SerialBT.readStringUntil('\n');
    bt_msg.trim();
  }
}

// Outputs
void actualizar_leds(int r, int g, int b) {
  leds[0] = r;
  leds[1] = g;
  leds[2] = b;
  for (int i = 0; i < 3; i++) {
    int val = leds[i];
    analogWrite(p_leds[i], val);
  }
}

void controlar_bomba(bool encender) {
  bomba_activa = encender;
  digitalWrite(p_bomba, (encender == BOMBA_ACTIVA_HIGH) ? HIGH : LOW);
}

void sonar_buzzer(int freq, int duracion_ms = 0) {
  tone(p_buzzer, freq, duracion_ms);
}

void apagar_buzzer() {
  noTone(p_buzzer);
}

void enviar_bt(String mensaje) {
  SerialBT.println(mensaje);
}

// Debug (cada N ciclos del loop; con delay(100), 10 ciclos = 1 segundo)
void debug(int cada_ciclos = 10) {
  static int ciclos = 0;
  if (!debug_activo) return;
  ciclos++;
  if (ciclos < cada_ciclos) return;
  ciclos = 0;

  Serial.print("l_pulsador: "); Serial.println(pulsador ? "True" : "False");
  Serial.print("l_luz: ");      Serial.print(luz);     Serial.print("% (raw "); Serial.print(luz_raw);     Serial.println(")");
  Serial.print("l_humedad: ");  Serial.print(humedad); Serial.print("% (raw "); Serial.print(humedad_raw); Serial.println(")");
  Serial.print("l_temp: ");     Serial.print(temp, 1); Serial.print("C (raw "); Serial.print(temp_raw);    Serial.println(")");
  if (mpu_valido) {
    Serial.print("l_mpu ACC:["); Serial.print(ax, 2); Serial.print(","); Serial.print(ay, 2); Serial.print(","); Serial.print(az, 2);
    Serial.print("] GYR:[");     Serial.print(gx, 1); Serial.print(","); Serial.print(gy, 1); Serial.print(","); Serial.print(gz, 1);
    Serial.println("]");
    Serial.print("l_mpu MAG ACC:"); Serial.print(magnitud_acelerometro, 2); Serial.print(" (raw "); Serial.print(magnitud_acelerometro_raw, 2); Serial.println(")");
    Serial.print("GYR:"); Serial.print(magnitud_gyroscopio, 2); Serial.print(" (raw "); Serial.print(magnitud_gyroscopio_raw, 2); Serial.println(")");
    Serial.print("TOTAL:"); Serial.print(magnitud_total, 2); Serial.print(" (raw "); Serial.print(magnitud_total_raw, 2); Serial.println(")");
  } else {
    Serial.println("l_mpu: ERR");
  }
  Serial.print("l_bt: ");       Serial.println(bt_msg.length() ? bt_msg : "-");
  Serial.print("bomba: ");      Serial.println(bomba_activa ? "ON" : "OFF");
  Serial.println("---");
}

void imprimir_ayuda() {
  Serial.println(F("\n=== TEST DE COMPONENTES ==="));
  Serial.println(F("d : alternar debug de sensores"));
  Serial.println(F("p : alternar minibomba ON/OFF"));
  Serial.println(F("r/g/b/0 : LED RGB Rojo / Verde / Azul / Apagado"));
  Serial.println(F("z : probar zumbador"));
  Serial.println(F("o : actualizar OLED (INFO)"));
  Serial.println(F("c : probar caras del tamagotchi (auto / 0..7)"));
  Serial.println(F("t : enviar PING por Bluetooth"));
  Serial.println(F("h : mostrar ayuda\n"));
}

void procesar_comandos_serial() {
  if (!Serial.available()) return;
  char cmd = Serial.read();
  if (cmd == '\n' || cmd == '\r') return;

  switch (cmd) {
    case 'd': debug_activo = !debug_activo; Serial.println(debug_activo ? "Debug ON" : "Debug OFF"); break;
    case 'p': controlar_bomba(!bomba_activa); Serial.println(bomba_activa ? "Bomba ON" : "Bomba OFF"); break;
    case 'r': actualizar_leds(255, 0, 0);   Serial.println("RGB: ROJO"); break;
    case 'g': actualizar_leds(0, 255, 0);   Serial.println("RGB: VERDE"); break;
    case 'b': actualizar_leds(0, 0, 255);   Serial.println("RGB: AZUL"); break;
    case '0': actualizar_leds(0, 0, 0);     Serial.println("RGB: OFF"); break;
    case 'z': sonar_buzzer(1000, 250);      Serial.println("Buzzer: beep"); break;
    case 'o':
      mostrarMensaje_INFO(luz, humedad, temp, pulsador, mpu_valido, ax, ay, bomba_activa, bt_msg);
      Serial.println(oled_ok ? "OLED: INFO" : "OLED: no detectada");
      break;
    case 'c':
      cara_forzada = (cara_forzada >= 7) ? -1 : cara_forzada + 1;
      Serial.print("Cara: "); Serial.println(cara_forzada < 0 ? "AUTO" : String(cara_forzada));
      break;
    case 't': enviar_bt("PING_TAMAGOTCHI"); Serial.println("BT: PING enviado"); break;
    case 'h': imprimir_ayuda(); break;
    default:  break;
  }
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin(BT_NOMBRE);
  SerialBT.setTimeout(20);

  pinMode(p_pulsador, INPUT_PULLUP);
  pinMode(p_bomba, OUTPUT);
  pinMode(p_buzzer, OUTPUT);
  for (int i = 0; i < 3; i++) pinMode(p_leds[i], OUTPUT);

  controlar_bomba(false);
  actualizar_leds(0, 0, 0);

  Wire.begin(p_sda, p_scl);
  oled_ok = iniciar_pantalla();
  mpu_ok  = mpu_init();

  Serial.print("OLED: "); Serial.println(oled_ok ? "OK" : "ERR");
  Serial.print("MPU:  "); Serial.println(mpu_ok  ? "OK" : "ERR");
  mostrarMensaje_INFO(luz, humedad, temp, pulsador, mpu_valido, ax, ay, bomba_activa, bt_msg);
  imprimir_ayuda();
}

/*
Funcionamiento del tamagochi:

Indicador de hidratacion
Inicador de alimentacion
Indicador de energia
Indicador de Actividad fisica

*/

// Variables de estado del tamagotchi
int hidratacion = 75; // Estimulo -> Beber agua
int alimentacion = 75; // Estimulo -> Comer
int energia = 75; // Estimulo -> Dormir
int actividad_fisica = 75; // Estimulo -> Ejercicio

bool esta_durmiendo = false; // Estado de sueño
bool esta_ejercitandose = false; // Estado de ejercicio
bool ya_orino = false; // Para controlar que solo orine una vez por ciclo
bool ya_canto = false;

// Banderas para enviar avisos BT solo cuando cambia el estado
bool tiene_sed_ant = false;
bool tiene_hambre_ant = false;
bool esta_cansado_ant = false;
bool mucha_energia_ant = false;
bool necesita_ejercicio_ant = false;
bool hidratacion_llena_ant = false;

void comer(){
  if(alimentacion < 100){
    alimentacion += random(20,60);
    if(alimentacion > 100) alimentacion = 100;
    enviar_bt("Comiendo... Alimentacion: " + String(alimentacion));
    animar_comiendo();
  } else {
    enviar_bt("Alimentacion completa");
  }
}

void tomar_agua(){
  if(hidratacion < 100){
    hidratacion += random(min(humedad, 20), max(humedad, 40));
    if(hidratacion > 100) hidratacion = 100;
    hidratacion_llena_ant = false;
    enviar_bt("Bebiendo agua... Hidratacion: " + String(hidratacion));
    animar_tomando_agua();
  } else if (!hidratacion_llena_ant) {
    hidratacion_llena_ant = true;
    enviar_bt("Hidratacion completa");
  }
}

void cantar(){
  enviar_bt("Cantando... Energia: " + String(energia));
  reproducir_cancion(p_buzzer);
}

void actualizar_variables_estado(){
  // Con delay(100) en loop(), 20 ciclos = ~2 segundos entre cada actualización
  static int ciclos = 0;
  ciclos++;
  if (ciclos < 20) return;
  ciclos = 0;

  // Disminuir variables con el tiempo (random(1, 3) devuelve 1 o 2)
  hidratacion -= random(1, 4);
  if (temp > 30) hidratacion -= random(1, 3); // Si hace calor, se deshidrata mas rapido
  alimentacion -= random(1, 3);
  energia -= random(1, 3);
  actividad_fisica -= random(1, 3);

  // Si el tamagochi esta durmiendo, aumenta la energia; si esta ejercitandose, aumenta la actividad fisica
  if (esta_durmiendo) {
    energia += random(5, 11);
    alimentacion -= random(1, 3);
  } else if (esta_ejercitandose) {
    actividad_fisica += random(20, 41);
    hidratacion -= random(1, 3);
  }

  hidratacion      = constrain(hidratacion, 0, 100);
  alimentacion     = constrain(alimentacion, 0, 100);
  energia          = constrain(energia, 0, 100);
  actividad_fisica = constrain(actividad_fisica, 0, 100);
}

void reaccionar_estado() {
  bool tiene_sed = (hidratacion < 20);
  if (tiene_sed && !tiene_sed_ant) {
    enviar_bt("El tamagochi tiene sed");
    ya_orino = false;
  } else if (hidratacion > 90 && !ya_orino) {
    enviar_bt("El tamagochi necesita ir al baño");
    controlar_bomba(true);
    delay(3000);
    controlar_bomba(false);
    ya_orino = true;
  }
  tiene_sed_ant = tiene_sed;

  bool tiene_hambre = (alimentacion < 20);
  if (tiene_hambre && !tiene_hambre_ant) {
    enviar_bt("El tamagochi tiene hambre");
    actualizar_leds(255, 0, 0); // Rojo
    ya_canto = false;
  } else if (alimentacion > 90 && !ya_canto) {
    enviar_bt("El tamagochi esta feliz y canta");
    cantar();
    ya_canto = true;
  }
  tiene_hambre_ant = tiene_hambre;

  bool esta_cansado = (energia < 20);
  bool mucha_energia = (energia > 90);
  if (esta_cansado && !esta_cansado_ant) {
    enviar_bt("El tamagochi esta cansado");
    actualizar_leds(0, 0, 0); // Apagados
  } else if (mucha_energia && !mucha_energia_ant) {
    enviar_bt("El tamagochi esta con mucha energia");
    actualizar_leds(0, 255, 0); // Verde
  }
  esta_cansado_ant = esta_cansado;
  mucha_energia_ant = mucha_energia;

  bool necesita_ejercicio = (actividad_fisica < 20);
  if (necesita_ejercicio && !necesita_ejercicio_ant) {
    enviar_bt("El tamagochi necesita hacer ejercicio");
    actualizar_leds(0, 0, 255); // Azul
  }
  necesita_ejercicio_ant = necesita_ejercicio;
}

void loop() {
  leer_entradas();

  // Acciones del usuario

  // Presionar el pulsador para comer
  if (pulsador && !pulsador_ant) {
    comer();
  }

  // Si el sensor de humedad se activa significa que el tamagochi esta tomando agua
  if(humedad > 50){
    tomar_agua();
  }

  // Si la luz es baja o la energia es baja y el tamagochi no esta durmiendo, entonces entra en estado de sueño
  if((luz < 20 || energia < 20) && !esta_durmiendo && !esta_ejercitandose){
    esta_durmiendo = true;
  }else if(esta_durmiendo){
    if(luz > 50 || esta_ejercitandose || pulsador || humedad > 50){
      esta_durmiendo = false;
    }
  }

  // Ejercicio
  if(magnitud_total > 50 && !esta_ejercitandose && !esta_durmiendo){
    esta_ejercitandose = true;
  }else if(esta_ejercitandose){
    if(magnitud_total < 20){
      esta_ejercitandose = false;
    }
  }

  actualizar_variables_estado();
  reaccionar_estado();
  actualizar_cara(esta_durmiendo, esta_ejercitandose, alimentacion, hidratacion, energia, actividad_fisica);
  procesar_comandos_serial();
  debug(10);

  // Guardar estado anterior del pulsador
  pulsador_ant = pulsador;
  delay(100); // 10 ciclos por segundo + antirrebote del pulsador
}
