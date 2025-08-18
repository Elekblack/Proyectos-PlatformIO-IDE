/*
 * playCodeBot_Oled para Pantallas OLED V 1.0.1
 * Esta biblioteca dibuja ojos de robot animados de manera suave en pantallas OLED, utilizando las primitivas gráficas de la biblioteca Adafruit GFX,
 * tales como rectángulos redondeados y triángulos. Permite controlar expresiones faciales, animaciones y posiciones de los ojos de forma sencilla.
 * 
 * Copyright (C) 2024 Dennis Hoelscher
 * www.fluxgarage.com
 * www.youtube.com/@FluxGarage
 *
 * Este programa es software libre: puedes redistribuirlo y/o modificarlo
 * bajo los términos de la Licencia Pública General GNU publicada por
 * la Free Software Foundation, ya sea la versión 3 de la Licencia, o
 * (a tu opción) cualquier versión posterior.
 *
 * Este programa se distribuye con la esperanza de que sea útil,
 * pero SIN NINGUNA GARANTÍA; ni siquiera la garantía implícita de
 * COMERCIABILIDAD o APTITUD PARA UN PROPÓSITO PARTICULAR. Consulta la
 * Licencia Pública General GNU para más detalles.
 *
 * Deberías haber recibido una copia de la Licencia Pública General GNU
 * junto con este programa. Si no, consulta <https://www.gnu.org/licenses/>.
 */

#ifndef PLAYCODEBOT_OLED_H
#define PLAYCODEBOT_OLED_H

#include <Adafruit_SSD1306.h> // Incluye la biblioteca Adafruit SSD1306 para manejar la pantalla OLED

// Definiciones para el uso de colores en pantallas monocromáticas.
// COLOR_FONDO se utiliza para el fondo y elementos superpuestos.
// COLOR_PRINCIPAL se utiliza para los dibujos principales de los ojos.
#define COLOR_FONDO 0 // fondo y superposiciones
#define COLOR_PRINCIPAL 1 // dibujos

// Definiciones para el cambio de tipos de estado de ánimo.
// Estos valores se usan en el método establecerEstadoAnimo para seleccionar expresiones como cansado, enojado o feliz.
#define PREDETERMINADO 0
#define CANSADO 1
#define ENOJADO 2
#define FELIZ 3

// Definiciones para activar o desactivar funcionalidades.
// ACTIVADO (1) enciende una opción, DESACTIVADO (0) la apaga.
#define ACTIVADO 1
#define DESACTIVADO 0

// Definiciones para posiciones predefinidas de los ojos.
// Estos valores se usan en el método establecerPosicion para mover los ojos a ubicaciones específicas en la pantalla.
#define NORTE 1 // norte, centro superior
#define NORTE_ESTE 2 // norte-este, superior derecho
#define ESTE 3 // este, medio derecho
#define SUR_ESTE 4 // sur-este, inferior derecho
#define SUR 5 // sur, centro inferior
#define SUR_OESTE 6 // sur-oeste, inferior izquierdo
#define OESTE 7 // oeste, medio izquierdo
#define NORTE_OESTE 8 // norte-oeste, superior izquierdo 
// para centro medio establece "PREDETERMINADO"

class OjosRobot
{
private:
    // Objeto para manejar la pantalla OLED, inicializado en el método inicializar.
    Adafruit_SSD1306* pantalla;

public:
    // Variables para la configuración general de la pantalla y la tasa de cuadros.
    int anchoPantalla = 128; // ancho de la pantalla OLED, en píxeles
    int alturaPantalla = 64; // altura de la pantalla OLED, en píxeles
    int intervaloCuadros = 20; // valor predeterminado para 50 cuadros por segundo (1000/50 = 20 milisegundos)
    unsigned long temporizadorFPS = 0; // para temporizar los cuadros por segundo

    // Variables para controlar los tipos de estado de ánimo y expresiones.
    bool cansado = 0;
    bool enojado = 0;
    bool feliz = 0;
    bool curioso = 0; // si verdadero, dibuja el ojo exterior más grande cuando mira izquierda o derecha
    bool ciclope = 0; // si verdadero, dibuja solo un ojo
    bool ojoI_abierto = 0; // ojo izquierdo abierto o cerrado?
    bool ojoD_abierto = 0; // ojo derecho abierto o cerrado?

    //*********************************************************************************************
    //  Geometría de los Ojos
    //*********************************************************************************************

    // Variables para el ojo izquierdo: tamaño y radio de borde.
    int anchoOjoI_Predeterminado = 36;
    int alturaOjoI_Predeterminado = 36;
    int anchoOjoI_Actual = anchoOjoI_Predeterminado;
    int alturaOjoI_Actual = 1; // empieza con ojo cerrado
    int anchoOjoI_Siguiente = anchoOjoI_Predeterminado;
    int alturaOjoI_Siguiente = alturaOjoI_Predeterminado;
    int offsetAlturaOjoI = 0;
    // Radio de borde
    byte radioBordeOjoI_Predeterminado = 8;
    byte radioBordeOjoI_Actual = radioBordeOjoI_Predeterminado;
    byte radioBordeOjoI_Siguiente = radioBordeOjoI_Predeterminado;

    // Variables para el ojo derecho: similares al ojo izquierdo.
    int anchoOjoD_Predeterminado = anchoOjoI_Predeterminado;
    int alturaOjoD_Predeterminado = alturaOjoI_Predeterminado;
    int anchoOjoD_Actual = anchoOjoD_Predeterminado;
    int alturaOjoD_Actual = 1; // empieza con ojo cerrado
    int anchoOjoD_Siguiente = anchoOjoD_Predeterminado;
    int alturaOjoD_Siguiente = alturaOjoD_Predeterminado;
    int offsetAlturaOjoD = 0;
    // Radio de borde
    byte radioBordeOjoD_Predeterminado = 8;
    byte radioBordeOjoD_Actual = radioBordeOjoD_Predeterminado;
    byte radioBordeOjoD_Siguiente = radioBordeOjoD_Predeterminado;

    // Coordenadas para el ojo izquierdo.
    int xOjoI_Predeterminado = ((anchoPantalla)-(anchoOjoI_Predeterminado+espacioEntre_Predeterminado+anchoOjoD_Predeterminado))/2;
    int yOjoI_Predeterminado = ((alturaPantalla-alturaOjoI_Predeterminado)/2);
    int xOjoI = xOjoI_Predeterminado;
    int yOjoI = yOjoI_Predeterminado;
    int xOjoI_Siguiente = xOjoI;
    int yOjoI_Siguiente = yOjoI;

    // Coordenadas para el ojo derecho.
    int xOjoD_Predeterminado = xOjoI+anchoOjoI_Actual+espacioEntre_Predeterminado;
    int yOjoD_Predeterminado = yOjoI;
    int xOjoD = xOjoD_Predeterminado;
    int yOjoD = yOjoD_Predeterminado;
    int xOjoD_Siguiente = xOjoD;
    int yOjoD_Siguiente = yOjoD;

    // Variables para ambos ojos.
    byte alturaParpadosMax = alturaOjoI_Predeterminado/2; // altura máxima de párpados superiores
    byte alturaParpadosCansado = 0;
    byte alturaParpadosCansado_Siguiente = alturaParpadosCansado;
    byte alturaParpadosEnojado = 0;
    byte alturaParpadosEnojado_Siguiente = alturaParpadosEnojado;
    byte offsetParpadosFelizInferiorMax = (alturaOjoI_Predeterminado/2)+3;
    byte offsetParpadosFelizInferior = 0;
    byte offsetParpadosFelizInferior_Siguiente = 0;
    int espacioEntre_Predeterminado = 10;
    int espacioEntre_Actual = espacioEntre_Predeterminado;
    int espacioEntre_Siguiente = 10;

    //*********************************************************************************************
    //  Animaciones Macro
    //*********************************************************************************************

    bool temblorH = 0;
    bool temblorH_Alternar = 0;
    byte amplitudTemblorH = 2;

    bool temblorV = 0;
    bool temblorV_Alternar = 0;
    byte amplitudTemblorV = 10;

    bool parpadeadorAuto = 0;
    int intervaloParpadeo = 1;
    int variacionIntervaloParpadeo = 4;
    unsigned long temporizadorParpadeo = 0;

    bool inactivo = 0;
    int intervaloInactivo = 1;
    int variacionIntervaloInactivo = 3;
    unsigned long temporizadorAnimacionInactivo = 0;

    bool confundido = 0;
    unsigned long temporizadorAnimacionConfundido = 0;
    int duracionAnimacionConfundido = 500;
    bool alternarConfundido = 1;

    bool reir = 0;
    unsigned long temporizadorAnimacionReir = 0;
    int duracionAnimacionReir = 500;
    bool alternarReir = 1;

    //*********************************************************************************************
    //  MÉTODOS GENERALES
    //*********************************************************************************************

    // Inicializa la biblioteca con el tamaño de pantalla, tasa de cuadros y un objeto de pantalla OLED.
    void inicializar(int ancho, int altura, byte tasaCuadros, Adafruit_SSD1306& pantallaOLED) {
        pantalla = &pantallaOLED; // Asigna el puntero al objeto de pantalla proporcionado
        anchoPantalla = ancho;
        alturaPantalla = altura;
        pantalla->clearDisplay(); // Limpia el buffer de la pantalla
        pantalla->display(); // Muestra la pantalla vacía
        alturaOjoI_Actual = 1; // Ojos cerrados inicialmente
        alturaOjoD_Actual = 1;
        establecerVelocidadCuadros(tasaCuadros);
    }

    void actualizar() {
        if (millis() - temporizadorFPS >= intervaloCuadros) {
            dibujarOjos();
            temporizadorFPS = millis();
        }
    }

    //*********************************************************************************************
    //  MÉTODOS SETTERS
    //*********************************************************************************************

    void establecerVelocidadCuadros(byte fps) {
        intervaloCuadros = 1000 / fps;
    }

    void establecerAncho(byte ojoIzquierdo, byte ojoDerecho) {
        anchoOjoI_Siguiente = ojoIzquierdo;
        anchoOjoD_Siguiente = ojoDerecho;
        anchoOjoI_Predeterminado = ojoIzquierdo;
        anchoOjoD_Predeterminado = ojoDerecho;
    }

    void establecerAltura(byte ojoIzquierdo, byte ojoDerecho) {
        alturaOjoI_Siguiente = ojoIzquierdo;
        alturaOjoD_Siguiente = ojoDerecho;
        alturaOjoI_Predeterminado = ojoIzquierdo;
        alturaOjoD_Predeterminado = ojoDerecho;
    }

    void establecerRadioBorde(byte ojoIzquierdo, byte ojoDerecho) {
        radioBordeOjoI_Siguiente = ojoIzquierdo;
        radioBordeOjoD_Siguiente = ojoDerecho;
        radioBordeOjoI_Predeterminado = ojoIzquierdo;
        radioBordeOjoD_Predeterminado = ojoDerecho;
    }

    void establecerEspacioEntre(int espacio) {
        espacioEntre_Siguiente = espacio;
        espacioEntre_Predeterminado = espacio;
    }

    void establecerEstadoAnimo(unsigned char estadoAnimo) {
        switch (estadoAnimo) {
            case CANSADO:
                cansado = 1;
                enojado = 0;
                feliz = 0;
                break;
            case ENOJADO:
                cansado = 0;
                enojado = 1;
                feliz = 0;
                break;
            case FELIZ:
                cansado = 0;
                enojado = 0;
                feliz = 1;
                break;
            default:
                cansado = 0;
                enojado = 0;
                feliz = 0;
                break;
        }
    }

    void establecerPosicion(unsigned char posicion) {
        switch (posicion) {
            case NORTE:
                xOjoI_Siguiente = obtenerLimitePantalla_X() / 2;
                yOjoI_Siguiente = 0;
                break;
            case NORTE_ESTE:
                xOjoI_Siguiente = obtenerLimitePantalla_X();
                yOjoI_Siguiente = 0;
                break;
            case ESTE:
                xOjoI_Siguiente = obtenerLimitePantalla_X();
                yOjoI_Siguiente = obtenerLimitePantalla_Y() / 2;
                break;
            case SUR_ESTE:
                xOjoI_Siguiente = obtenerLimitePantalla_X();
                yOjoI_Siguiente = obtenerLimitePantalla_Y();
                break;
            case SUR:
                xOjoI_Siguiente = obtenerLimitePantalla_X() / 2;
                yOjoI_Siguiente = obtenerLimitePantalla_Y();
                break;
            case SUR_OESTE:
                xOjoI_Siguiente = 0;
                yOjoI_Siguiente = obtenerLimitePantalla_Y();
                break;
            case OESTE:
                xOjoI_Siguiente = 0;
                yOjoI_Siguiente = obtenerLimitePantalla_Y() / 2;
                break;
            case NORTE_OESTE:
                xOjoI_Siguiente = 0;
                yOjoI_Siguiente = 0;
                break;
            default:
                xOjoI_Siguiente = obtenerLimitePantalla_X() / 2;
                yOjoI_Siguiente = obtenerLimitePantalla_Y() / 2;
                break;
        }
    }

    void establecerParpadeoAutomatico(bool activo, int intervalo, int variacion) {
        parpadeadorAuto = activo;
        intervaloParpadeo = intervalo;
        variacionIntervaloParpadeo = variacion;
    }

    void establecerParpadeoAutomatico(bool activo) {
        parpadeadorAuto = activo;
    }

    void establecerModoInactivo(bool activo, int intervalo, int variacion) {
        inactivo = activo;
        intervaloInactivo = intervalo;
        variacionIntervaloInactivo = variacion;
    }

    void establecerModoInactivo(bool activo) {
        inactivo = activo;
    }

    void establecerCuriosidad(bool bitCurioso) {
        curioso = bitCurioso;
    }

    void establecerCiclope(bool bitCiclope) {
        ciclope = bitCiclope;
    }

    void establecerTemblorH(bool bitTemblor, byte amplitud) {
        temblorH = bitTemblor;
        amplitudTemblorH = amplitud;
    }

    void establecerTemblorH(bool bitTemblor) {
        temblorH = bitTemblor;
    }

    void establecerTemblorV(bool bitTemblor, byte amplitud) {
        temblorV = bitTemblor;
        amplitudTemblorV = amplitud;
    }

    void establecerTemblorV(bool bitTemblor) {
        temblorV = bitTemblor;
    }

    //*********************************************************************************************
    //  MÉTODOS GETTERS
    //*********************************************************************************************

    int obtenerLimitePantalla_X() {
        return anchoPantalla - anchoOjoI_Actual - espacioEntre_Actual - anchoOjoD_Actual;
    }

    int obtenerLimitePantalla_Y() {
        return alturaPantalla - alturaOjoI_Predeterminado;
    }

    //*********************************************************************************************
    //  MÉTODOS DE ANIMACIÓN BÁSICA
    //*********************************************************************************************

    void cerrar() {
        alturaOjoI_Siguiente = 1;
        alturaOjoD_Siguiente = 1;
        ojoI_abierto = 0;
        ojoD_abierto = 0;
    }

    void abrir() {
        ojoI_abierto = 1;
        ojoD_abierto = 1;
    }

    void parpadear() {
        cerrar();
        abrir();
    }

    void cerrar(bool izquierdo, bool derecho) {
        if (izquierdo) {
            alturaOjoI_Siguiente = 1;
            ojoI_abierto = 0;
        }
        if (derecho) {
            alturaOjoD_Siguiente = 1;
            ojoD_abierto = 0;
        }
    }

    void abrir(bool izquierdo, bool derecho) {
        if (izquierdo) {
            ojoI_abierto = 1;
        }
        if (derecho) {
            ojoD_abierto = 1;
        }
    }

    void parpadear(bool izquierdo, bool derecho) {
        cerrar(izquierdo, derecho);
        abrir(izquierdo, derecho);
    }

    //*********************************************************************************************
    //  MÉTODOS DE ANIMACIÓN MACRO
    //*********************************************************************************************

    void animacion_confundido() {
        confundido = 1;
    }

    void animacion_reir() {
        reir = 1;
    }

    //*********************************************************************************************
    //  PRE-CÁLCULOS Y DIBUJOS ACTUALES
    //*********************************************************************************************

    void dibujarOjos() {
        // Pre-cálculos para transiciones suaves
        if (curioso) {
            if (xOjoI_Siguiente <= 10) {
                offsetAlturaOjoI = 8;
            } else if (xOjoI_Siguiente >= (obtenerLimitePantalla_X() - 10) && ciclope) {
                offsetAlturaOjoI = 8;
            } else {
                offsetAlturaOjoI = 0;
            }
            if (xOjoD_Siguiente >= anchoPantalla - anchoOjoD_Actual - 10) {
                offsetAlturaOjoD = 8;
            } else {
                offsetAlturaOjoD = 0;
            }
        } else {
            offsetAlturaOjoI = 0;
            offsetAlturaOjoD = 0;
        }

        alturaOjoI_Actual = (alturaOjoI_Actual + alturaOjoI_Siguiente + offsetAlturaOjoI) / 2;
        yOjoI += ((alturaOjoI_Predeterminado - alturaOjoI_Actual) / 2);
        yOjoI -= offsetAlturaOjoI / 2;
        alturaOjoD_Actual = (alturaOjoD_Actual + alturaOjoD_Siguiente + offsetAlturaOjoD) / 2;
        yOjoD += (alturaOjoD_Predeterminado - alturaOjoD_Actual) / 2;
        yOjoD -= offsetAlturaOjoD / 2;

        if (ojoI_abierto) {
            if (alturaOjoI_Actual <= 1 + offsetAlturaOjoI) {
                alturaOjoI_Siguiente = alturaOjoI_Predeterminado;
            }
        }
        if (ojoD_abierto) {
            if (alturaOjoD_Actual <= 1 + offsetAlturaOjoD) {
                alturaOjoD_Siguiente = alturaOjoD_Predeterminado;
            }
        }

        anchoOjoI_Actual = (anchoOjoI_Actual + anchoOjoI_Siguiente) / 2;
        anchoOjoD_Actual = (anchoOjoD_Actual + anchoOjoD_Siguiente) / 2;

        espacioEntre_Actual = (espacioEntre_Actual + espacioEntre_Siguiente) / 2;

        xOjoI = (xOjoI + xOjoI_Siguiente) / 2;
        yOjoI = (yOjoI + yOjoI_Siguiente) / 2;
        xOjoD_Siguiente = xOjoI_Siguiente + anchoOjoI_Actual + espacioEntre_Actual;
        yOjoD_Siguiente = yOjoI_Siguiente;
        xOjoD = (xOjoD + xOjoD_Siguiente) / 2;
        yOjoD = (yOjoD + yOjoD_Siguiente) / 2;

        radioBordeOjoI_Actual = (radioBordeOjoI_Actual + radioBordeOjoI_Siguiente) / 2;
        radioBordeOjoD_Actual = (radioBordeOjoD_Actual + radioBordeOjoD_Siguiente) / 2;

        // Aplicando animaciones macro
        if (parpadeadorAuto) {
            if (millis() >= temporizadorParpadeo) {
                parpadear();
                temporizadorParpadeo = millis() + (intervaloParpadeo * 1000) + (random(variacionIntervaloParpadeo) * 1000);
            }
        }

        if (reir) {
            if (alternarReir) {
                establecerTemblorV(1, 5);
                temporizadorAnimacionReir = millis();
                alternarReir = 0;
            } else if (millis() >= temporizadorAnimacionReir + duracionAnimacionReir) {
                establecerTemblorV(0, 0);
                alternarReir = 1;
                reir = 0;
            }
        }

        if (confundido) {
            if (alternarConfundido) {
                establecerTemblorH(1, 20);
                temporizadorAnimacionConfundido = millis();
                alternarConfundido = 0;
            } else if (millis() >= temporizadorAnimacionConfundido + duracionAnimacionConfundido) {
                establecerTemblorH(0, 0);
                alternarConfundido = 1;
                confundido = 0;
            }
        }

        if (inactivo) {
            if (millis() >= temporizadorAnimacionInactivo) {
                xOjoI_Siguiente = random(obtenerLimitePantalla_X());
                yOjoI_Siguiente = random(obtenerLimitePantalla_Y());
                temporizadorAnimacionInactivo = millis() + (intervaloInactivo * 1000) + (random(variacionIntervaloInactivo) * 1000);
            }
        }

        if (temblorH) {
            if (temblorH_Alternar) {
                xOjoI += amplitudTemblorH;
                xOjoD += amplitudTemblorH;
            } else {
                xOjoI -= amplitudTemblorH;
                xOjoD -= amplitudTemblorH;
            }
            temblorH_Alternar = !temblorH_Alternar;
        }

        if (temblorV) {
            if (temblorV_Alternar) {
                yOjoI += amplitudTemblorV;
                yOjoD += amplitudTemblorV;
            } else {
                yOjoI -= amplitudTemblorV;
                yOjoD -= amplitudTemblorV;
            }
            temblorV_Alternar = !temblorV_Alternar;
        }

        if (ciclope) {
            anchoOjoD_Actual = 0;
            alturaOjoD_Actual = 0;
            espacioEntre_Actual = 0;
        }

        // Dibujos actuales
        pantalla->clearDisplay();

        pantalla->fillRoundRect(xOjoI, yOjoI, anchoOjoI_Actual, alturaOjoI_Actual, radioBordeOjoI_Actual, COLOR_PRINCIPAL);
        if (!ciclope) {
            pantalla->fillRoundRect(xOjoD, yOjoD, anchoOjoD_Actual, alturaOjoD_Actual, radioBordeOjoD_Actual, COLOR_PRINCIPAL);
        }

        if (cansado) {
            alturaParpadosCansado_Siguiente = alturaOjoI_Actual / 2;
            alturaParpadosEnojado_Siguiente = 0;
        } else {
            alturaParpadosCansado_Siguiente = 0;
        }
        if (enojado) {
            alturaParpadosEnojado_Siguiente = alturaOjoI_Actual / 2;
            alturaParpadosCansado_Siguiente = 0;
        } else {
            alturaParpadosEnojado_Siguiente = 0;
        }
        if (feliz) {
            offsetParpadosFelizInferior_Siguiente = alturaOjoI_Actual / 2;
        } else {
            offsetParpadosFelizInferior_Siguiente = 0;
        }

        alturaParpadosCansado = (alturaParpadosCansado + alturaParpadosCansado_Siguiente) / 2;
        if (!ciclope) {
            pantalla->fillTriangle(xOjoI, yOjoI - 1, xOjoI + anchoOjoI_Actual, yOjoI - 1, xOjoI, yOjoI + alturaParpadosCansado - 1, COLOR_FONDO);
            pantalla->fillTriangle(xOjoD, yOjoD - 1, xOjoD + anchoOjoD_Actual, yOjoD - 1, xOjoD + anchoOjoD_Actual, yOjoD + alturaParpadosCansado - 1, COLOR_FONDO);
        } else {
            pantalla->fillTriangle(xOjoI, yOjoI - 1, xOjoI + (anchoOjoI_Actual / 2), yOjoI - 1, xOjoI, yOjoI + alturaParpadosCansado - 1, COLOR_FONDO);
            pantalla->fillTriangle(xOjoI + (anchoOjoI_Actual / 2), yOjoI - 1, xOjoI + anchoOjoI_Actual, yOjoI - 1, xOjoI + anchoOjoI_Actual, yOjoI + alturaParpadosCansado - 1, COLOR_FONDO);
        }

        alturaParpadosEnojado = (alturaParpadosEnojado + alturaParpadosEnojado_Siguiente) / 2;
        if (!ciclope) {
            pantalla->fillTriangle(xOjoI, yOjoI - 1, xOjoI + anchoOjoI_Actual, yOjoI - 1, xOjoI + anchoOjoI_Actual, yOjoI + alturaParpadosEnojado - 1, COLOR_FONDO);
            pantalla->fillTriangle(xOjoD, yOjoD - 1, xOjoD + anchoOjoD_Actual, yOjoD - 1, xOjoD, yOjoD + alturaParpadosEnojado - 1, COLOR_FONDO);
        } else {
            pantalla->fillTriangle(xOjoI, yOjoI - 1, xOjoI + (anchoOjoI_Actual / 2), yOjoI - 1, xOjoI + (anchoOjoI_Actual / 2), yOjoI + alturaParpadosEnojado - 1, COLOR_FONDO);
            pantalla->fillTriangle(xOjoI + (anchoOjoI_Actual / 2), yOjoI - 1, xOjoI + anchoOjoI_Actual, yOjoI - 1, xOjoI + (anchoOjoI_Actual / 2), yOjoI + alturaParpadosEnojado - 1, COLOR_FONDO);
        }

        offsetParpadosFelizInferior = (offsetParpadosFelizInferior + offsetParpadosFelizInferior_Siguiente) / 2;
        pantalla->fillRoundRect(xOjoI - 1, (yOjoI + alturaOjoI_Actual) - offsetParpadosFelizInferior + 1, anchoOjoI_Actual + 2, alturaOjoI_Predeterminado, radioBordeOjoI_Actual, COLOR_FONDO);
        if (!ciclope) {
            pantalla->fillRoundRect(xOjoD - 1, (yOjoD + alturaOjoD_Actual) - offsetParpadosFelizInferior + 1, anchoOjoD_Actual + 2, alturaOjoD_Predeterminado, radioBordeOjoD_Actual, COLOR_FONDO);
        }

        pantalla->display();
    }
};
#endif // PLAYCODEBOT_OLED_H
// Fin de la definición de la clase OjosRobot