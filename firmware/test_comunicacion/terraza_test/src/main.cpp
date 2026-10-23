#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <esp_now.h>
#include <WiFi.h>
#include "DataStructure.h"
#include "Sensores.h"

Adafruit_BMP085 bmp;
uint8_t receiverMAC[] = {0x68, 0x09, 0x47, 0x57, 0xC9, 0xDC};

void setup() {
    Serial.begin(115200);
    Wire.begin(PIN_BME180_SDA, PIN_BME180_SCL);
    if (!bmp.begin()) { Serial.println("Error BMP180"); while(1); }
    
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) { Serial.println("Error ESP-NOW"); return; }
    
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, receiverMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);
}

void loop() {
    WeatherData data;
    data.tempPromedio = bmp.readTemperature();
    data.presion = bmp.readPressure() / 100.0F;
    
    esp_now_send(receiverMAC, (uint8_t *) &data, sizeof(data));
    Serial.printf("Enviado: T=%.2f, P=%.2f\n", data.tempPromedio, data.presion);
    delay(2000);
}
