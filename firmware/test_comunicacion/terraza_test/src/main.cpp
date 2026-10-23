#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h> 
#include "DataStructure.h"
#include "Sensores.h"

Adafruit_BMP085 bmp;
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void setup() {
    Serial.begin(115200);
    delay(2000); 
    Serial.println("--- INICIANDO SETUP ---");
    
    Wire.begin(PIN_BME180_SDA, PIN_BME180_SCL);
    if (!bmp.begin()) { Serial.println("Error BMP180"); while(1); }
    
    // Configuración robusta de Wi-Fi
    WiFi.mode(WIFI_STA);
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);
    WiFi.disconnect(); 
    
    Serial.print("Canal Wi-Fi: ");
    Serial.println(WiFi.channel());

    if (esp_now_init() != ESP_OK) { Serial.println("Error ESP-NOW"); return; }
    
    // PEQUEÑA ESPERA PARA ESTABILIZAR EL ESP-NOW
    delay(1000); 

    esp_now_peer_info_t peerInfo = {}; // Inicializar en cero
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 1; 
    peerInfo.encrypt = false;
    
    // Intentar añadir el peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Error al añadir peer");
        return;
    }
    Serial.println("ESP-NOW inicializado y peer añadido.");
}

void loop() {
    WeatherData data;
    data.tempPromedio = bmp.readTemperature();
    data.presion = bmp.readPressure() / 100.0F;
    
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &data, sizeof(data));
    
    if (result == ESP_OK) {
        Serial.printf("Enviado con éxito: T=%.2f, P=%.2f\n", data.tempPromedio, data.presion);
    } else {
        Serial.println("Error en el envío");
    }
    delay(2000);
}
