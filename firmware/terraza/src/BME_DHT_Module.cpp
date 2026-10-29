#include "BME_DHT_Module.h"
#include <Wire.h>

ClimaModule::ClimaModule() : dht(PIN_DHT22, DHT22) {}

bool ClimaModule::begin() {
    dht.begin();
    Wire.begin(PIN_BME180_SDA, PIN_BME180_SCL);
    return bmp.begin();
}

void ClimaModule::leerSensores() {
    tempBMP = bmp.readTemperature();
    presBMP = bmp.readPressure() / 100.0F; // Pa a hPa
    tempDHT = dht.readTemperature();
    humDHT = dht.readHumidity();
}

float ClimaModule::getTempPromedio() {
    bool bmpOk = !isnan(tempBMP);
    bool dhtOk = !isnan(tempDHT);
    
    if (bmpOk && dhtOk) return (tempBMP + tempDHT) / 2.0;
    if (bmpOk) return tempBMP;
    if (dhtOk) return tempDHT;
    return 0.0;
}

float ClimaModule::getHumedad() { return humDHT; }
float ClimaModule::getPresion() { return presBMP; }
