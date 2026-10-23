#include <Adafruit_Sensor.h>
#include <DHT.h>

// Definición de pines y tipo de sensor
#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// Variables para control de tiempo sin delay
unsigned long tiempoAnterior = 0;
const long intervalo = 2000; // El DHT22 toma lecturas estable cada 2 segundos (2000 ms)

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println("Iniciando lectura de DHT22 en ESP32...");
}

void loop() {
  unsigned long tiempoActual = millis();

  // Comprueba si ya transcurrió el intervalo de 2 segundos
  if (tiempoActual - tiempoAnterior >= intervalo) {
    tiempoAnterior = tiempoActual;

    float humedad = dht.readHumidity();
    float temperatura = dht.readTemperature(); // En grados Celsius

    // Verificar si la lectura falló
    if (isnan(humedad) || isnan(temperatura)) {
      Serial.println("Error al leer el sensor DHT22. Revisa la conexión.");
    } else {
      Serial.print("Humedad: ");
      Serial.print(humedad);
      Serial.print("%  |  Temperatura: ");
      Serial.print(temperatura);
      Serial.println(" °C");
    }
  }


}
