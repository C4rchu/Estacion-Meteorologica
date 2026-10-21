                                                      //CODIGO UV 
const int uvPin = 34; // Pin VP es el GPIO 36 en ESP32

unsigned long previousMillis = 0;
const long interval = 200; // Lee cada 200 milisegundos

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    int sensorValue = analogRead(uvPin); // Lee el pin VP (GPIO 36)
    Serial.print("Valor ADC (0-4095): ");
    Serial.println(sensorValue);
  }
}



