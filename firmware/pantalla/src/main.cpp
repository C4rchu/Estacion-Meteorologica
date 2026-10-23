#include <TFT_eSPI.h>
#include <SPI.h>

// Pines configurados en platformio.ini (segun conexionado_estacion.txt):
// CS=5, DC=2, RST=4, MOSI=23, MISO=19, SCLK=18
TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);

  // Inicializar la pantalla ST7796S
  tft.init();
  tft.setRotation(1); // Orientacion horizontal (Landscape)
  tft.fillScreen(TFT_BLACK);

  // Pantalla de bienvenida
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(40, 50);
  tft.println("SISTEMA LISTO");

  tft.setTextSize(2);
  tft.setCursor(40, 100);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("Pantalla configurada");
  tft.setCursor(40, 130);
  tft.println("correctamente.");

  Serial.println("Pantalla ST7796S inicializada.");
}

void loop() {
}
