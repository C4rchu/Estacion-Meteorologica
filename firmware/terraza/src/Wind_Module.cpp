#include "Wind_Module.h"

volatile unsigned int WindModule::pulseCount = 0;
volatile unsigned long WindModule::lastInterruptTime = 0;

WindModule::WindModule() : windSpeed(0), lastTimeSpeed(0) {}

void WindModule::begin() {
    pinMode(PIN_ANEMOMETRO, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_ANEMOMETRO), countPulse, FALLING);
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
    int sensorValue = analogRead(PIN_VELETA);
    // Rangos calibrados: Norte (~4095), Este (~2800), Sur (~1500), Oeste (~600)
    if (sensorValue >= 3500)                       return "Norte";
    if (sensorValue >= 2400 && sensorValue < 3500) return "Este";
    if (sensorValue >= 1100 && sensorValue < 2400) return "Sur";
    if (sensorValue >= 200  && sensorValue < 1100) return "Oeste";
    return "---";
}
