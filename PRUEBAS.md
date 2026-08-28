# Pruebas automatizadas

Desde la raíz del repositorio, ejecuta:

```bash
./scripts/test_all.sh
```

El script compila los cinco firmwares y ejecuta las pruebas unitarias nativas de
los dos controladores `PlayCodeBot`. Las pruebas cubren:

- decodificación de comandos Bluetooth, incluidas minúsculas;
- correspondencia correcta entre izquierda y derecha;
- descarte de saltos de línea enviados por terminales Bluetooth;
- finalización de giros positivos y negativos;
- corrección de dirección cuando un giro sobrepasa su objetivo;
- cancelación de un giro cuando vence el tiempo máximo de seguridad.

Las pruebas nativas no necesitan una placa conectada. La validación de motores,
MPU6050, pantallas y joystick sobre hardware real sigue requiriendo pruebas de
integración en los dispositivos.
