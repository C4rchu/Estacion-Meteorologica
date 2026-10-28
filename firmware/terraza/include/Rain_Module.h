#ifndef RAIN_MODULE_H
#define RAIN_MODULE_H

#include <Arduino.h>
#include "Sensores.h"

class RainModule {
public:
    RainModule();
    void begin();
    bool estaLloviendo(); // true si llueve (DO LOW), false si no
    int leerIntensidad(); // AO valor
    float getLluviaAcumulada(); // mm de lluvia acumulada
    void resetLluvia();
    static void IRAM_ATTR countTip();

private:
    static volatile unsigned long cantidadVuelcos;
    static volatile unsigned long lastTipTime;
    const float mmPorVuelco = 0.4; // 0.4 mm por vuelco
};


#endif
