// RobotDosRuedas.h
#ifndef ROBOT_DOS_RUEDAS_H
#define ROBOT_DOS_RUEDAS_H

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>  // Requiere la biblioteca MPU6050 de jrowberg/i2cdevlib. Agregar en PlatformIO: lib_deps = jrowberg/MPU6050

class ControlPID {
private:
    float kp;  // Ganancia proporcional
    float ki;  // Ganancia integral
    float kd;  // Ganancia derivativa
    float integral;  // Acumulador integral
    float errorAnterior;  // Error previo para derivativa
    unsigned long tiempoAnterior;  // Tiempo previo para dt

public:
    /**
     * Constructor para el controlador PID.
     * @param p Valor de kp.
     * @param i Valor de ki.
     * @param d Valor de kd.
     */
    ControlPID(float p, float i, float d);

    /**
     * Calcula la salida del PID basada en el setpoint y la entrada actual.
     * @param entrada Valor medido actual.
     * @param setpoint Valor deseado.
     * @return Salida del PID.
     */
    float calcular(float entrada, float setpoint);

    /**
     * Reinicia los valores internos del PID (integral y error anterior).
     */
    void reiniciar();
};

class RobotDosRuedas {
private:
    // Pines para los motores (MX1508: dos pines por motor, PWM para velocidad y dirección)
    int pinIzqAdelante;  // Pin para motor izquierdo adelante
    int pinIzqAtras;     // Pin para motor izquierdo atrás
    int pinDerAdelante;  // Pin para motor derecho adelante
    int pinDerAtras;     // Pin para motor derecho atrás

    MPU6050 mpu;  // Objeto para el sensor MPU6050

    // Variables para el giroscopio y orientación
    float yaw;  // Ángulo actual en yaw (grados)
    float yawObjetivo;  // Ángulo objetivo para giros precisos
    float yawInicial;  // Yaw inicial para mantener rumbo recto
    unsigned long tiempoAnterior;  // Para calcular dt en integración
    float offsetGyroZ;  // Offset calibrado para gyro Z
    bool mpuDisponible;

    // Controladores PID
    ControlPID pidGiro;  // PID para giros angulares
    ControlPID pidRecto; // PID para mantener línea recta

    // Modo de operación actual
    enum Modo { DETENIDO, AVANZAR_SIMPLE, RETROCEDER_SIMPLE, GIRAR_DER_SIMPLE, GIRAR_IZQ_SIMPLE, GIRAR_ANGULO, AVANZAR_RECTO };
    Modo modoActual;

    int velocidadActual;  // Velocidad base actual (0-255)

    /**
     * Establece la velocidad de los motores.
     * @param velIzq Velocidad para motor izquierdo (-255 a 255, negativo para atrás).
     * @param velDer Velocidad para motor derecho (-255 a 255, negativo para atrás).
     */
    void establecerVelocidades(int velIzq, int velDer);

    /**
     * Lee y actualiza el yaw basado en el giroscopio.
     */
    void actualizarYaw();

    /**
     * Calibra el offset del giroscopio Z.
     */
    void calibrarGiroscopio();

public:
    /**
     * Constructor de la clase.
     * @param izqAd Pin para motor izquierdo adelante.
     * @param izqAt Pin para motor izquierdo atrás.
     * @param derAd Pin para motor derecho adelante.
     * @param derAt Pin para motor derecho atrás.
     */
    RobotDosRuedas(int izqAd, int izqAt, int derAd, int derAt);

    /**
     * Inicializa el robot: configura pines, inicia Wire y MPU6050, calibra.
     * @return true si el MPU6050 está disponible.
     */
    bool inicializar();

    /**
     * Movimiento básico: avanzar a la velocidad indicada.
     * @param velocidad Velocidad (0-255).
     */
    void avanzar(int velocidad);

    /**
     * Movimiento básico: retroceder a la velocidad indicada.
     * @param velocidad Velocidad (0-255).
     */
    void retroceder(int velocidad);

    /**
     * Movimiento básico: girar a la derecha (en el lugar) a la velocidad indicada.
     * @param velocidad Velocidad (0-255).
     */
    void girarDerecha(int velocidad);

    /**
     * Movimiento básico: girar a la izquierda (en el lugar) a la velocidad indicada.
     * @param velocidad Velocidad (0-255).
     */
    void girarIzquierda(int velocidad);

    /**
     * Detiene el robot inmediatamente.
     */
    void detener();

    /**
     * Gira un ángulo preciso usando PID y MPU6050. Esta función es bloqueante.
     * @param angulo Ángulo en grados (positivo para derecha, negativo para izquierda).
     * @param tiempoMaximoMs Tiempo límite antes de detener los motores.
     * @return true si alcanzó el ángulo; false si no hay sensor o venció el tiempo.
     */
    bool girarAngulo(float angulo, unsigned long tiempoMaximoMs = 6000);

    /**
     * Inicia el avance en línea recta manteniendo el rumbo con PID y MPU6050.
     * Debe llamarse actualizar() en el loop principal para mantener el control.
     * @param velocidad Velocidad base (0-255).
     * @return true si el MPU6050 está disponible y el modo pudo iniciarse.
     */
    bool iniciarAvanzarRecto(int velocidad);

    /**
     * Función de actualización que debe llamarse en el loop principal para modos avanzados.
     * Actualiza el yaw y ajusta motores según el modo.
     */
    void actualizar();
};

#endif
