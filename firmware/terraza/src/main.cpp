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
    memset(data.direccionViento, 0, sizeof(data.direccionViento));
    strncpy(data.direccionViento, dir.c_str(), sizeof(data.direccionViento) - 1);
    
    data.uv = analogRead(PIN_SENSOR_UV); 
    data.llueve = lluvia.estaLloviendo();
    
    // Solo resetear si ha pasado un tiempo prudencial sin lluvia, o usar un contador
    static unsigned long lastRainTime = 0;
    if (data.llueve) {
        lastRainTime = millis();
    } else if (millis() - lastRainTime > 10000) { // Esperar 10 segundos sin lluvia para resetear
        lluvia.resetLluvia();
    }
    
    data.lluviaAcumulada = lluvia.getLluviaAcumulada();
    
    sender.sendData(data);
    
    // Debug: imprimir lo que enviamos
    Serial.print("Enviando -> T: "); Serial.print(data.tempPromedio);
    Serial.print(" | H: "); Serial.print(data.humedad);
    Serial.print(" | V: "); Serial.print(data.velocidadViento);
    Serial.print(" | Direccion: "); Serial.print(dir);
    Serial.print(" [N/E/S/O=");
    Serial.print(digitalRead(PIN_VELETA_NORTE)); Serial.print("/");
    Serial.print(digitalRead(PIN_VELETA_ESTE)); Serial.print("/");
    Serial.print(digitalRead(PIN_VELETA_SUR)); Serial.print("/");
    Serial.print(digitalRead(PIN_VELETA_OESTE)); Serial.print("]");
    Serial.print(" | Lluvia: " + String(data.lluviaAcumulada));
    Serial.println(" | UV: " + String(data.uv));
    
    delay(2000);
}
