#include "Robot2Ruedas.h"

// Constructor
Robot2Ruedas::Robot2Ruedas(int mIzqA, int mIzqB, int mDerA, int mDerB, MPU6050* mpuPtr, Adafruit_SSD1306* oledPtr) {
  pinMotorIzqA = mIzqA;
  pinMotorIzqB = mIzqB;
  pinMotorDerA = mDerA;
  pinMotorDerB = mDerB;

  mpu = mpuPtr;
  oled = oledPtr;

  pinMode(pinMotorIzqA, OUTPUT);
  pinMode(pinMotorIzqB, OUTPUT);
  pinMode(pinMotorDerA, OUTPUT);
  pinMode(pinMotorDerB, OUTPUT);
}

// ---- Funciones de movimiento ----
void Robot2Ruedas::avanzar(int velocidad) {
  analogWrite(pinMotorIzqA, velocidad);
  analogWrite(pinMotorIzqB, 0);
  analogWrite(pinMotorDerA, velocidad);
  analogWrite(pinMotorDerB, 0);
  mostrarMensaje("Avanzando", "Vel: " + String(velocidad));
}

void Robot2Ruedas::retroceder(int velocidad) {
  analogWrite(pinMotorIzqA, 0);
  analogWrite(pinMotorIzqB, velocidad);
  analogWrite(pinMotorDerA, 0);
  analogWrite(pinMotorDerB, velocidad);
  mostrarMensaje("Retrocediendo", "Vel: " + String(velocidad));
}

void Robot2Ruedas::girar_derecha(int velocidad) {
  analogWrite(pinMotorIzqA, velocidad);
  analogWrite(pinMotorIzqB, 0);
  analogWrite(pinMotorDerA, 0);
  analogWrite(pinMotorDerB, velocidad);
  mostrarMensaje("Girando", "Derecha");
}

void Robot2Ruedas::girar_izquierda(int velocidad) {
  analogWrite(pinMotorIzqA, 0);
  analogWrite(pinMotorIzqB, velocidad);
  analogWrite(pinMotorDerA, velocidad);
  analogWrite(pinMotorDerB, 0);
  mostrarMensaje("Girando", "Izquierda");
}

void Robot2Ruedas::detener() {
  analogWrite(pinMotorIzqA, 0);
  analogWrite(pinMotorIzqB, 0);
  analogWrite(pinMotorDerA, 0);
  analogWrite(pinMotorDerB, 0);
  mostrarMensaje("Robot", "Detenido");
}

// ---- Girar a un ángulo con MPU6050_light ----
void Robot2Ruedas::girar_a_angulo(float anguloObjetivo, int velocidad) {
  mpu->update();
  float anguloInicial = mpu->getAngleZ();
  float anguloActual = anguloInicial;

  if (anguloObjetivo > 0) {
    // Giro a la derecha
    girar_derecha(velocidad);
    while ((anguloActual - anguloInicial) < anguloObjetivo) {
      mpu->update();
      anguloActual = mpu->getAngleZ();
    }
  } else {
    // Giro a la izquierda
    girar_izquierda(velocidad);
    while ((anguloActual - anguloInicial) > anguloObjetivo) {
      mpu->update();
      anguloActual = mpu->getAngleZ();
    }
  }

  detener();
  mostrarMensaje("Giro completo", "Angulo: " + String(anguloObjetivo));
}

// ---- Mostrar mensajes en OLED ----
void Robot2Ruedas::mostrarMensaje(String linea1, String linea2) {
  oled->clearDisplay();
  oled->setTextSize(1);
  oled->setTextColor(SSD1306_WHITE);
  oled->setCursor(0, 10);
  oled->println(linea1);
  oled->setCursor(0, 30);
  oled->println(linea2);
  oled->display();
}
