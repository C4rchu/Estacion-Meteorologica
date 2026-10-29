#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "DataStructure.h"
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
WeatherData receivedData;
bool newData = false;
bool dataReceivedOnce = false;

enum ScreenState { RESUMEN, SENSORES, VIENTO, LLUVIA };
ScreenState currentState = RESUMEN;
unsigned long lastScreenChange = 0;
const long screenInterval = 5000; // 5 segundos

void drawWaiting() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString("Esperando datos...", 10, 10);
}

void drawResumen() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString("RESUMEN", 10, 10);
    tft.setTextSize(3);
    tft.drawString("Temp: " + String(receivedData.tempPromedio, 1) + " C", 10, 60);
    tft.drawString("Hum: " + String(receivedData.humedad, 1) + " %", 10, 100);
}

void drawSensores() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString("SENSORES", 10, 10);
    tft.setTextSize(3);
    tft.drawString("UV: " + String(receivedData.uv, 1), 10, 60);
    tft.drawString("Pres: " + String(receivedData.presion, 0) + " hPa", 10, 100);
}

void drawViento() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString("VIENTO", 10, 10);
    tft.setTextSize(3);
    tft.drawString("Vel: " + String(receivedData.velocidadViento, 1) + " km/h", 10, 60);
    tft.drawString("Dir: " + String(receivedData.direccionViento), 10, 100);
}

void drawLluvia() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString("LLUVIA", 10, 10);
    tft.setTextSize(3);
    tft.drawString("Acum: " + String(receivedData.lluviaAcumulada, 1) + " mm", 10, 60);
    tft.drawString(receivedData.llueve ? "Lloviendo" : "Seco", 10, 100);
}

void drawCurrentScreen() {
    switch (currentState) {
        case RESUMEN: drawResumen(); break;
        case SENSORES: drawSensores(); break;
        case VIENTO: drawViento(); break;
        case LLUVIA: drawLluvia(); break;
    }
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    newData = true;
    dataReceivedOnce = true;
}

void setup() {
    Serial.begin(115200);
    tft.init();
    tft.setRotation(1);
    drawWaiting();

    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
    
    Serial.println("Receptor ESP-NOW listo");
}

void loop() {
    if (!dataReceivedOnce) return;

    // Cambio automático de pantalla
    if (millis() - lastScreenChange > screenInterval) {
        lastScreenChange = millis();
        currentState = (ScreenState)((currentState + 1) % 4);
        drawCurrentScreen();
    }
    // Actualización si llegan datos nuevos
    if (newData) {
        drawCurrentScreen();
        newData = false;
    }
}
