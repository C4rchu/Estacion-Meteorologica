#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include "DataStructure.h"

TFT_eSPI tft = TFT_eSPI();
WeatherData receivedData;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 10);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.printf("Temp: %.1f C\n", receivedData.tempPromedio);
    tft.printf("Pres: %.0f hPa", receivedData.presion);
}

void setup() {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) { return; }
    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop() { }
