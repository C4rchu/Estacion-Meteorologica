#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP085.h> 

Adafruit_BMP085 bmp;

// Control de muestreo (cada 2 segundos)
unsigned long tiempoAnteriorLectura = 0;
const unsigned long intervaloLectura = 2000; 

void inicializarSENSOR();
void leerYSacarPorPuertoSerie();

void setup() {
  Serial.begin(115200);
  delay(500);

  // Inicialización del bus I2C (SDA = GPIO 21, SCL = GPIO 22)
  Wire.begin(21, 22);

  inicializarSENSOR();

  Serial.println("\n--- LECTURA DIRECTA BMP180 ---");
}

void loop() {
  unsigned long tiempoActual = millis();

  // Muestreo periodico 
  if (tiempoActual - tiempoAnteriorLectura >= intervaloLectura) {
    tiempoAnteriorLectura = tiempoActual;
    leerYSacarPorPuertoSerie();
  }
}

void inicializarSENSOR() {
  if (!bmp.begin()) {
    Serial.println("¡ERROR: No se detecta el sensor BMP180!");
    Serial.println("Revisá las conexiones: VCC (3V3), GND, SDA (GPIO 21) y SCL (GPIO 22).");
    while (1);
  }

  Serial.println("-> Sensor BMP180 detectado e inicializado correctamente.");
}

void leerYSacarPorPuertoSerie() {
  float temp = bmp.readTemperature();
  float pres = bmp.readPressure() / 100.0F; // Conversión de Pa a hPa

  Serial.print("Temperatura: ");
  Serial.print(temp, 2);
  Serial.print(" °C | Presion: ");
  Serial.print(pres, 2);
  Serial.println(" hPa");
}
