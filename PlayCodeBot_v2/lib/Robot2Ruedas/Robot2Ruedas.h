#ifndef ROBOT2RUEDAS_H
#define ROBOT2RUEDAS_H

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_light.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Robot2Ruedas {
  private:
    // Pines del driver MX1508
    int pinMotorIzqA, pinMotorIzqB;
    int pinMotorDerA, pinMotorDerB;

    // Objetos de hardware
    MPU6050* mpu;
    Adafruit_SSD1306* oled;

  public:
    // Constructor
    Robot2Ruedas(int mIzqA, int mIzqB, int mDerA, int mDerB, MPU6050* mpuPtr, Adafruit_SSD1306* oledPtr);

    // Funciones de movimiento
    void avanzar(int velocidad);
    void retroceder(int velocidad);
    void girar_derecha(int velocidad);
    void girar_izquierda(int velocidad);
    void detener();

    // Función para girar a un ángulo específico
    void girar_a_angulo(float anguloObjetivo, int velocidad);

    // Mostrar mensajes en OLED
    void mostrarMensaje(String linea1, String linea2 = "");
};

#endif
