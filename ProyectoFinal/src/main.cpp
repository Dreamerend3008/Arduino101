#include <Arduino.h>
#include <Wire.h>
#include <BluetoothSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA 64
#define OLED_RESET -1
#define DIRECCION_OLED 0x3C
#define DIRECCION_MPU 0x68

Adafruit_SSD1306 oled(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, OLED_RESET);
BluetoothSerial SerialBT;

// De acuerdo con la ia son buenos pines para la sp 
int p_luz      = 34; // LDR 
int p_humedad  = 35; // Sensor de humedad
int p_temp     = 32; // Sensor de temperatura 

int p_pulsador = 14; // Pulsador a GND 
int p_sda      = 21; 
int p_scl      = 22;

// Salidas
int p_leds[3]  = {26, 18, 19}; // {R, G, B}
int p_bomba    = 27;           // Minibomba vía transistor/relé + diodo
int p_buzzer   = 25;           // Zumbador

// Constantes para calibrar
const char* BT_NOMBRE        = "Tamagotchi_ESP32";
const int   LUZ_MIN_RAW      = 0;
const int   LUZ_MAX_RAW      = 4095;
const int   HUM_SECO_RAW     = 4095;
const int   HUM_MOJADO_RAW   = 1200;
const float ADC_VREF_MV      = 3300.0f;
const float TEMP_OFFSET_MV   = 0.0f;  
const float TEMP_MV_POR_C    = 10.0f;
const bool  BOMBA_ACTIVA_HIGH = true; 

// ===================== VARIABLES DE LECTURA Y ESTADO =====================
int   luz_raw = 0, luz = 0;         // luz: 0..100 %
int   humedad_raw = 0, humedad = 0; // humedad: 0..100 %
int   temp_raw = 0; float temp = 0.0f; // °C
bool  pulsador = false;
bool  pulsador_ant = false;
float ax = 0, ay = 0, az = 0;       // Acelerómetro (g)
float gx = 0, gy = 0, gz = 0;       // Giroscopio (°/s)
String bt_msg = "";

int   leds[3] = {0, 0, 0};
bool  bomba_activa = false;
bool  oled_ok = false;
bool  mpu_ok  = false;
bool  mpu_valido = false;
bool  debug_activo = true;
unsigned long ultimo_debug = 0;

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

void mostrarMensaje_INFO() {
  if (!oled_ok) return;
  oled.clearDisplay();
  oled.setTextSize(1);

  // Título
  oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  oled.setCursor(0, 0);
  oled.println("   TEST TAMAGOTCHI   ");

  oled.setTextColor(SSD1306_WHITE);

  // Línea 1: Sensores analógicos
  oled.setCursor(0, 16);
  oled.print("L:"); oled.print(luz); oled.print("% ");
  oled.print("H:"); oled.print(humedad); oled.print("% ");
  oled.print("T:"); oled.print((int)temp); oled.println("C");

  // Línea 2: Digitales (pulsador + MPU)
  oled.setCursor(0, 26);
  oled.print("Btn:"); oled.print(pulsador ? "1" : "0");
  oled.print(" MPU:");
  if (mpu_valido) {
    oled.print(ax, 1); oled.print(","); oled.println(ay, 1);
  } else {
    oled.println("ERR");
  }

  // Línea 3: Bluetooth / Bomba
  oled.setCursor(0, 36);
  oled.print("Bomba:"); oled.print(bomba_activa ? "ON " : "OFF ");
  oled.print("BT:"); oled.println(bt_msg.length() ? bt_msg : "-");

  // Barra de progreso (humedad 0..100%)
  oled.drawRect(14, 52, 100, 8, SSD1306_WHITE);
  int anchoBarra = map(humedad, 0, 100, 0, 96);
  anchoBarra = constrain(anchoBarra, 0, 96);
  oled.fillRect(16, 54, anchoBarra, 4, SSD1306_WHITE);

  oled.display();
}

// Debug
void debug(int intervalo_ms) {
  if (!debug_activo || millis() - ultimo_debug < (unsigned long)intervalo_ms) return;
  ultimo_debug = millis();

  Serial.print("l_pulsador: "); Serial.println(pulsador ? "True" : "False");
  Serial.print("l_luz: ");      Serial.print(luz);     Serial.print("% (raw "); Serial.print(luz_raw);     Serial.println(")");
  Serial.print("l_humedad: ");  Serial.print(humedad); Serial.print("% (raw "); Serial.print(humedad_raw); Serial.println(")");
  Serial.print("l_temp: ");     Serial.print(temp, 1); Serial.print("C (raw "); Serial.print(temp_raw);    Serial.println(")");
  if (mpu_valido) {
    Serial.print("l_mpu ACC:["); Serial.print(ax, 2); Serial.print(","); Serial.print(ay, 2); Serial.print(","); Serial.print(az, 2);
    Serial.print("] GYR:[");     Serial.print(gx, 1); Serial.print(","); Serial.print(gy, 1); Serial.print(","); Serial.print(gz, 1);
    Serial.println("]");
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
    case 'o': mostrarMensaje_INFO();        Serial.println(oled_ok ? "OLED: INFO" : "OLED: no detectada"); break;
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
  oled_ok = oled.begin(SSD1306_SWITCHCAPVCC, DIRECCION_OLED);
  mpu_ok  = mpu_init();

  Serial.print("OLED: "); Serial.println(oled_ok ? "OK" : "ERR");
  Serial.print("MPU:  "); Serial.println(mpu_ok  ? "OK" : "ERR");
  mostrarMensaje_INFO();
  imprimir_ayuda();
}

void loop() {
  leer_entradas();

  if (pulsador && !pulsador_ant) {
    Serial.println("EVENTO: Pulsador presionado");
  }

  procesar_comandos_serial();
  debug(1000);

  // Guardar estado anterior del pulsador
  pulsador_ant = pulsador;
}
