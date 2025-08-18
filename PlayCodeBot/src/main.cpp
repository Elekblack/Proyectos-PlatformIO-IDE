// ------
// Proyecto: Robot "PlayCodeBot" para educación y entretenimiento
// Autor: ElekBlack
// Licencia: MIT
// ------ 

#include <Arduino.h>
#include "PlayCodeBot_OLED.h"

PlayCodeBot_OLED oled; // Instancia de la clase OLED

void setup() {
  
  Serial.begin(9600);
  if (!oled.comenzar()) {
    Serial.println("Error al iniciar la pantalla OLED");
    while (true); // Detener si falla la inicialización
  } 
  oled.limpiarPantalla(); // Limpiar pantalla al inicio
  oled.mostrarTexto("Hola", "PlayCodeBot", "¡Listo para jugar!"); 
  delay(5000); // Esperar 5 segundos para mostrar el mensaje    

}

void loop() {
   // Ejemplo de uso de la función dibujarOjos
  oled.dibujarOjos(0, 0); // Dibujar ojos en posición central
  delay(2000); // Esperar 2 segundo para mostrar los ojos 
  // Ejemplo de parpadeo
  oled.parpadear(); // Simular parpadeo 
  delay(1000); // Esperar 1 segundo después del parpadeo
}
