#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // Dibujar un borde rojo para verificar los límites
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_RED);

  // Mostrar texto
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(20, 50);
  tft.println("HOLA MUNDO!");

  tft.setTextSize(2);
  tft.setCursor(20, 100);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("La pantalla");
  tft.setCursor(20, 130);
  tft.println("esta funcionando.");
}

void loop() {
}
