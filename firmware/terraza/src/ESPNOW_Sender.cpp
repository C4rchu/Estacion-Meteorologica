#include "ESPNOW_Sender.h"
#include <esp_wifi.h>

// MAC del ESP32 Pantalla
uint8_t ESPNOWSender::receiverMAC[] = {0x68, 0x09, 0x47, 0x57, 0xC9, 0xDC};

ESPNOWSender::ESPNOWSender() {}

void ESPNOWSender::begin() {
    WiFi.mode(WIFI_STA);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo)); // Importante inicializar
    memcpy(peerInfo.peer_addr, receiverMAC, 6);
    peerInfo.channel = 0;  // Canal automático
    peerInfo.ifidx = WIFI_IF_STA;
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
        Serial.print("Error al enviar datos. Código: ");
        Serial.println(result);
    }
}
