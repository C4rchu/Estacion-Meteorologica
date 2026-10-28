#include "Rain_Module.h"

volatile unsigned long RainModule::cantidadVuelcos = 0;
volatile unsigned long RainModule::lastTipTime = 0;

RainModule::RainModule() {}

void RainModule::begin() {
    pinMode(PIN_YL83_DO, INPUT);
    analogSetAttenuation(ADC_11db);
    
    pinMode(PIN_PLUVIOMETRO, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_PLUVIOMETRO), countTip, FALLING);
}

bool RainModule::estaLloviendo() {
    return digitalRead(PIN_YL83_DO) == LOW; // LOW es activo para este sensor
}

int RainModule::leerIntensidad() {
    return analogRead(PIN_YL83_AO);
}

void IRAM_ATTR RainModule::countTip() {
    unsigned long now = millis();
    if (now - lastTipTime >= 150) { // 150ms debouncing
        cantidadVuelcos++;
        lastTipTime = now;
    }
}

float RainModule::getLluviaAcumulada() {
    noInterrupts();
    unsigned long v = cantidadVuelcos;
    interrupts();
    return v * mmPorVuelco;
}

void RainModule::resetLluvia() {
    noInterrupts();
    cantidadVuelcos = 0;
    interrupts();
}

