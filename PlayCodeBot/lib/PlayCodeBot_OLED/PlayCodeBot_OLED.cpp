/**
 * OLED_CozmoDisplay.cpp
 * Implementación de la librería para pantalla OLED con ojos estilo Cozmo.
 */

#include "PlayCodeBot_OLED.h"

// Constructor: inicializa valores predeterminados
PlayCodeBot_OLED::PlayCodeBot_OLED()
    : pantalla(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, PIN_RESET) {
  centroOjoX[0] = 40;  // Ojo izquierdo
  centroOjoX[1] = 88;  // Ojo derecho
  centroOjoY = 32;     // Altura común de los ojos

  anchoOjo = 30;
  altoOjo = 20;
  tamanoPupila = 8;
}

bool PlayCodeBot_OLED::comenzar() {
  // Inicializar pantalla por I2C
  if (!pantalla.begin(SSD1306_SWITCHCAPVCC, DIRECCION_I2C)) {
    return false; // Falló la inicialización
  }

  pantalla.clearDisplay();           // Limpiar pantalla
  pantalla.setTextSize(1);           // Tamaño de texto normal
  pantalla.setTextColor(SSD1306_WHITE); // Color blanco (único disponible)
  pantalla.cp437(true);              // Habilitar caracteres ASCII extendidos

  return true;
}

void PlayCodeBot_OLED::limpiarPantalla() {
  pantalla.clearDisplay();
}

void PlayCodeBot_OLED::dibujarTextoCentrado(const String& texto, int y) {
  if (texto.length() == 0) return; // No dibujar si está vacío

  // Calcular el ancho del texto para centrarlo
  int16_t x1, y1;
  uint16_t anchoTexto, altoTexto;
  pantalla.getTextBounds(texto, 0, 0, &x1, &y1, &anchoTexto, &altoTexto);

  // Calcular posición X para centrar
  int x = (ANCHO_PANTALLA - anchoTexto) / 2;

  // Establecer cursor y dibujar
  pantalla.setCursor(x, y);
  pantalla.print(texto);
}

void PlayCodeBot_OLED::mostrarTexto(const String& linea1, const String& linea2, const String& linea3) {
  limpiarPantalla();

  const int alturaLinea = 16; // Altura estimada de una línea de texto
  const int yInicial = 10;

  pantalla.setTextSize(1);
  dibujarTextoCentrado(linea1, yInicial);
  dibujarTextoCentrado(linea2, yInicial + alturaLinea);
  dibujarTextoCentrado(linea3, yInicial + 2 * alturaLinea);

  mostrar(); // Actualizar pantalla
}

void PlayCodeBot_OLED::dibujarOjos(int desplazamientoX, int desplazamientoY) {
  limpiarPantalla();

  // Dibujar contorno de los ojos (rectángulos redondeados)
  for (int i = 0; i < 2; i++) {
    pantalla.drawRoundRect(
      centroOjoX[i] - anchoOjo / 2,
      centroOjoY - altoOjo / 2,
      anchoOjo,
      altoOjo,
      10,  // Radio de redondeo
      SSD1306_WHITE
    );
  }

  // Dibujar pupilas (círculos dentro de los ojos)
  for (int i = 0; i < 2; i++) {
    int xPupila = centroOjoX[i] + desplazamientoX;
    int yPupila = centroOjoY + desplazamientoY;

    // Limitar movimiento de la pupila dentro del ojo
    int minX = centroOjoX[i] - anchoOjo / 2 + tamanoPupila / 2 + 2;
    int maxX = centroOjoX[i] + anchoOjo / 2 - tamanoPupila / 2 - 2;
    int minY = centroOjoY - altoOjo / 2 + tamanoPupila / 2 + 2;
    int maxY = centroOjoY + altoOjo / 2 - tamanoPupila / 2 - 2;

    xPupila = constrain(xPupila, minX, maxX);
    yPupila = constrain(yPupila, minY, maxY);

    pantalla.fillCircle(xPupila, yPupila, tamanoPupila / 2, SSD1306_WHITE);
  }

  mostrar(); // Mostrar en pantalla
}

void PlayCodeBot_OLED::parpadear() {
  // Mostrar ojos abiertos
  dibujarOjos();

  delay(150);

  // Cerrar ojos (dibujar línea horizontal en el centro del ojo)
  limpiarPantalla();
  for (int i = 0; i < 2; i++) {
    pantalla.fillRect(
      centroOjoX[i] - anchoOjo / 2,
      centroOjoY - 1,
      anchoOjo,
      3,  // Alto del párpado
      SSD1306_WHITE
    );
  }
  mostrar();

  delay(100); // Duración del cierre

  // Reabrir ojos
  dibujarOjos();
}

void PlayCodeBot_OLED::mostrar() {
  pantalla.display(); // Envía los cambios al display
}