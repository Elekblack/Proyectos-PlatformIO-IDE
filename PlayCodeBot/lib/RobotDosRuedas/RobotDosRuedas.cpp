// RobotDosRuedas.cpp
#include "RobotDosRuedas.h"

// Implementación de ControlPID
ControlPID::ControlPID(float p, float i, float d) : kp(p), ki(i), kd(d), integral(0.0), errorAnterior(0.0), tiempoAnterior(0) {}

float ControlPID::calcular(float entrada, float setpoint) {
    unsigned long ahora = millis();
    float dt = (ahora - tiempoAnterior) / 1000.0;  // dt en segundos
    if (dt == 0) return 0.0;  // Evitar división por cero

    float error = setpoint - entrada;
    integral += error * dt;
    float derivada = (error - errorAnterior) / dt;

    float salida = kp * error + ki * integral + kd * derivada;

    errorAnterior = error;
    tiempoAnterior = ahora;

    return salida;
}

void ControlPID::reiniciar() {
    integral = 0.0;
    errorAnterior = 0.0;
    tiempoAnterior = millis();
}

// Implementación de RobotDosRuedas
RobotDosRuedas::RobotDosRuedas(int izqAd, int izqAt, int derAd, int derAt)
    : pinIzqAdelante(izqAd), pinIzqAtras(izqAt), pinDerAdelante(derAd), pinDerAtras(derAt),
      pidGiro(2.0, 0.01, 0.5),  // Valores de tuning para giro (ajustar según pruebas)
      pidRecto(1.5, 0.005, 0.2),  // Valores de tuning para recto (ajustar según pruebas)
      modoActual(DETENIDO), velocidadActual(0), yaw(0.0), yawObjetivo(0.0), yawInicial(0.0), offsetGyroZ(0.0) {}

void RobotDosRuedas::inicializar() {
    // Configura pines como salida
    pinMode(pinIzqAdelante, OUTPUT);
    pinMode(pinIzqAtras, OUTPUT);
    pinMode(pinDerAdelante, OUTPUT);
    pinMode(pinDerAtras, OUTPUT);

    // Inicia I2C
    Wire.begin();

    // Inicia MPU6050
    mpu.initialize();
    if (!mpu.testConnection()) {
        // Error si no conecta, pero por ahora solo continua
    }

    // Calibra giroscopio
    calibrarGiroscopio();

    tiempoAnterior = millis();
}

void RobotDosRuedas::calibrarGiroscopio() {
    // Calibra offset de gyro Z tomando promedio de 100 lecturas
    float suma = 0.0;
    for (int i = 0; i < 100; i++) {
        int16_t gx, gy, gz;
        mpu.getRotation(&gx, &gy, &gz);
        suma += gz;
        delay(10);
    }
    offsetGyroZ = suma / 100.0 / 131.0;  // Sensibilidad para ±250 dps: 131 LSB/dps
}

void RobotDosRuedas::actualizarYaw() {
    unsigned long ahora = millis();
    float dt = (ahora - tiempoAnterior) / 1000.0;  // dt en segundos
    tiempoAnterior = ahora;

    int16_t gx, gy, gz;
    mpu.getRotation(&gx, &gy, &gz);
    float gyroZ = (gz - offsetGyroZ * 131.0) / 131.0;  // dps, corrigiendo offset

    yaw += gyroZ * dt;  // Integra para obtener grados
}

void RobotDosRuedas::establecerVelocidades(int velIzq, int velDer) {
    // velIzq y velDer: -255 a 255
    if (velIzq >= 0) {
        analogWrite(pinIzqAtras, 0);
        analogWrite(pinIzqAdelante, velIzq);
    } else {
        analogWrite(pinIzqAdelante, 0);
        analogWrite(pinIzqAtras, -velIzq);
    }

    if (velDer >= 0) {
        analogWrite(pinDerAtras, 0);
        analogWrite(pinDerAdelante, velDer);
    } else {
        analogWrite(pinDerAdelante, 0);
        analogWrite(pinDerAtras, -velDer);
    }
}

void RobotDosRuedas::avanzar(int velocidad) {
    modoActual = AVANZAR_SIMPLE;
    velocidadActual = constrain(velocidad, 0, 255);
    establecerVelocidades(velocidadActual, velocidadActual);
}

void RobotDosRuedas::retroceder(int velocidad) {
    modoActual = RETROCEDER_SIMPLE;
    velocidadActual = constrain(velocidad, 0, 255);
    establecerVelocidades(-velocidadActual, -velocidadActual);
}

void RobotDosRuedas::girarDerecha(int velocidad) {
    modoActual = GIRAR_DER_SIMPLE;
    velocidadActual = constrain(velocidad, 0, 255);
    establecerVelocidades(velocidadActual, -velocidadActual);
}

void RobotDosRuedas::girarIzquierda(int velocidad) {
    modoActual = GIRAR_IZQ_SIMPLE;
    velocidadActual = constrain(velocidad, 0, 255);
    establecerVelocidades(-velocidadActual, velocidadActual);
}

void RobotDosRuedas::detener() {
    modoActual = DETENIDO;
    establecerVelocidades(0, 0);
}

void RobotDosRuedas::girarAngulo(float angulo) {
    // Función bloqueante: gira hasta alcanzar el ángulo
    actualizarYaw();  // Actualiza yaw actual
    yawObjetivo = yaw + angulo;
    pidGiro.reiniciar();

    float error = yawObjetivo - yaw;
    while (abs(error) > 1.0) {  // Umbral de 1 grado
        actualizarYaw();
        error = yawObjetivo - yaw;
        float salida = pidGiro.calcular(yaw, yawObjetivo);

        // Salida PID controla la velocidad de giro (positiva para derecha)
        int velGiro = constrain(abs(salida), 0, 255);
        if (angulo > 0) {  // Giro derecha
            establecerVelocidades(velGiro, -velGiro);
        } else {  // Giro izquierda
            establecerVelocidades(-velGiro, velGiro);
        }

        delay(10);  // Frecuencia de control ~100Hz
    }
    detener();  // Detiene al finalizar
}

void RobotDosRuedas::iniciarAvanzarRecto(int velocidad) {
    modoActual = AVANZAR_RECTO;
    velocidadActual = constrain(velocidad, 0, 255);
    actualizarYaw();  // Actualiza yaw actual
    yawInicial = yaw;
    pidRecto.reiniciar();
}

void RobotDosRuedas::actualizar() {
    actualizarYaw();  // Siempre actualiza yaw

    switch (modoActual) {
        case AVANZAR_RECTO: {
            float error = yawInicial - yaw;
            float salida = pidRecto.calcular(yaw, yawInicial);

            // Salida PID es corrección diferencial
            int velIzq = constrain(velocidadActual + salida, -255, 255);
            int velDer = constrain(velocidadActual - salida, -255, 255);
            establecerVelocidades(velIzq, velDer);
            break;
        }
        // Otros modos simples ya setean velocidades directamente, no necesitan update
        default:
            break;
    }
}