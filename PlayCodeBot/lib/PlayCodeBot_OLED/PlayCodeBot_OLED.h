/**
 * PlayCodeBot_OLED.h
 * Librería para manejar una pantalla OLED 0.96" 128x64 SSD1306 por I2C.
 * Permite mostrar texto centrado y dibujar ojos en pantalla.
 * 
 * Autor: ElekBlack
 * Basado en la librería Adafruit SSD1306 y Adafruit GFX.
 * Esta librería es parte del proyecto PlayCodeBot.
 * Fecha: 2025
 */

#ifndef PlayCodeBot_OLED_H
#define PlayCodeBot_OLED_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Configuración del display
#define ANCHO_PANTALLA   128
#define ALTO_PANTALLA    64
#define DIRECCION_I2C    0x3C  // Dirección típica del SSD1306
#define PIN_RESET       -1     // No se usa pin de reset físico

class PlayCodeBot_OLED {
  private:
    Adafruit_SSD1306 pantalla; // Objeto de la pantalla OLED

    // Posición central de los ojos
    int centroOjoX[2];  // [0] ojo izquierdo, [1] ojo derecho
    int centroOjoY;

    // Dimensiones de los ojos
    int anchoOjo;
    int altoOjo;
    int tamanoPupila;

    /**
     * Dibuja un texto centrado horizontalmente en una coordenada Y específica.
     * @param texto Texto a mostrar.
     * @param y Coordenada vertical donde se dibuja.
     */
    void dibujarTextoCentrado(const String& texto, int y);

  public:
    /**
     * Constructor: inicializa las dimensiones y posiciones por defecto.
     */
    PlayCodeBot_OLED();

    /**
     * Inicializa la pantalla OLED.
     * @return true si la inicialización fue exitosa, false en caso contrario.
     */
    bool comenzar();

    /**
     * Limpia el contenido actual de la pantalla.
     */
    void limpiarPantalla();

    /**
     * Muestra tres líneas de texto centradas horizontalmente.
     * @param linea1 Primera línea (puede estar vacía).
     * @param linea2 Segunda línea (puede estar vacía).
     * @param linea3 Tercera línea (puede estar vacía).
     */
    void mostrarTexto(const String& linea1, const String& linea2, const String& linea3);

    /**
     * Dibuja dos ojos estilizados (como los de Cozmo) con pupilas.
     * @param desplazamientoX Movimiento horizontal de las pupilas (positivo → derecha).
     * @param desplazamientoY Movimiento vertical de las pupilas (positivo → abajo).
     */
    void dibujarOjos(int desplazamientoX = 0, int desplazamientoY = 0);

    /**
     * Simula un parpadeo: cierra y abre los ojos brevemente.
     */
    void parpadear();

    /**
     * Actualiza la pantalla para mostrar los cambios realizados.
     */
    void mostrar();
};

#endif