# Guía ESP32 y pruebas de circuito

## 1. Diferencias clave frente a Arduino Uno

- **GPIOs a 3.3 V:** nunca conectar 5 V directos a un pin de entrada.
- **ADC de 12 bits (`0..4095`):** usar solo pines **ADC1 (`32, 33, 34, 35, 36, 39`)** porque el Bluetooth interno deshabilita ADC2.
- **Pines solo entrada (`34, 35, 36, 39`):** no sirven como salida ni tienen `INPUT_PULLUP`.
- **Pines reservados:** evitar `6..11` (flash) y `0, 2, 12, 15` (arranque).
- **I2C:** `GPIO 21` (SDA) y `GPIO 22` (SCL), compartidos entre OLED y MPU6050.
- **Bluetooth:** integrado (`BluetoothSerial.h`), sin módulo externo.
- **Monitor Serial:** `115200` baudios. Si PlatformIO se queda en `Connecting...` al subir, mantener pulsado **BOOT** 2 s hasta que inicie la carga.

## 2. Conexiones (`src/main.cpp`)

Unir todos los **GND** entre sí (ESP32, sensores y fuente externa de la bomba).

| Componente | Pin ESP32 | Alimentación / Notas |
| :--- | :--- | :--- |
| **LDR (luz)** | `34` | `3V3` y `GND` |
| **Sensor humedad** | `35` | `3V3` y `GND` |
| **Sensor temp. (LM35/TMP36)** | `32` | `3V3` y `GND` (`TEMP_OFFSET_MV = 0` LM35, `500` TMP36) |
| **Pulsador** | `14` | Entre `GPIO 14` y `GND` (`INPUT_PULLUP` interno) |
| **OLED SSD1306 (`0x3C`)** | `21` (SDA), `22` (SCL) | `3V3` y `GND` |
| **MPU6050 (`0x68`)** | `21` (SDA), `22` (SCL) | `3V3`, `GND`, `AD0` libre o a `GND` |
| **Minibomba** | `27` | Vía transistor/relé + **diodo 1N4007 en antiparalelo** + fuente externa 5 V |
| **Zumbador** | `25` | `+` a `GPIO 25`, `-` a `GND` |
| **LED RGB** | `26` (R), `18` (G), `19` (B) | Resistencia 220–330 Ω por color; común a `GND` |
| **Bluetooth** | *(interno)* | Sin cables (`Tamagotchi_ESP32`) |

## 3. Probar el Bluetooth interno

1. Sube `src/main.cpp` y abre el Monitor Serial a `115200`.
2. En un celular **Android**, ve a *Ajustes → Bluetooth* y empareja **`Tamagotchi_ESP32`** (iOS no soporta Bluetooth Classic SPP).
3. Abre la app **Serial Bluetooth Terminal** → *Devices* → pestaña **Bluetooth Classic** → selecciona `Tamagotchi_ESP32`.
4. **Celular → ESP32:** envía `HOLA` desde la app; en el Monitor Serial debe salir `l_bt: HOLA`.
5. **ESP32 → Celular:** escribe **`t`** + Enter en el Monitor Serial; en el celular debe llegar `PING_TAMAGOTCHI`.

*(Si no aparece, pulsa **EN/RST** en la ESP32. Si se reinicia al conectar, cambia a un cable/puerto USB con más corriente.)*

## 4. Comandos de prueba en Monitor Serial (`115200`)

| Tecla | Prueba |
| :---: | :--- |
| `d` | Activa / desactiva lecturas periódicas de sensores |
| `p` | Enciende / apaga minibomba (si trabaja invertida, `BOMBA_ACTIVA_HIGH = false`) |
| `r` / `g` / `b` / `0` | LED RGB Rojo / Verde / Azul / Apagado |
| `z` | Pitido en el zumbador |
| `o` | Actualiza la OLED (`mostrarMensaje_INFO`) |
| `t` | Envía `PING_TAMAGOTCHI` por Bluetooth |
| `h` | Muestra ayuda |

Al probar luz y humedad, anota los valores `raw` mínimo/máximo reales y ajústalos en `LUZ_MIN_RAW`, `LUZ_MAX_RAW`, `HUM_SECO_RAW` y `HUM_MOJADO_RAW` en `src/main.cpp`.
