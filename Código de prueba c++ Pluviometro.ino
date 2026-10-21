// Prueba inicial de un pluviometro de balancin con contacto reed y ESP32.
// Cada vuelco del balancin produce un pulso que se cuenta en el Monitor Serie.

const uint8_t pinPluviometro = 27;
const unsigned long reboteMs = 150;

volatile unsigned long cantidadVuelcos = 0;
volatile unsigned long ultimoVuelcoMs = 0;

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

  Serial.println("Prueba de pluviometro iniciada");
  Serial.println("Haz bascular el balancin y observa el contador.");
}

void loop() {
  static unsigned long ultimoReporteMs = 0;

  if (millis() - ultimoReporteMs >= 1000) {
    ultimoReporteMs = millis();

    noInterrupts();
    const unsigned long vuelcos = cantidadVuelcos;
    interrupts();

    Serial.print("Vuelcos acumulados: ");
    Serial.println(vuelcos);
  }
}