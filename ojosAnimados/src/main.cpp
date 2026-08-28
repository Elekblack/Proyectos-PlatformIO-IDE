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
    Serial.begin(115200);
    Wire.begin();
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("No se pudo inicializar la pantalla OLED"));
        while (1);
    }
    ojos.inicializar(SCREEN_WIDTH, SCREEN_HEIGHT, 50, display);
    ojos.abrir();
    delay(2000); // Espera 2 segundos para ver el cambio
}

void loop() {
    ojos.actualizar();
    ojos.establecerParpadeoAutomatico(false, 2, 3);
    ojos.ojoD_abierto = true; // Abre el ojo derecho
    ojos.ojoI_abierto = true; // Abre el ojo izquierdo  
    ojos.dibujarOjos();
    ojos.establecerPosicion(ESTE); // Mira a la derecha 
    ojos.dibujarOjos();
    delay(2000); // Espera un segundo para ver el cambio
    ojos.establecerPosicion(NORTE_ESTE);
    ojos.dibujarOjos();
    delay(2000); // Espera un segundo para ver el cambio
    ojos.establecerPosicion(OESTE); // Mira a la izquierda
    ojos.dibujarOjos();
    delay(2000); // Espera un segundo para ver el cambio
    ojos.establecerPosicion(PREDETERMINADO); // Vuelve al centro
    ojos.establecerParpadeoAutomatico(true, 2, 3);
    ojos.dibujarOjos();
    delay(6000); // Espera un segundo para ver el cambio
}
