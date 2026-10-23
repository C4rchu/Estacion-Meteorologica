#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "DataStructure.h"

// Variables para guardar los datos recibidos
WeatherData receivedData;

// Función callback que se ejecuta al recibir datos
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  Serial.println("Datos recibidos por ESP-NOW:");
  Serial.print("Temp: "); Serial.println(receivedData.tempPromedio);
  // Aquí irá la lógica para actualizar la pantalla con los nuevos datos
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  
  Serial.println("Receptor ESP-NOW listo");
}

void loop() {
  // El loop puede estar libre para la lógica de la pantalla
}
