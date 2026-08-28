#ifndef GIRO_SEGURO_H
#define GIRO_SEGURO_H

#include <stdint.h>

enum class EstadoGiro {
  EN_CURSO,
  COMPLETADO,
  TIEMPO_AGOTADO
};

inline EstadoGiro evaluarGiro(float avanceGrados, float objetivoGrados,
                             uint32_t tiempoTranscurridoMs, uint32_t tiempoMaximoMs) {
  const bool completado = objetivoGrados >= 0.0f ? avanceGrados >= objetivoGrados
                                                  : avanceGrados <= objetivoGrados;
  return completado ? EstadoGiro::COMPLETADO
                    : (tiempoTranscurridoMs >= tiempoMaximoMs ? EstadoGiro::TIEMPO_AGOTADO
                                                              : EstadoGiro::EN_CURSO);
}

#endif
