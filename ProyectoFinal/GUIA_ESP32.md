# Guía de conexión — ESP32 DevKit V1 (30 pines)

## 1. Placa usada

Este proyecto usa una **DOIT ESP32 DevKit V1 de 30 pines**, normalmente equipada con un módulo **ESP-WROOM-32**.

PlatformIO ya está configurado así:

```ini
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
monitor_speed = 115200
```

> Al cablear, seguir la etiqueta **GPIO** impresa junto al pin (por ejemplo, `34`, `G34` o `GPIO34`), no contar posiciones desde una esquina. Algunos fabricantes cambian la orientación de las etiquetas.

## 2. Pines verificados

Todos los GPIO usados están expuestos en el modelo de **30 pines** y evitan los pines de la memoria flash y los pines delicados de arranque.

| Función | GPIO | Etiqueta habitual | Motivo / restricción |
| :--- | :---: | :---: | :--- |
| LDR | `34` | `34` | ADC1; solo entrada; sin `INPUT_PULLUP` |
| Humedad | `35` | `35` | ADC1; solo entrada; sin `INPUT_PULLUP` |
| Temperatura | `32` | `32` | ADC1 |
| Pulsador | `14` | `14` | Entrada con `INPUT_PULLUP` |
| I2C SDA | `21` | `21` | Bus compartido OLED/MPU6050 |
| I2C SCL | `22` | `22` | Bus compartido OLED/MPU6050 |
| Bomba (control) | `27` | `27` | Salida al transistor/MOSFET, nunca a la bomba directamente |
| Zumbador | `25` | `25` | Salida |
| LED rojo | `26` | `26` | PWM |
| LED verde | `18` | `18` | PWM |
| LED azul | `19` | `19` | PWM |

No cambiar las entradas analógicas a GPIO de **ADC2**: Bluetooth/Wi-Fi puede impedir sus lecturas. No usar `GPIO 6..11` porque están conectados a la memoria flash. Conviene evitar `0`, `2`, `12` y `15` porque intervienen en el arranque.

## 3. Cableado

Unir todos los **GND**: ESP32, sensores y fuente externa de la bomba.

| Componente | Conexión |
| :--- | :--- |
| **LDR** | Hacer un divisor: `3V3 → LDR → GPIO34 → resistencia de 10 kΩ → GND`. No conectar una LDR sola entre `3V3` y `GND`. |
| **Sensor de humedad** | `AO → GPIO35`, `VCC → 3V3`, `GND → GND`. Usar la salida analógica `AO`, no `DO`. |
| **LM35 o TMP36** | `OUT → GPIO32`, `VCC → 3V3`, `GND → GND`. En el código usar `TEMP_OFFSET_MV = 0` para LM35 o `500` para TMP36. Revisar el orden de patas de la versión comprada. |
| **Pulsador** | Un terminal a `GPIO14` y el otro a `GND`; no necesita resistencia externa. |
| **OLED SSD1306 I2C** | `SDA → GPIO21`, `SCL → GPIO22`, `VCC → 3V3`, `GND → GND`; dirección esperada `0x3C`. |
| **MPU6050** | `SDA → GPIO21`, `SCL → GPIO22`, `VCC → 3V3`, `GND → GND`, `AD0 → GND`; dirección esperada `0x68`. |
| **Minibomba 5 V** | `GPIO27 → resistencia/base o gate del transistor/MOSFET`. Alimentar la bomba con fuente externa de 5 V y poner un diodo 1N4007 en antiparalelo: cátodo al positivo. **No alimentar ni manejar la bomba desde un GPIO.** |
| **Zumbador** | Zumbador pequeño de 3.3 V: `+ → GPIO25`, `- → GND`. Si consume más de 12 mA, usar transistor. |
| **LED RGB de cátodo común** | R, G y B a `GPIO26`, `GPIO18` y `GPIO19`, cada color con resistencia de 220–330 Ω; pata común a `GND`. El código no está invertido para ánodo común. |
| **Bluetooth** | Es interno; no conectar un módulo Bluetooth externo. Nombre: `Tamagotchi_ESP32`. |

Los GPIO trabajan a **3.3 V** y no toleran 5 V en sus entradas.

## 4. Carga y monitor serie

1. Conectar la placa por un cable USB que transmita datos.
2. En PlatformIO ejecutar **Upload**.
3. Abrir el monitor a `115200` baudios.
4. Si aparece `Connecting...`, mantener pulsado **BOOT**, soltarlo cuando empiece la escritura y luego pulsar **EN** si la placa no reinicia sola.

Si hay más de un puerto, configurar temporalmente `upload_port` y `monitor_port` en `platformio.ini`; no hace falta agregarlos normalmente.

## 5. Prueba de Bluetooth

1. Subir `src/main.cpp`.
2. En Android, emparejar `Tamagotchi_ESP32` y abrir una terminal **Bluetooth Classic SPP**.
3. Enviar `HOLA`; el monitor debe mostrar `l_bt: HOLA`.
4. Escribir `t` en el monitor; el celular debe recibir `PING_TAMAGOTCHI`.

Bluetooth Classic SPP no funciona directamente con iOS. Esta guía corresponde al ESP32 clásico con ESP-WROOM-32, no a variantes C3, S2 o S3.

## 6. Comandos de prueba

| Tecla | Prueba |
| :---: | :--- |
| `d` | Activa/desactiva las lecturas periódicas |
| `p` | Enciende/apaga la bomba |
| `r`, `g`, `b`, `0` | LED rojo, verde, azul o apagado |
| `z` | Prueba el zumbador |
| `o` | Actualiza la OLED |
| `t` | Envía un mensaje por Bluetooth |
| `h` | Muestra la ayuda |

Anotar los valores `raw` reales en seco/mojado y con luz/oscuridad. Después ajustar `LUZ_MIN_RAW`, `LUZ_MAX_RAW`, `HUM_SECO_RAW` y `HUM_MOJADO_RAW` en `src/main.cpp`. Si la bomba funciona invertida, cambiar `BOMBA_ACTIVA_HIGH` a `false`.
