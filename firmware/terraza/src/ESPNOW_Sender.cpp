#include "ESPNOW_Sender.h"

// MAC del ESP32 Pantalla
uint8_t ESPNOWSender::receiverMAC[] = {0x10, 0x06, 0x1C, 0x83, 0xAC, 0xB8};

ESPNOWSender::ESPNOWSender() {}

void ESPNOWSender::begin() {
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, receiverMAC, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }
}

void ESPNOWSender::sendData(WeatherData data) {
    esp_err_t result = esp_now_send(receiverMAC, (uint8_t *) &data, sizeof(data));
    if (result == ESP_OK) {
        Serial.println("Datos enviados con éxito");
    } else {
        Serial.println("Error al enviar datos");
    }
}
