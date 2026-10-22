#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

unsigned long tiempoAnterior = 0;
const unsigned long intervalo = 2000;

void setup()
{
    Serial.begin(115200);

    Wire.begin(D2, D1);

    Serial.println("Iniciando BME280...");

    if (bme.begin(0x76))
    {
        Serial.println("BME280 encontrado en 0x76");
    }
    else if (bme.begin(0x77))
    {
        Serial.println("BME280 encontrado en 0x77");
    }
    else
    {
        Serial.println("No se encontro el BME280");
    }
}

void loop()
{
    unsigned long tiempoActual = millis();

    if (tiempoActual - tiempoAnterior >= intervalo)
    {
        tiempoAnterior = tiempoActual;

        Serial.print("Temperatura: ");
        Serial.print(bme.readTemperature());
        Serial.println(" C");

        Serial.print("Humedad: ");
        Serial.print(bme.readHumidity());
        Serial.println(" %");

        Serial.print("Presion: ");
        Serial.print(bme.readPressure() / 100.0F);
        Serial.println(" hPa");

        Serial.println("-------------------");
    }
}