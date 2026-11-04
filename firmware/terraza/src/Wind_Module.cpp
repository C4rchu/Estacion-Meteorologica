#include "Wind_Module.h"

volatile unsigned int WindModule::pulseCount = 0;
volatile unsigned long WindModule::lastInterruptTime = 0;

WindModule::WindModule() : windSpeed(0), lastTimeSpeed(0) {}

void WindModule::begin() {
    pinMode(PIN_ANEMOMETRO, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_ANEMOMETRO), countPulse, FALLING);

    pinMode(PIN_VELETA_NORTE, INPUT_PULLDOWN);
    pinMode(PIN_VELETA_ESTE, INPUT_PULLDOWN);
    pinMode(PIN_VELETA_SUR, INPUT_PULLDOWN);
    pinMode(PIN_VELETA_OESTE, INPUT_PULLDOWN);
}

void IRAM_ATTR WindModule::countPulse() {
    unsigned long interruptTime = millis();
    if (interruptTime - lastInterruptTime > 15) {
        pulseCount++;
        lastInterruptTime = interruptTime;
    }
}

void WindModule::calcularViento() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastTimeSpeed >= 1000) {
        detachInterrupt(digitalPinToInterrupt(PIN_ANEMOMETRO));
        unsigned int pulses = pulseCount;
        pulseCount = 0;
        lastTimeSpeed = currentMillis;
        attachInterrupt(digitalPinToInterrupt(PIN_ANEMOMETRO), countPulse, FALLING);
        
        windSpeed = (float)pulses / 3.0 * 2.4;
    }
}

float WindModule::getVelocidad() { return windSpeed; }

String WindModule::getDireccion() {
    bool norte = digitalRead(PIN_VELETA_NORTE) == HIGH;
    bool este = digitalRead(PIN_VELETA_ESTE) == HIGH;
    bool sur = digitalRead(PIN_VELETA_SUR) == HIGH;
    bool oeste = digitalRead(PIN_VELETA_OESTE) == HIGH;
    uint8_t activados = norte + este + sur + oeste;

    if (activados == 1) {
        if (norte) return "Norte";
        if (este) return "Este";
        if (sur) return "Sur";
        return "Oeste";
    }

    if (activados == 2) {
        if (norte && este) return "Noreste";
        if (este && sur) return "Sureste";
        if (sur && oeste) return "Suroeste";
        if (oeste && norte) return "Noroeste";
    }

    return "---";
}
