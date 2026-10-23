#include "Rain_Module.h"

RainModule::RainModule() {}

void RainModule::begin() {
    pinMode(PIN_YL83_DO, INPUT);
    analogSetAttenuation(ADC_11db);
}

bool RainModule::estaLloviendo() {
    return digitalRead(PIN_YL83_DO) == LOW; // LOW es activo para este sensor
}

int RainModule::leerIntensidad() {
    return analogRead(PIN_YL83_AO);
}
