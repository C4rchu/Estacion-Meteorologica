#ifndef BME_DHT_MODULE_H
#define BME_DHT_MODULE_H

#include <Adafruit_BMP085.h>
#include <DHT.h>
#include "Sensores.h"
#include <math.h>

class ClimaModule {
public:
    ClimaModule();
    bool begin();
    void leerSensores();
    float getTempPromedio();
    float getHumedad();
    float getPresion();

private:
    Adafruit_BMP085 bmp;
    DHT dht;
    float tempBMP, tempDHT, humDHT, presBMP;
};

#endif
