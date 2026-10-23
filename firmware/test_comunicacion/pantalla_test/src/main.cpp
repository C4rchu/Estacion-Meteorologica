#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <TFT_eSPI.h>
#include "DataStructure.h"

TFT_eSPI tft = TFT_eSPI();
WeatherData receivedData;
int currentScreen = 0;
const int numScreens = 3;

void drawScreen() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    
    // Placeholder para el icono (zona superior derecha)
    tft.setCursor(220, 10);
    tft.setTextSize(2);
    tft.print("[Icon]"); 

    tft.setCursor(10, 10);
    switch (currentScreen) {
        case 0:
            tft.setTextSize(3);
            tft.println("METEO");
            tft.setTextSize(2);
            tft.printf("Temp: %.1f C\n", receivedData.tempPromedio);
            tft.printf("Hum : %.1f %%", receivedData.humedad);
            break;
        case 1:
            tft.setTextSize(3);
            tft.println("INFO");
            tft.setTextSize(2);
            tft.printf("Pres: %.0f hPa\n", receivedData.presion);
            tft.printf("Lluvia: %s", receivedData.llueve ? "Si" : "No");
            break;
        case 2:
            tft.setTextSize(3);
            tft.println("VIENTO/UV");
            tft.setTextSize(2);
            tft.printf("Vel: %.1f km/h\n", receivedData.velocidadViento);
            tft.printf("Dir: %s\n", receivedData.direccionViento);
            tft.printf("UV: %.2f", receivedData.uv);
            break;
    }
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    drawScreen(); 
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    
    // Configuración de calibración de touch
    uint16_t calData[5] = { 356, 3567, 265, 3497, 7 };
    tft.setTouch(calData);

    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString("Esperando datos...", 10, 10);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error ESP-NOW"); 
        return; 
    }

    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop() {
    uint16_t touchX = 0, touchY = 0;
    
    if (tft.getTouch(&touchX, &touchY)) {
        // Debounce simple
        delay(300); 
        currentScreen = (currentScreen + 1) % numScreens;
        drawScreen();
    }
}
