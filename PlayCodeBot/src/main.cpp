// ------
// Proyecto: Robot "PlayCodeBot" para educación y entretenimiento
// Autor: ElekBlack
// Licencia: MIT
// ------ 

// main.cpp (Programa de ejemplo para probar la librería con control remoto por Bluetooth)
// Asegúrate de agregar la librería RobotDosRuedas en PlatformIO (copiar archivos en lib/) y lib_deps = bodmer/BluetoothSerial (para ESP32 Bluetooth)

#include <Arduino.h>
#include <BluetoothSerial.h>  // Biblioteca para Bluetooth Serial en ESP32
#include "RobotDosRuedas.h"

// Define los pines (ajusta según tu conexión)
// Para ESP32 Devkit V1 30 pines, elige pines PWM como 25, 26, 27, 14 por ejemplo
RobotDosRuedas robot(25, 26, 27, 14);  // izqAd, izqAt, derAd, derAt

BluetoothSerial SerialBT;  // Objeto para Bluetooth

void setup() {
    Serial.begin(115200);  // Para depuración
    SerialBT.begin("RobotBT");  // Nombre del dispositivo Bluetooth

    robot.inicializar();  // Inicializa el robot y MPU6050

    Serial.println("Robot listo. Conecta via Bluetooth y envia comandos:");
    Serial.println("W: Avanzar, S: Retroceder, A: Girar Izq, D: Girar Der, X: Detener");
    Serial.println("F: Avanzar Recto, 9: Girar 90° Der, 0: Girar 90° Izq");
}

void loop() {
    if (SerialBT.available()) {
        char comando = SerialBT.read();
        switch (comando) {
            case 'W':
                robot.avanzar(200);  // Velocidad ejemplo
                SerialBT.println("Avanzando");
                break;
            case 'S':
                robot.retroceder(200);
                SerialBT.println("Retrocediendo");
                break;
            case 'A':
                robot.girarIzquierda(150);
                SerialBT.println("Girando Izquierda");
                break;
            case 'D':
                robot.girarDerecha(150);
                SerialBT.println("Girando Derecha");
                break;
            case 'X':
                robot.detener();
                SerialBT.println("Detenido");
                break;
            case 'F':
                robot.iniciarAvanzarRecto(200);
                SerialBT.println("Avanzando Recto");
                break;
            case '9':
                robot.girarAngulo(90.0);
                SerialBT.println("Girando 90° Derecha");
                break;
            case '0':
                robot.girarAngulo(-90.0);
                SerialBT.println("Girando 90° Izquierda");
                break;
            default:
                SerialBT.println("Comando desconocido");
                break;
        }
    }

    // Llama a actualizar para modos avanzados como AVANZAR_RECTO
    robot.actualizar();

    delay(20);  // Pequeño delay para estabilidad
}
