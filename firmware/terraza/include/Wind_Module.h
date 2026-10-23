#ifndef WIND_MODULE_H
#define WIND_MODULE_H

#include <Arduino.h>
#include "Sensores.h"

class WindModule {
public:
    WindModule();
    void begin();
    void calcularViento();
    float getVelocidad();
    String getDireccion();
    static void IRAM_ATTR countPulse();

private:
    static volatile unsigned int pulseCount;
    static volatile unsigned long lastInterruptTime;
    float windSpeed;
    unsigned long lastTimeSpeed;
};

#endif
