#include <unity.h>

#include "ComandosRobot.h"
#include "GiroSeguro.h"

void test_comandos_de_movimiento() {
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ComandoRobot::AVANZAR),
                          static_cast<int>(decodificarComando('F')));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ComandoRobot::RETROCEDER),
                          static_cast<int>(decodificarComando('b')));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ComandoRobot::GIRAR_IZQUIERDA),
                          static_cast<int>(decodificarComando('L')));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ComandoRobot::GIRAR_DERECHA),
                          static_cast<int>(decodificarComando('R')));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ComandoRobot::DETENER),
                          static_cast<int>(decodificarComando('s')));
}

void test_comandos_asistidos() {
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ComandoRobot::AVANZAR_RECTO),
                          static_cast<int>(decodificarComando('1')));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ComandoRobot::GIRAR_90_DERECHA),
                          static_cast<int>(decodificarComando('2')));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ComandoRobot::GIRAR_90_IZQUIERDA),
                          static_cast<int>(decodificarComando('3')));
}

void test_separadores_no_son_comandos() {
    TEST_ASSERT_TRUE(esSeparadorComando('\n'));
    TEST_ASSERT_TRUE(esSeparadorComando('\r'));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ComandoRobot::NINGUNO),
                          static_cast<int>(decodificarComando('\n')));
}

void test_giro_se_completa_dentro_de_tolerancia() {
    TEST_ASSERT_EQUAL_INT(static_cast<int>(EstadoGiro::COMPLETADO),
                          static_cast<int>(evaluarGiro(0.8f, 1000, 6000)));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(EstadoGiro::COMPLETADO),
                          static_cast<int>(evaluarGiro(-1.0f, 1000, 6000)));
}

void test_giro_expira_y_no_queda_bloqueado() {
    TEST_ASSERT_EQUAL_INT(static_cast<int>(EstadoGiro::EN_CURSO),
                          static_cast<int>(evaluarGiro(12.0f, 5999, 6000)));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(EstadoGiro::TIEMPO_AGOTADO),
                          static_cast<int>(evaluarGiro(12.0f, 6000, 6000)));
}

void test_giro_corrige_la_direccion_si_sobrepasa_el_objetivo() {
    TEST_ASSERT_EQUAL_INT(1, direccionGiro(12.0f));
    TEST_ASSERT_EQUAL_INT(-1, direccionGiro(-2.0f));
    TEST_ASSERT_EQUAL_INT(0, direccionGiro(0.5f));
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_comandos_de_movimiento);
    RUN_TEST(test_comandos_asistidos);
    RUN_TEST(test_separadores_no_son_comandos);
    RUN_TEST(test_giro_se_completa_dentro_de_tolerancia);
    RUN_TEST(test_giro_expira_y_no_queda_bloqueado);
    RUN_TEST(test_giro_corrige_la_direccion_si_sobrepasa_el_objetivo);
    return UNITY_END();
}
