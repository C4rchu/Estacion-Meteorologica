#include <Arduino.h>

const uint8_t pinPluviometro = 27;
const unsigned long reboteMs = 150;

const float mlPorVuelco = 2.0; // Cada vuelco equivale a 2 ml

volatile unsigned long cantidadVuelcos = 0;
volatile unsigned long ultimoVuelcoMs = 0;

// Rutina de interrupción con antirrebote por software
void IRAM_ATTR contarVuelco() {
  const unsigned long ahora = millis();
  if (ahora - ultimoVuelcoMs >= reboteMs) {
    cantidadVuelcos++;
    ultimoVuelcoMs = ahora;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(pinPluviometro, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinPluviometro), contarVuelco, FALLING);

  Serial.println("--- Pluviómetro Inicializado (Modo Mililitros) ---");
}

void loop() {
  static unsigned long ultimoReporteMs = 0;
  static unsigned long ultimoValorProcesado = 0;

  // Revisa cada 500 ms si hubo cambios
  if (millis() - ultimoReporteMs >= 500) {
    ultimoReporteMs = millis();

    // Copia de la variable volátil de forma segura
    noInterrupts();
    unsigned long vuelcos = cantidadVuelcos;
    interrupts();

    // Si hubo un nuevo vuelco, calcula las variables
    if (vuelcos != ultimoValorProcesado) {
      float totalAguaMl = vuelcos * mlPorVuelco;

      // Variables disponibles para usar en tu proyecto:
      // - vuelcos (cantidad de veces que basculó)
      // - totalAguaMl (mililitros acumulados)

      Serial.print("Vuelcos: ");
      Serial.print(vuelcos);
      Serial.print(" | Volumen: ");
      Serial.print(totalAguaMl, 1);
      Serial.println(" ml");

      ultimoValorProcesado = vuelcos;
    }
  }
}