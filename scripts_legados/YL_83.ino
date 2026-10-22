// Definición de pines
const int pinDO = 14;  // Pin Digital (Umbral / Sí o No)
const int pinAO = 35;  // Pin Analógico (Cantidad / Intensidad en ADC1)

void setup() {
  // Inicializamos la comunicación serie a 115200 baudios
  Serial.begin(115200);
  
  // Configuramos el pin digital como entrada
  pinMode(pinDO, INPUT);
  
  // Configuramos la atenuación del ADC del ESP32 para leer de 0 a ~3.3V
  analogSetAttenuation(ADC_11db);
  
  Serial.println("--- Sistema de Monitoreo: Sensor de Lluvia YL-83 ---");
}

void loop() {
  // 1. Leemos el pin digital (0 o 1 / LOW o HIGH)
  int estadoDigital = digitalRead(pinDO);
  
  // 2. Leemos el pin analógico (0 a 4095 en el ESP32)
  int valorAnalogico = analogRead(pinAO);
  
  // 3. Mostramos los datos por el Monitor Serie
  Serial.print("Digital (DO - Umbral): ");
  if (estadoDigital == LOW) {
    Serial.print("AGUA DETECTADA (0)");
  } else {
    Serial.print("SECO (1)");
  }
  
  Serial.print(" | Analógico (AO - Intensidad): ");
  Serial.print(valorAnalogico);
  
  // Pequeña referencia visual de la cantidad (a menor valor, más agua)
  if (valorAnalogico > 3500) {
    Serial.println(" -> Superficie seca");
  } else if (valorAnalogico > 2000) {
    Serial.println(" -> Humedad / Llovizna leve");
  } else {
    Serial.println(" -> Lluvia intensa");
  }

  delay(1000); // Pausa de 1 segundo entre cada lectura
}