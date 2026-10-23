#ifndef ESPNOW_SENDER_H
#define ESPNOW_SENDER_H

#include <esp_now.h>
#include <WiFi.h>
#include "DataStructure.h"

class ESPNOWSender {
public:
    ESPNOWSender();
    void begin();
    void sendData(WeatherData data);

private:
    static uint8_t receiverMAC[];
};

#endif
