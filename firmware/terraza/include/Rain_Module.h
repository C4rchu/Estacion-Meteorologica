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

private:
};

#endif
