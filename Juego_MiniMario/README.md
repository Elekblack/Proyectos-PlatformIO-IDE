# Mini Mario - ESP32-S3 N16R8 + ST7735S + Joystick

Juego tipo *endless runner*: Mario corre automáticamente, salta tuberías,
se agacha ante enemigos voladores y recoge monedas.

## Conexiones

| ST7735S | ESP32-S3 | Joystick | ESP32-S3 |
|---|---:|---|---:|
| VCC | 3V3 | VCC | 3V3 (no 5 V) |
| GND | GND | GND | GND |
| SCL/SCK | GPIO 12 | VRx | GPIO 1 |
| SDA/MOSI | GPIO 11 | VRy | GPIO 2 |
| CS | GPIO 10 | SW | GPIO 4 |
| DC | GPIO 9 | | |
| RST | GPIO 8 | | |
| BL | GPIO 7 | | |

## Compilar y subir (PlatformIO)

```bash
cd Juego_MiniMario
pio run -t upload
pio device monitor
```

## Si la pantalla se ve mal (colores/desplazada)

En `src/main.cpp`, línea `#define INIT_TAB INITR_BLACKTAB`, prueba con:
- `INITR_GREENTAB`
- `INITR_REDTAB`
- `INITR_144GREENTAB`

según la variante exacta de tu módulo ST7735S. También puedes ajustar
`tft.setRotation(1)` (valores 0-3) si tu panel queda al revés.

## Si el joystick reacciona al revés

El centro se calibra automáticamente al encender. Deja el joystick suelto
durante ese instante. Si un eje responde al revés, cambia `INVERT_X_AXIS` o
`INVERT_Y_AXIS` a `true` en `src/main.cpp`.

## Cómo se juega

- El personaje corre automáticamente.
- Botón del joystick (SW): saltar tuberías, iniciar juego, reintentar.
- Joystick hacia abajo: agacharse para esquivar los pájaros voladores.
- Joystick a la izquierda/derecha: frenar o correr más rápido.
- Recoge monedas amarillas para sumar puntos extra.
- El puntaje máximo se guarda en la memoria flash (NVS) del ESP32.

Al encender, deja el joystick en reposo durante una fracción de segundo para
que el programa calibre automáticamente el centro de ambos ejes. Si algún eje
queda invertido, cambia `INVERT_X_AXIS` o `INVERT_Y_AXIS` a `true` en
`src/main.cpp`.

## Ideas para extender (buen ejercicio para tus estudiantes)

- Agregar sprites en bitmap (`drawRGBBitmap`) en vez de formas geométricas.
- Añadir un menú de dificultad controlado con el joystick.
- Añadir efectos de sonido con un buzzer pasivo y `ledc`.
- Guardar tabla de mejores puntajes (top 5) en vez de solo el máximo.
