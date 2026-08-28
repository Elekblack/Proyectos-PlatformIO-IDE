#ifndef COMANDOS_ROBOT_H
#define COMANDOS_ROBOT_H

enum class ComandoRobot {
    NINGUNO,
    AVANZAR,
    RETROCEDER,
    GIRAR_IZQUIERDA,
    GIRAR_DERECHA,
    DETENER,
    AVANZAR_RECTO,
    GIRAR_90_DERECHA,
    GIRAR_90_IZQUIERDA
};

inline ComandoRobot decodificarComando(char comando) {
    switch (comando) {
        case 'F': case 'f': return ComandoRobot::AVANZAR;
        case 'B': case 'b': return ComandoRobot::RETROCEDER;
        case 'L': case 'l': return ComandoRobot::GIRAR_IZQUIERDA;
        case 'R': case 'r': return ComandoRobot::GIRAR_DERECHA;
        case 'S': case 's': return ComandoRobot::DETENER;
        case '1': return ComandoRobot::AVANZAR_RECTO;
        case '2': return ComandoRobot::GIRAR_90_DERECHA;
        case '3': return ComandoRobot::GIRAR_90_IZQUIERDA;
        default: return ComandoRobot::NINGUNO;
    }
}

constexpr bool esSeparadorComando(char comando) {
    return comando == '\r' || comando == '\n' || comando == ' ' || comando == '\t';
}

#endif
