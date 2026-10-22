#include <Adafruit_GFX.h>
#include <Adafruit_ST7796S.h>
#include <SPI.h>

// Definición de pines para la pantalla ST7796S en el ESP32
#define TFT_CS   15  
#define TFT_DC    2  
#define TFT_RST   4  

// Instancia de la pantalla
Adafruit_ST7796S tft = Adafruit_ST7796S(TFT_CS, TFT_DC, TFT_RST);

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

  // Inicializar pantalla
  tft.init();
  tft.setRotation(3); // Ajusta la rotación según sea necesario
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.setCursor(10, 50);
  tft.println("Iniciando...");

  pinMode(pinPluviometro, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinPluviometro), contarVuelco, FALLING);
}

void loop() {
  static unsigned long ultimoReporteMs = 0;
  static unsigned long ultimoValorMostrado = 0;

  if (millis() - ultimoReporteMs >= 500) {
    ultimoReporteMs = millis();

    noInterrupts();
    unsigned long vuelcos = cantidadVuelcos;
    interrupts();

    if (vuelcos != ultimoValorMostrado) {
      tft.fillScreen(ST77XX_BLACK);
      tft.setCursor(10, 50);
      tft.print("vuelcos: ");
      tft.print(vuelcos);
      ultimoValorMostrado = vuelcos;
    }
  }
}