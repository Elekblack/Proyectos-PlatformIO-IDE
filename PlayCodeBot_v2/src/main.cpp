#include <Arduino.h>
#include <Wire.h>
#include <BluetoothSerial.h>
#include <MPU6050_light.h>
#include <Adafruit_SSD1306.h>
#include "ComandosRobot.h"
#include "Robot2Ruedas.h"

// ---- Configuración de hardware ----
#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA 64
#define OLED_RESET -1

// Pines del puente H MX1508
#define MOTOR_IZQ_A 26
#define MOTOR_IZQ_B 27
#define MOTOR_DER_A 32
#define MOTOR_DER_B 33

// Objetos globales
Adafruit_SSD1306 oled(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, OLED_RESET);
MPU6050 mpu(Wire);
BluetoothSerial BT;
Robot2Ruedas* robot;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // ---- Inicializar OLED ----
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Error: no se encontró pantalla OLED");
    for (;;);
  }
  oled.clearDisplay();
  oled.display();

  // ---- Inicializar MPU6050 ----
  byte status = mpu.begin();
  if (status != 0) {
    Serial.println("Error al iniciar MPU6050");
    while (1);
  }
  mpu.calcOffsets();

  // ---- Inicializar Bluetooth ----
  BT.begin("PlayCodeBot");

  // ---- Inicializar Robot ----
  robot = new Robot2Ruedas(MOTOR_IZQ_A, MOTOR_IZQ_B, MOTOR_DER_A, MOTOR_DER_B, &mpu, &oled);
  robot->mostrarMensaje("Robot listo", "Esperando BT");
}

void loop() {
  mpu.update();

  if (BT.available()) {
    const char recibido = BT.read();
    switch (decodificarComando(recibido)) {
      case ComandoRobot::AVANZAR: robot->avanzar(200); break;
      case ComandoRobot::RETROCEDER: robot->retroceder(200); break;
      case ComandoRobot::GIRAR_IZQUIERDA: robot->girar_izquierda(200); break;
      case ComandoRobot::GIRAR_DERECHA: robot->girar_derecha(200); break;
      case ComandoRobot::DETENER: robot->detener(); break;
      case ComandoRobot::GIRAR_90_DERECHA:
        BT.println(robot->girar_a_angulo(90, 180) ? "Giro completado" : "Giro cancelado por seguridad");
        break;
      case ComandoRobot::GIRAR_90_IZQUIERDA:
        BT.println(robot->girar_a_angulo(-90, 180) ? "Giro completado" : "Giro cancelado por seguridad");
        break;
      case ComandoRobot::NINGUNO:
        if (!esSeparadorComando(recibido)) BT.println("Comando desconocido");
        break;
    }
  }
}
