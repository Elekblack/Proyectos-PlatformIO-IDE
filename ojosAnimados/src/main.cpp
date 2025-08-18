#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <playCodeBot_Oled.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
OjosRobot ojos;

void setup() {
    Wire.begin();
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("No se pudo inicializar la pantalla OLED"));
        while (1);
    }
    ojos.inicializar(SCREEN_WIDTH, SCREEN_HEIGHT, 50, display);
    ojos.establecerParpadeoAutomatico(true, 2, 3);
    ojos.abrir();
}

void loop() {
    ojos.actualizar();
}