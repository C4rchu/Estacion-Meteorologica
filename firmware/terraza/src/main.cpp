#include <Arduino.h>
#include "BME_DHT_Module.h"
#include "Wind_Module.h"
#include "Rain_Module.h"
#include "ESPNOW_Sender.h"
#include "DataStructure.h"

ClimaModule clima;
WindModule viento;
RainModule lluvia;
ESPNOWSender sender;

void setup() {
    Serial.begin(115200);
    clima.begin();
    viento.begin();
    lluvia.begin();
    sender.begin();
    
    // Configurar ADC para sensor UV
    pinMode(PIN_SENSOR_UV, INPUT);
    analogSetAttenuation(ADC_11db);
}

void loop() {
    WeatherData data;
    
    clima.leerSensores();
    viento.calcularViento();
    
    data.tempPromedio = clima.getTempPromedio();
    data.humedad = clima.getHumedad();
    data.presion = clima.getPresion();
    data.velocidadViento = viento.getVelocidad();
    
    String dir = viento.getDireccion();
    strncpy(data.direccionViento, dir.c_str(), sizeof(data.direccionViento));
    
    data.uv = analogRead(PIN_SENSOR_UV) * (3.3 / 4095.0); // Simple conversion, might need calibration
    data.llueve = lluvia.estaLloviendo();
    data.lluviaAcumulada = lluvia.getLluviaAcumulada();
    
    sender.sendData(data);
    
    delay(2000);
}
