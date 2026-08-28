#ifndef GIRO_SEGURO_H
#define GIRO_SEGURO_H

#include <stdint.h>

enum class EstadoGiro {
    EN_CURSO,
    COMPLETADO,
    TIEMPO_AGOTADO
};

constexpr EstadoGiro evaluarGiro(float errorGrados, uint32_t tiempoTranscurridoMs,
                                 uint32_t tiempoMaximoMs, float toleranciaGrados = 1.0f) {
    return (errorGrados >= -toleranciaGrados && errorGrados <= toleranciaGrados)
               ? EstadoGiro::COMPLETADO
               : (tiempoTranscurridoMs >= tiempoMaximoMs ? EstadoGiro::TIEMPO_AGOTADO
                                                          : EstadoGiro::EN_CURSO);
}

constexpr int direccionGiro(float errorGrados, float toleranciaGrados = 1.0f) {
    return errorGrados > toleranciaGrados ? 1 : (errorGrados < -toleranciaGrados ? -1 : 0);
}

#endif
