#include <unity.h>

#include "ComandosRobot.h"
#include "GiroSeguro.h"

void test_comandos_aceptan_mayusculas_y_minusculas() {
  TEST_ASSERT_EQUAL_INT(static_cast<int>(ComandoRobot::AVANZAR),
                        static_cast<int>(decodificarComando('F')));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(ComandoRobot::RETROCEDER),
                        static_cast<int>(decodificarComando('b')));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(ComandoRobot::GIRAR_IZQUIERDA),
                        static_cast<int>(decodificarComando('l')));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(ComandoRobot::GIRAR_DERECHA),
                        static_cast<int>(decodificarComando('R')));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(ComandoRobot::DETENER),
                        static_cast<int>(decodificarComando('s')));
}

void test_comandos_de_giro_preciso() {
  TEST_ASSERT_EQUAL_INT(static_cast<int>(ComandoRobot::GIRAR_90_DERECHA),
                        static_cast<int>(decodificarComando('1')));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(ComandoRobot::GIRAR_90_IZQUIERDA),
                        static_cast<int>(decodificarComando('2')));
}

void test_ignora_separadores_de_linea() {
  TEST_ASSERT_TRUE(esSeparadorComando('\n'));
  TEST_ASSERT_TRUE(esSeparadorComando('\r'));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(ComandoRobot::NINGUNO),
                        static_cast<int>(decodificarComando('X')));
}

void test_giro_positivo_y_negativo_completan() {
  TEST_ASSERT_EQUAL_INT(static_cast<int>(EstadoGiro::COMPLETADO),
                        static_cast<int>(evaluarGiro(90.0f, 90.0f, 1500, 6000)));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(EstadoGiro::COMPLETADO),
                        static_cast<int>(evaluarGiro(-91.0f, -90.0f, 1500, 6000)));
}

void test_giro_expira_si_el_sensor_no_avanza() {
  TEST_ASSERT_EQUAL_INT(static_cast<int>(EstadoGiro::EN_CURSO),
                        static_cast<int>(evaluarGiro(0.0f, 90.0f, 5999, 6000)));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(EstadoGiro::TIEMPO_AGOTADO),
                        static_cast<int>(evaluarGiro(0.0f, 90.0f, 6000, 6000)));
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_comandos_aceptan_mayusculas_y_minusculas);
  RUN_TEST(test_comandos_de_giro_preciso);
  RUN_TEST(test_ignora_separadores_de_linea);
  RUN_TEST(test_giro_positivo_y_negativo_completan);
  RUN_TEST(test_giro_expira_si_el_sensor_no_avanza);
  return UNITY_END();
}
