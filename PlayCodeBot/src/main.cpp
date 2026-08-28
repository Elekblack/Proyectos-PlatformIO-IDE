// ------
// Proyecto: Robot "PlayCodeBot" para educación y entretenimiento
// Autor: ElekBlack
// Licencia: MIT
// ------ 

// main.cpp (Programa de ejemplo para probar la librería con control remoto por Bluetooth)
// Asegúrate de agregar la librería RobotDosRuedas en PlatformIO (copiar archivos en lib/) y lib_deps = bodmer/BluetoothSerial (para ESP32 Bluetooth)

#include <Arduino.h>
#include <BluetoothSerial.h>  // Biblioteca para Bluetooth Serial en ESP32
#include "ComandosRobot.h"
#include "RobotDosRuedas.h"

// Define los pines (ajusta según tu conexión)
// Para ESP32 Devkit V1 30 pines, elige pines PWM como 25, 26, 27, 14 por ejemplo
RobotDosRuedas robot(26, 27, 32, 33);  // izqAd, izqAt, derAd, derAt

BluetoothSerial SerialBT;  // Objeto para Bluetooth

void setup() {
    Serial.begin(115200);  // Para depuración
    SerialBT.begin("RobotBT");  // Nombre del dispositivo Bluetooth

    const bool mpuDisponible = robot.inicializar();  // Inicializa el robot y MPU6050

    Serial.println("Robot listo. Conecta via Bluetooth y envia comandos:");
    Serial.println("F: Avanzar, B: Retroceder, L: Girar Izq, R: Girar Der, S: Detener");
    Serial.println("1: Avanzar recto, 2: Girar 90 grados Der, 3: Girar 90 grados Izq");
    if (!mpuDisponible) {
        Serial.println("ADVERTENCIA: MPU6050 no disponible; movimientos asistidos deshabilitados");
    }
}

void loop() {
    if (SerialBT.available()) {
        const char recibido = SerialBT.read();
        switch (decodificarComando(recibido)) {
            case ComandoRobot::AVANZAR:
                robot.avanzar(200);  // Velocidad ejemplo
                SerialBT.println("Avanzando");
                break;
            case ComandoRobot::RETROCEDER:
                robot.retroceder(200);
                SerialBT.println("Retrocediendo");
                break;
            case ComandoRobot::GIRAR_IZQUIERDA:
                robot.girarIzquierda(150);
                SerialBT.println("Girando Izquierda");
                break;
            case ComandoRobot::GIRAR_DERECHA:
                robot.girarDerecha(150);
                SerialBT.println("Girando Derecha");
                break;
            case ComandoRobot::DETENER:
                robot.detener();
                SerialBT.println("Detenido");
                break;
            case ComandoRobot::AVANZAR_RECTO:
                if (robot.iniciarAvanzarRecto(200)) SerialBT.println("Avanzando Recto");
                else SerialBT.println("Error: MPU6050 no disponible");
                break;
            case ComandoRobot::GIRAR_90_DERECHA:
                SerialBT.println(robot.girarAngulo(90.0)
                                     ? "Giro 90 grados Derecha completado"
                                     : "Error: giro cancelado por seguridad");
                break;
            case ComandoRobot::GIRAR_90_IZQUIERDA:
                SerialBT.println(robot.girarAngulo(-90.0)
                                     ? "Giro 90 grados Izquierda completado"
                                     : "Error: giro cancelado por seguridad");
                break;
            case ComandoRobot::NINGUNO:
                if (!esSeparadorComando(recibido)) SerialBT.println("Comando desconocido");
                break;
        }
    }

    // Llama a actualizar para modos avanzados como AVANZAR_RECTO
    robot.actualizar();

    delay(20);  // Pequeño delay para estabilidad
}
