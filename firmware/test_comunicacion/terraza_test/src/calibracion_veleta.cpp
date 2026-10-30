#include <Arduino.h>
#include "Sensores.h" // Asegura que PIN_VELETA esté definido aquí

// --- INSTRUCCIONES DE CALIBRACIÓN ---
// 1. Sube este código a tu ESP32 de terraza.
// 2. Abre el Monitor Serie (baud rate 115200).
// 3. Gira la veleta lentamente a cada posición (Norte, Sur, Este, Oeste y posiciones intermedias).
// 4. Anota los valores numéricos que aparecen en pantalla para cada posición.
// 5. Estos valores crudos (0-4095) son los que usaremos para configurar los rangos exactos.
// -------------------------------------

void setup() {
  Serial.begin(115200);
  pinMode(PIN_VELETA, INPUT);
  // Opcional: configurar atenuación si es necesario para tu divisor de voltaje
  // analogSetAttenuation(ADC_11db); 
  Serial.println("Iniciando lectura cruda de la veleta...");
}

void loop() {
  int valorCrudo = analogRead(PIN_VELETA);
  Serial.print("Valor ADC Veleta: ");
  Serial.println(valorCrudo);
  delay(500); // Muestreo cada medio segundo
}
