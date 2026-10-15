#include <Adafruit_GFX.h>
#include <Adafruit_ST7796S.h>
#include <SPI.h>

// Definición de pines para la pantalla ST7796S en el ESP32
#define TFT_CS   15  
#define TFT_DC    2  
#define TFT_RST   4  

Adafruit_ST7796S tft = Adafruit_ST7796S(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  // Inicializar la pantalla ST7796S
  tft.init(320, 480);
  tft.setRotation(1); // Orientación horizontal (Landscape)
  tft.fillScreen(ST77XX_BLACK);

  // Pantalla de bienvenida
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.setCursor(40, 50);
  tft.println("SISTEMA LISTO");

  tft.setTextSize(2);
  tft.setCursor(40, 100);
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Pantalla configurada");
  tft.setCursor(40, 130);
  tft.println("correctamente.");
}

void loop() {
}
