#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "DataStructure.h"
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
WeatherData receivedData;
bool newData = false;
bool dataReceivedOnce = false;

// Historial para gráficas
const int HIST_SIZE = 30;
float histTemp[HIST_SIZE] = {0};
float histHum[HIST_SIZE] = {0};
float histPres[HIST_SIZE] = {0};
float histVel[HIST_SIZE] = {0};
float histUV[HIST_SIZE] = {0};
float histLluvia[HIST_SIZE] = {0};
int histCount = 0;

void updateHistory(float newValue, float *historyArray) {
    for (int i = 0; i < HIST_SIZE - 1; i++) {
        historyArray[i] = historyArray[i + 1];
    }
    historyArray[HIST_SIZE - 1] = newValue;
}

void drawGraph(int x, int y, int w, int h, float *data, uint32_t color, float minVal, float maxVal) {
    tft.drawRect(x, y, w, h, TFT_WHITE);
    if (histCount < 2) return;
    int step = w / (HIST_SIZE - 1);
    for (int i = 0; i < HIST_SIZE - 1; i++) {
        int x1 = x + i * step;
        int y1 = y + h - (int)((data[i] - minVal) / (maxVal - minVal) * h);
        int x2 = x + (i + 1) * step;
        int y2 = y + h - (int)((data[i+1] - minVal) / (maxVal - minVal) * h);
        tft.drawLine(x1, y1, x2, y2, color);
    }
}

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
    drawGraph(10, 100, 300, 80, histTemp, TFT_RED, 0, 50); // Ajustar rangos según necesidad
    tft.drawString("Hum: " + String(receivedData.humedad, 1) + " %", 10, 200);
    drawGraph(10, 240, 300, 80, histHum, TFT_BLUE, 0, 100);
}

void drawSensores() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString("SENSORES", 10, 10);
    tft.setTextSize(3);
    tft.drawString("UV: " + String(receivedData.uv, 1), 10, 60);
    drawGraph(10, 100, 300, 80, histUV, TFT_YELLOW, 0, 15);
    tft.drawString("Pres: " + String(receivedData.presion, 0) + " hPa", 10, 200);
    drawGraph(10, 240, 300, 80, histPres, TFT_GREEN, 900, 1100);
}

void drawViento() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString("VIENTO", 10, 10);
    tft.setTextSize(3);
    tft.drawString("Vel: " + String(receivedData.velocidadViento, 1) + " km/h", 10, 60);
    drawGraph(10, 100, 300, 150, histVel, TFT_CYAN, 0, 100);
    tft.setTextSize(2);
    tft.drawString("Dir: " + String(receivedData.direccionViento), 10, 260);
}

void drawLluvia() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString("LLUVIA", 10, 10);
    tft.setTextSize(3);
    tft.drawString("Acum: " + String(receivedData.lluviaAcumulada, 1) + " mm", 10, 60);
    drawGraph(10, 100, 300, 150, histLluvia, TFT_BLUE, 0, 50);
    tft.setTextSize(2);
    tft.drawString(receivedData.llueve ? "Lloviendo" : "Seco", 10, 300);
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
    Serial.println("Datos recibidos por ESP-NOW");
    WeatherData tempData;
    memcpy(&tempData, incomingData, sizeof(tempData));
    
    // Debug
    Serial.print("Recibido Temp: "); Serial.println(tempData.tempPromedio);
    Serial.print("Recibido Dir: "); Serial.println(tempData.direccionViento);
    
    receivedData = tempData;
    newData = true;
    dataReceivedOnce = true;
    
    updateHistory(receivedData.tempPromedio, histTemp);
    updateHistory(receivedData.humedad, histHum);
    updateHistory(receivedData.presion, histPres);
    updateHistory(receivedData.velocidadViento, histVel);
    updateHistory(receivedData.uv, histUV);
    updateHistory(receivedData.lluviaAcumulada, histLluvia);
    if (histCount < HIST_SIZE) histCount++;
}

void setup() {
    Serial.begin(115200);
    tft.init();
    tft.setRotation(1);
    
    // Test visual
    tft.fillScreen(TFT_RED);
    delay(500);
    drawWaiting();

    WiFi.mode(WIFI_STA);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
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
