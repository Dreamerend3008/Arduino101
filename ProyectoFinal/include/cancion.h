#pragma once
#include <Arduino.h>

// Notas musicales (Hz)
constexpr float R = 0;

constexpr float c0 = 16.352f;
constexpr float d0 = 18.354f;
constexpr float e0 = 20.602f;
constexpr float f0 = 21.827f;
constexpr float g0 = 24.500f;
constexpr float a0 = 27.500f;
constexpr float b0 = 30.868f;

constexpr float c3 = c0 * 8;
constexpr float d3 = d0 * 8;
constexpr float e3 = e0 * 8;
constexpr float f3 = f0 * 8;
constexpr float g3 = g0 * 8;
constexpr float a3 = a0 * 8;
constexpr float b3 = b0 * 8;
constexpr float e3b = (e3 + d3) / 2;
constexpr float b3b = (b3 + a3) / 2;
constexpr float a3b = (a3 + g3) / 2;

constexpr float c4 = c0 * 16;
constexpr float d4 = d0 * 16;
constexpr float e4 = e0 * 16;
constexpr float f4 = f0 * 16;
constexpr float g4 = g0 * 16;
constexpr float a4 = a0 * 16;
constexpr float b4 = b0 * 16;
constexpr float c4s = (c4 + d4) / 2;
constexpr float f4s = (f4 + g4) / 2;
constexpr float f4b = (f4 + e4) / 2;
constexpr float b4b = (b4 + a4) / 2;
constexpr float e4b = (e4 + d4) / 2;
constexpr float a4b = (a4 + g4) / 2;

constexpr float c5 = c0 * 32;
constexpr float d5 = d0 * 32;
constexpr float e5 = e0 * 32;
constexpr float f5 = f0 * 32;
constexpr float g5 = g0 * 32;
constexpr float a5 = a0 * 32;
constexpr float b5 = b0 * 32;
constexpr float a5b = (a5 + g5) / 2;
constexpr float b5b = (b5 + a5) / 2;
constexpr float d5b = (d5 + c5) / 2;
constexpr float e5b = (e5 + d5) / 2;
constexpr float c5s = (c5 + d5) / 2;
constexpr float f5s = (f5 + g5) / 2;

constexpr float c6 = c0 * 64;
constexpr float d6 = d0 * 64;
constexpr float e6 = e0 * 64;
constexpr float f6 = f0 * 64;
constexpr float g6 = g0 * 64;
constexpr float a6 = a0 * 64;
constexpr float b6 = b0 * 64;
constexpr float d6b = (d6 + c6) / 2;

static const float melody[][2] = {
{g4, 1},
{c6, 4}, {R, 512}, {c6, 16 / 3}, {b5b, 16},
{c6, 8}, {b5b, 8}, {a5b, 8}, {g5, 8},
{b5b, 16}, {a5b, 16}, {g5, 16}, {f5, 16}, {e5b, 8}, {f5, 8},
{g5, 2},

{b5b, 4}, {R, 512}, {b5b, 16 / 3}, {a5b, 16},
{g5, 8}, {f5, 8}, {e5b, 8}, {f5, 8},
{g5, 16}, {e5b, 16}, {c5, 16}, {e5b, 16}, {g5, 8}, {f5, 8},
{e5b, 4}, {d5, 4},
//
{c6, 4}, {R, 512}, {c6, 16 / 3}, {b5b, 16},
{c6, 8}, {b5b, 8}, {a5b, 8}, {g5, 8},
{b5b, 16}, {a5b, 16}, {g5, 16}, {f5, 16}, {e5b, 8}, {f5, 8},
{g5, 2},
//
{b5b, 4}, {R, 512}, {b5b, 16 / 3}, {a5b, 16},
{g5, 8}, {f5, 8}, {e5b, 8}, {f5, 8},
{g5, 16}, {e5b, 16}, {c5, 16}, {e5b, 16}, {g5, 8}, {f5, 8},
{e5b, 4}, {d5, 4},
{R, 8}, {c5, 16}, {d5, 16}, {e5b, 8}, {c5, 8},
{f5, 8}, {e5b, 8}, {d5, 8}, {c5, 8},
{b4b, 16 / 3}, {a4b, 16}, {g4, 8}, {a4b, 8},

{b4b, 4}, {f4, 4},
{R, 8}, {d4, 16}, {e4b, 16}, {f4, 8}, {d4, 8},
{g4, 8}, {f4, 8}, {e4b, 8}, {d4, 8},
{e4b, 4}, {R, 512}, {e4b, 4},
{d4, 2},
{R, 8}, {c5, 16}, {d5, 16}, {e5b, 8}, {c5, 8},
{f5, 8}, {e5b, 8}, {d5, 8}, {c5, 8},
{b4b, 16}, {a4b, 16}, {g4, 16}, {a4b, 16}, {b4b, 8}, {a4b, 8},

{g4, 4}, {e4b, 4},
{R, 8}, {d4, 16}, {e4b, 16}, {f4, 8}, {d4, 8},
{g4, 8}, {f4, 8}, {e4b, 8}, {d4, 8},
{d4, 8}, {R, 512}, {d4, 16}, {e4b, 16}, {d4, 8}, {R, 512}, {d4, 16}, {e4b, 16},
{d4, 8}, {R, 512}, {d4, 16}, {e4b, 16}, {d4, 8}, {R, 512}, {d4, 16}, {e4b, 16},
{d4, 16}, {e4b, 16}, {d4, 16}, {e4b, 16}, {d4, 16}, {e4b, 16}, {d4, 16}, {e4b, 16},

{d4, 16}, {e4b, 16}, {d4, 16}, {e4b, 16}, {d4, 16}, {e4b, 16}, {d4, 16}, {e4b, 16},
{g4, 16}, {a4b, 16}, {g4, 16}, {a4b, 16}, {g4, 16}, {a4b, 16}, {g4, 16}, {a4b, 16},
{g4, 16}, {a4, 16}, {b4, 16}, {c5, 16}, {d5, 16}, {e5b, 16}, {f5, 16}, {g5, 16},
{c6, 4}, {R, 512}, {c6, 16 / 3}, {b5b, 16},
{c6, 8}, {b5b, 8}, {a5b, 8}, {g5, 8},
};

// ponytail: reproducción bloqueante tal cual el original; pasar a máquina de estados con millis() si se necesita leer sensores mientras canta
inline void play_tone(uint8_t port, float f, long t) {
  if (f > 0) {
    const int T = (int)(1000000.0f / f);
    int d = T / 2;
    int count = t / T;
    for (int i = 0; i < count; i++) {
      digitalWrite(port, HIGH);
      delayMicroseconds(d);
      digitalWrite(port, LOW);
      delayMicroseconds(d);
    }
  } else {
    delay(t / 1000);
  }
}

inline void reproducir_cancion(uint8_t pin, int bpm = 200) {
  const long MINUTE = 60000000L;
  const long T = (MINUTE / bpm) * 4;
  const int n = sizeof(melody) / sizeof(melody[0]);
  for (int i = 0; i < n; i++) {
    play_tone(pin, melody[i][0], (long)(T / melody[i][1]));
    delay(50);
  }
}
