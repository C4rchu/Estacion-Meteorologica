#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h> // Necesario para configurar el canal
#include <TFT_eSPI.h>
#include "DataStructure.h"

TFT_eSPI tft = TFT_eSPI();
WeatherData receivedData;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    Serial.println("¡Datos recibidos!");
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 10);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.printf("Temp: %.1f C\n", receivedData.tempPromedio);
    tft.printf("Pres: %.0f hPa", receivedData.presion);
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    
    // Forzar canal 1
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    Serial.print("MAC del Receptor: ");
    Serial.println(WiFi.macAddress());
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString("Esperando datos...", 10, 10);

    if (esp_now_init() != ESP_OK) {
        
        Serial.println("Error ESP-NOW"); 
        return; 
        esp_wifi_set_promiscuous(true); 

    }

    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
    Serial.println("Receptor listo. Esperando...");
    Serial.print("Dirección MAC:");
    Serial.println(WiFi.macAddress());
}

void loop() { }
