#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA 64
#define OLED_RESET -1
#define DIRECCION_OLED 0x3C

Adafruit_SSD1306 oled(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, OLED_RESET);
bool oled_ok = false;
int  cara_forzada = -1; // -1 = automático, 0..7 = prueba manual de caras

enum EstadoCara {
  CARA_FELIZ = 0,
  CARA_TRISTE,
  CARA_ENOJADO,
  CARA_DORMIDO,
  CARA_HAMBRE,
  CARA_SED,
  CARA_EJERCICIO,
  CARA_EJERCITANDO
};

inline bool iniciar_pantalla() {
  oled_ok = oled.begin(SSD1306_SWITCHCAPVCC, DIRECCION_OLED);
  return oled_ok;
}

inline void mostrarMensaje_INFO(int luz, int humedad, float temp, bool pulsador,
                                bool mpu_valido, float ax, float ay,
                                bool bomba_activa, const String& bt_msg) {
  if (!oled_ok) return;
  oled.clearDisplay();
  oled.setTextSize(1);

  oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  oled.setCursor(0, 0);
  oled.println("   TEST TAMAGOTCHI   ");

  oled.setTextColor(SSD1306_WHITE);

  oled.setCursor(0, 16);
  oled.print("L:"); oled.print(luz); oled.print("% ");
  oled.print("H:"); oled.print(humedad); oled.print("% ");
  oled.print("T:"); oled.print((int)temp); oled.println("C");

  oled.setCursor(0, 26);
  oled.print("Btn:"); oled.print(pulsador ? "1" : "0");
  oled.print(" MPU:");
  if (mpu_valido) {
    oled.print(ax, 1); oled.print(","); oled.println(ay, 1);
  } else {
    oled.println("ERR");
  }

  oled.setCursor(0, 36);
  oled.print("Bomba:"); oled.print(bomba_activa ? "ON " : "OFF ");
  oled.print("BT:"); oled.println(bt_msg.length() ? bt_msg : "-");

  oled.drawRect(14, 52, 100, 8, SSD1306_WHITE);
  int anchoBarra = constrain(map(humedad, 0, 100, 0, 96), 0, 96);
  oled.fillRect(16, 54, anchoBarra, 4, SSD1306_WHITE);

  oled.display();
}

// ponytail: primitivas Adafruit_GFX en vez de bitmaps PROGMEM; usar bitmaps si se quiere pixel art complejo
inline void dibujar_arco(int cx, int cy, int rx, int ry, bool hacia_abajo) {
  for (int dx = -rx; dx <= rx; dx++) {
    float t = (float)dx / rx;
    int dy = (int)roundf(ry * (1.0f - t * t));
    int y = hacia_abajo ? (cy + dy) : (cy - dy);
    oled.drawPixel(cx + dx, y, SSD1306_WHITE);
    oled.drawPixel(cx + dx, y + 1, SSD1306_WHITE);
  }
}

inline void dibujar_ojo(int x, int y, bool cerrado) {
  if (cerrado) {
    oled.fillRect(x - 7, y, 15, 2, SSD1306_WHITE);
    return;
  }
  oled.fillCircle(x, y, 7, SSD1306_WHITE);
  oled.fillCircle(x + 2, y - 2, 2, SSD1306_BLACK);
}

inline void dibujar_etiqueta_cara(const char* texto) {
  int x = (ANCHO_PANTALLA - (int)strlen(texto) * 6) / 2;
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(x, 55);
  oled.print(texto);
}

inline void dibujar_cara_feliz(bool anim) {
  static int paso_parpadeo = 0;
  paso_parpadeo = (paso_parpadeo + 1) % 6;
  bool parpadeo = (paso_parpadeo == 0);
  dibujar_ojo(44, 21, parpadeo);
  dibujar_ojo(84, 21, parpadeo);
  oled.fillRoundRect(25, 29, 8, 4, 2, SSD1306_WHITE);
  oled.fillRoundRect(95, 29, 8, 4, 2, SSD1306_WHITE);
  dibujar_arco(64, 35, 14, anim ? 8 : 7, true);
  dibujar_etiqueta_cara("FELIZ");
}

inline void dibujar_cara_triste(bool anim) {
  dibujar_ojo(44, 23, false);
  dibujar_ojo(84, 23, false);
  oled.drawLine(34, 13, 50, 9, SSD1306_WHITE);
  oled.drawLine(34, 14, 50, 10, SSD1306_WHITE);
  oled.drawLine(78, 9, 94, 13, SSD1306_WHITE);
  oled.drawLine(78, 10, 94, 14, SSD1306_WHITE);
  int ly = anim ? 35 : 32;
  oled.fillCircle(37, ly, 2, SSD1306_WHITE);
  oled.drawPixel(37, ly - 3, SSD1306_WHITE);
  dibujar_arco(64, 44, 11, 5, false);
  dibujar_etiqueta_cara("TRISTE");
}

inline void dibujar_cara_enojado(bool anim) {
  dibujar_ojo(44, 23, false);
  dibujar_ojo(84, 23, false);
  oled.fillTriangle(34, 12, 54, 12, 54, 21, SSD1306_BLACK);
  oled.fillTriangle(74, 12, 94, 12, 74, 21, SSD1306_BLACK);
  oled.drawLine(34, 12, 53, 19, SSD1306_WHITE);
  oled.drawLine(34, 13, 53, 20, SSD1306_WHITE);
  oled.drawLine(94, 12, 75, 19, SSD1306_WHITE);
  oled.drawLine(94, 13, 75, 20, SSD1306_WHITE);
  for (int i = 0; i < 4; i++) {
    int x0 = 50 + i * 7;
    oled.drawLine(x0, (i % 2 == 0) ? 41 : 37, x0 + 7, (i % 2 == 0) ? 37 : 41, SSD1306_WHITE);
  }
  int vx = 108, vy = anim ? 11 : 12;
  oled.drawLine(vx - 3, vy - 3, vx - 1, vy - 1, SSD1306_WHITE);
  oled.drawLine(vx + 3, vy - 3, vx + 1, vy - 1, SSD1306_WHITE);
  oled.drawLine(vx - 3, vy + 3, vx - 1, vy + 1, SSD1306_WHITE);
  oled.drawLine(vx + 3, vy + 3, vx + 1, vy + 1, SSD1306_WHITE);
  dibujar_etiqueta_cara("ENOJADO");
}

inline void dibujar_cara_dormido(int frame) {
  // Ojos cerrados relajados
  dibujar_arco(44, 23, 8, 3, true);
  dibujar_arco(84, 23, 8, 3, true);
  // Respiración en 4 pasos (inhalar / exhalar)
  const int radios[4] = {2, 3, 5, 3};
  oled.drawCircle(64, 39, radios[frame % 4], SSD1306_WHITE);
  // Zzz subiendo progresivamente
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
  int f = frame % 4;
  if (f >= 1) { oled.setCursor(96, 22 - f);  oled.print("z"); }
  if (f >= 2) { oled.setCursor(105, 13 - f); oled.print("z"); }
  if (f >= 3) { oled.setCursor(114, 5);      oled.print("Z"); }
  dibujar_etiqueta_cara("DORMIDO");
}

inline void dibujar_cara_hambre(bool anim) {
  dibujar_ojo(44, 20, false);
  dibujar_ojo(84, 20, false);
  int h = anim ? 13 : 10;
  oled.drawRoundRect(52, 34, 24, h, 5, SSD1306_WHITE);
  oled.fillRoundRect(57, 34 + h - 4, 14, 3, 1, SSD1306_WHITE);
  oled.drawFastVLine(74, 34 + h, anim ? 4 : 2, SSD1306_WHITE);
  oled.fillCircle(110, 17, 5, SSD1306_WHITE);
  oled.drawLine(110, 11, 113, 8, SSD1306_WHITE);
  dibujar_etiqueta_cara("HAMBRE");
}

inline void dibujar_cara_sed(bool anim) {
  dibujar_ojo(44, 22, false);
  dibujar_ojo(84, 22, false);
  oled.fillRect(36, 14, 17, 6, SSD1306_BLACK);
  oled.fillRect(76, 14, 17, 6, SSD1306_BLACK);
  oled.drawFastHLine(36, 20, 17, SSD1306_WHITE);
  oled.drawFastHLine(76, 20, 17, SSD1306_WHITE);
  oled.drawRoundRect(54, 34, 20, 8, 3, SSD1306_WHITE);
  oled.fillRoundRect(60, 40, 8, anim ? 7 : 5, 3, SSD1306_WHITE);
  int gy = anim ? 17 : 13;
  oled.fillTriangle(110, gy - 6, 106, gy, 114, gy, SSD1306_WHITE);
  oled.fillCircle(110, gy + 1, 4, SSD1306_WHITE);
  dibujar_etiqueta_cara("SED");
}

inline void dibujar_cara_ejercicio(bool anim) {
  oled.fillRect(28, 6, 72, 4, SSD1306_WHITE);
  oled.drawLine(100, 7, 106, anim ? 5 : 9, SSD1306_WHITE);
  oled.drawLine(100, 9, 105, anim ? 11 : 13, SSD1306_WHITE);
  dibujar_ojo(44, 22, false);
  dibujar_ojo(84, 22, false);
  oled.drawLine(36, 13, 51, 15, SSD1306_WHITE);
  oled.drawLine(92, 13, 77, 15, SSD1306_WHITE);
  oled.drawFastHLine(52, 36, 24, SSD1306_WHITE);
  dibujar_arco(64, 36, 12, 7, true);
  int dy = anim ? 16 : 22;
  oled.fillRect(104, dy - 4, 3, 9, SSD1306_WHITE);
  oled.fillRect(117, dy - 4, 3, 9, SSD1306_WHITE);
  oled.drawFastHLine(107, dy, 10, SSD1306_WHITE);
  oled.drawFastHLine(107, dy + 1, 10, SSD1306_WHITE);
  dibujar_etiqueta_cara("EJERCICIO");
}

inline void dibujar_cara_ejercitando(int frame) {
  int salto = (frame % 2 == 0) ? -2 : 2;
  // Banda deportiva animada
  oled.fillRect(30, 8 + salto, 68, 4, SSD1306_WHITE);
  oled.drawLine(98, 9 + salto, 105, 6 + salto + (frame % 2) * 3, SSD1306_WHITE);
  oled.drawLine(98, 11 + salto, 104, 13 + salto + (frame % 2) * 2, SSD1306_WHITE);
  // Ojos concentrados
  dibujar_ojo(46, 23 + salto, false);
  dibujar_ojo(82, 23 + salto, false);
  oled.drawLine(38, 14 + salto, 52, 16 + salto, SSD1306_WHITE);
  oled.drawLine(90, 14 + salto, 76, 16 + salto, SSD1306_WHITE);
  // Boca con esfuerzo / sonrisa
  if (frame % 2 == 0) {
    oled.drawRoundRect(56, 36 + salto, 16, 7, 3, SSD1306_WHITE);
  } else {
    dibujar_arco(64, 37 + salto, 11, 5, true);
  }
  // Gota de sudor cayendo
  int sy = 18 + (frame % 4) * 4;
  oled.fillCircle(96, sy, 2, SSD1306_WHITE);
  // Dos mancuernas subiendo y bajando alternadas
  int ly = (frame % 2 == 0) ? 16 : 32;
  int ry = (frame % 2 == 0) ? 32 : 16;
  oled.fillRect(6, ly - 4, 3, 9, SSD1306_WHITE);
  oled.fillRect(18, ly - 4, 3, 9, SSD1306_WHITE);
  oled.drawFastHLine(9, ly, 9, SSD1306_WHITE);
  oled.fillRect(107, ry - 4, 3, 9, SSD1306_WHITE);
  oled.fillRect(119, ry - 4, 3, 9, SSD1306_WHITE);
  oled.drawFastHLine(110, ry, 9, SSD1306_WHITE);
  dibujar_etiqueta_cara("EJERCITANDO");
}

inline void animar_comiendo() {
  if (!oled_ok) return;
  for (int f = 0; f < 4; f++) {
    oled.clearDisplay();
    // Ojos felices cerrados (^ ^)
    dibujar_arco(44, 23, 8, 4, false);
    dibujar_arco(84, 23, 8, 4, false);
    // Comida acercándose y desapareciendo al masticar
    if (f < 2) {
      int fx = 104 - f * 18;
      oled.fillCircle(fx, 38, 5, SSD1306_WHITE);
      oled.drawLine(fx, 32, fx + 3, 29, SSD1306_WHITE);
    }
    // Boca abriendo y masticando + migajas
    if (f % 2 == 0) {
      oled.fillCircle(64, 39, 8, SSD1306_WHITE);
      oled.fillCircle(64, 39, 5, SSD1306_BLACK);
    } else {
      oled.fillRect(54, 38, 20, 3, SSD1306_WHITE);
      oled.fillCircle(48, 36, 1, SSD1306_WHITE);
      oled.fillCircle(80, 41, 1, SSD1306_WHITE);
    }
    dibujar_etiqueta_cara("COMIENDO");
    oled.display();
    delay(220);
  }
}

inline void animar_tomando_agua() {
  if (!oled_ok) return;
  for (int f = 0; f < 3; f++) {
    oled.clearDisplay();
    // Ojos felices
    dibujar_arco(44, 22, 8, 4, false);
    dibujar_arco(84, 22, 8, 4, false);
    // Gota cayendo hacia la boca
    int gy = 10 + f * 9;
    oled.fillTriangle(64, gy - 5, 60, gy, 68, gy, SSD1306_WHITE);
    oled.fillCircle(64, gy + 1, 3, SSD1306_WHITE);
    // Boca tomando agua
    if (f < 2) {
      oled.drawCircle(64, 40, 6, SSD1306_WHITE);
    } else {
      dibujar_arco(64, 37, 11, 5, true);
    }
    dibujar_etiqueta_cara("BEBIENDO");
    oled.display();
    delay(180);
  }
}

inline EstadoCara obtener_estado_cara(bool esta_durmiendo, bool esta_ejercitandose,
                                      int alimentacion, int hidratacion,
                                      int energia, int actividad_fisica) {
  if (esta_durmiendo)      return CARA_DORMIDO;
  if (esta_ejercitandose)  return CARA_EJERCITANDO;

  int criticas = (alimentacion < 20) + (hidratacion < 20) + (energia < 20) + (actividad_fisica < 20);
  if (criticas >= 2)         return CARA_ENOJADO;
  if (alimentacion < 20)     return CARA_HAMBRE;
  if (hidratacion < 20)      return CARA_SED;
  if (actividad_fisica < 20) return CARA_EJERCICIO;
  if (energia < 35 || alimentacion < 40 || hidratacion < 40 || actividad_fisica < 40) return CARA_TRISTE;
  return CARA_FELIZ;
}

inline void mostrar_cara(EstadoCara estado, int frame) {
  if (!oled_ok) return;
  bool anim = (frame % 2 != 0);
  oled.clearDisplay();
  switch (estado) {
    case CARA_FELIZ:       dibujar_cara_feliz(anim);        break;
    case CARA_TRISTE:      dibujar_cara_triste(anim);       break;
    case CARA_ENOJADO:     dibujar_cara_enojado(anim);      break;
    case CARA_DORMIDO:     dibujar_cara_dormido(frame);     break;
    case CARA_HAMBRE:      dibujar_cara_hambre(anim);       break;
    case CARA_SED:         dibujar_cara_sed(anim);          break;
    case CARA_EJERCICIO:   dibujar_cara_ejercicio(anim);    break;
    case CARA_EJERCITANDO: dibujar_cara_ejercitando(frame); break;
  }
  oled.display();
}

inline void actualizar_cara(bool esta_durmiendo, bool esta_ejercitandose,
                            int alimentacion, int hidratacion,
                            int energia, int actividad_fisica,
                            int cada_ciclos = 4) {
  static int ciclos = 4; // dibuja en el primer ciclo
  static int frame = 0;
  ciclos++;
  if (ciclos < cada_ciclos) return;
  ciclos = 0;
  frame = (frame + 1) % 4;

  EstadoCara estado = (cara_forzada >= 0)
    ? (EstadoCara)cara_forzada
    : obtener_estado_cara(esta_durmiendo, esta_ejercitandose, alimentacion, hidratacion, energia, actividad_fisica);
  mostrar_cara(estado, frame);
}
