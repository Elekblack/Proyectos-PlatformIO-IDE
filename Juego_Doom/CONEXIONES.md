# Doom-like para ESP32-S3 N16R8

Pantalla y joystick se alimentan exclusivamente a **3.3 V** y comparten GND.

| ST7735S | ESP32-S3 |
|---|---:|
| VCC / GND | 3V3 / GND |
| SCL / SDA | GPIO 12 / GPIO 11 |
| CS / DC / RST / BL | GPIO 10 / 9 / 8 / 7 |

| Joystick | ESP32-S3 |
|---|---:|
| VCC / GND | 3V3 (no 5 V) / GND |
| VRx / VRy / SW | GPIO 1 / 2 / 4 |

## Controles

- VRx gira, VRy avanza/retrocede y SW dispara o reinicia.
- Elimina los siete enemigos y entra en la salida verde.
- Deja el joystick centrado durante la calibracion inicial.
- Si un eje esta invertido, cambia `INVERT_X` o `INVERT_Y` en `src/main.cpp`.

## Uso

```bash
pio run
pio run -t upload
pio device monitor -b 115200
```

## Si no aparece el puerto USB

1. Usa un cable USB que transmita datos, no uno que sea solamente de carga.
2. Mantén pulsado `BOOT`, pulsa y suelta `RESET` y luego suelta `BOOT`.
3. Comprueba que aparezca `/dev/ttyACM0` con `pio device list`.
4. Ejecuta `pio run -t upload` y no desconectes la placa durante la carga.

Si la placa usa un conversor USB-UART y aparece como `/dev/ttyUSB0`, cambia
`upload_port` y `monitor_port` en `platformio.ini` por `/dev/ttyUSB0`.
