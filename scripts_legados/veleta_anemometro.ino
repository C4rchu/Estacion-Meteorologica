#include <Adafruit_GFX.h>
#include <Adafruit_ST7796S.h>
#include <SPI.h>

// Definición de pines para la pantalla ST7796S en el ESP32
#define TFT_CS   15  
#define TFT_DC    2  
#define TFT_RST   4  

Adafruit_ST7796S tft = Adafruit_ST7796S(TFT_CS, TFT_DC, TFT_RST);

// Configuración de pines en el ESP32
const int anemometerPin = 16;  // Pin conectado al anemómetro
const int veletaPin = 26;     // Pin analógico (VP / GPIO 36) para los 2 cables de la veleta de 4 reed switch

volatile unsigned int pulseCount = 0;
volatile unsigned long lastInterruptTime = 0;

unsigned long lastTimeSpeed = 0;
unsigned long lastTimeLCD = 0;
unsigned long lastTimeStartup = 0;
bool startupPhase = true;

float windSpeed = 0;
String currentDir = "---";

// Función de interrupción optimizada con el antirrebote original de 15ms
void IRAM_ATTR countPulse() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > 15) {
    pulseCount++;
    lastInterruptTime = interruptTime;
  }
}

//Esta función transforma el valor analógico que entrega la veleta en una dirección cardinal.
String getWindDirection(int sensorValue) {
  // Ajuste más amplio para mayor tolerancia al ruido:
  // - Este (E):  0 - 400
  // - Norte (N): 500 - 1500
  // - Sur (S):   2000 - 3000
  
  if (sensorValue >= 2000 && sensorValue <= 3000) return "Sur (S)";
  if (sensorValue >= 500  && sensorValue < 1500)  return "Norte (N)";
  if (sensorValue >= 0    && sensorValue < 400)   return "Este (E)";
  
  // El valor 4095 indica que el circuito está abierto (no hay contacto reed switch)
  return "---";
}

void setup() {
  Serial.begin(115200);

  // Configurar pin del anemómetro
  pinMode(anemometerPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(anemometerPin), countPulse, FALLING);

  // Inicializar la pantalla ST7796S
  tft.init(320, 480);
  tft.setRotation(1); // Orientación horizontal (Landscape)
  tft.fillScreen(ST77XX_BLACK);

  // --- Dibujar los 2 cuadrados con bordes redondeados ---
  // Cuadrado 1 (Velocidad)
  tft.fillRoundRect(20, 60, 210, 200, 20, ST77XX_RED);
  tft.drawRoundRect(20, 60, 210, 200, 20, ST77XX_WHITE);
  
  // Cuadrado 2 (Dirección)
  tft.fillRoundRect(250, 60, 210, 200, 20, ST77XX_RED);
  tft.drawRoundRect(250, 60, 210, 200, 20, ST77XX_WHITE);
  
  // Etiquetas fijas
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(50, 75);
  tft.println("VELOCIDAD");
  
  tft.setCursor(280, 75);
  tft.println("DIRECCION");

  lastTimeStartup = millis();
  lastTimeSpeed = millis();
  lastTimeLCD = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. Pantalla de presentación (simplificada)
  if (startupPhase) {
    if (currentMillis - lastTimeStartup >= 2000) {
      startupPhase = false;
    }
    return;  
  }

  // 2. Leemos la veleta constantemente de forma analógica con promedio para suavizar ruido
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(veletaPin);
    delay(2); // Pequeña pausa para estabilizar ADC
  }
  int rawVeleta = sum / 10;
  
  // Imprimir valor para calibración (Comenta esta línea después de calibrar para mejor rendimiento)
  Serial.println("Valor ADC actual: " + String(rawVeleta));
  
  currentDir = getWindDirection(rawVeleta);

  // 3. Cálculo de velocidad del anemómetro cada 1 segundo (con la lógica original exacta)
  if (currentMillis - lastTimeSpeed >= 1000) {
    detachInterrupt(digitalPinToInterrupt(anemometerPin));
    
    unsigned int pulses = pulseCount;
    pulseCount = 0;
    lastTimeSpeed = currentMillis;
    
    attachInterrupt(digitalPinToInterrupt(anemometerPin), countPulse, FALLING);

    // Fórmula original que andaba bien
    float revs = (float)pulses / 3.0;  
    windSpeed = revs * 2.4; 

    // Control por monitor serie
    Serial.print("Pulsos: ");
    Serial.print(pulses);
    Serial.print(" | Velocidad: ");
    Serial.print(windSpeed);
    Serial.print(" km/h | Veleta ADC: ");
    Serial.print(rawVeleta);
    Serial.print(" | Dir: ");
    Serial.println(currentDir);
  }

  // 4. Actualiza el display TFT cada 500 ms de forma fluida
  if (currentMillis - lastTimeLCD >= 500) {
    lastTimeLCD = currentMillis;

    // --- Renderizar Velocidad ---
    // Área roja para limpiar texto anterior
    tft.fillRect(40, 110, 170, 100, ST77XX_RED);
    
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(4);
    tft.setCursor(50, 120);
    tft.print(windSpeed, 1); // Con 1 decimal

    tft.setTextSize(2);
    tft.setCursor(50, 170);
    tft.print("km/h");

    // --- Renderizar Dirección ---
    // Área roja para limpiar texto anterior
    tft.fillRect(270, 110, 170, 100, ST77XX_RED);
    
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(3);
    tft.setCursor(280, 130);
    tft.print(currentDir);
  }
}


/* 
 * Notas de la modificación (Anemometro):
 * - Al final puse a la fórmula que tenia cuando usaba la pantalla LCD 16x2 I2C para 
 *   las revoluciones (pulses / 3.0 * 2.4). 
 * - Le metimos el detachInterrupt y attachInterrupt cada 1 segundo 
 *   cuando leemos los pulsos, así el micro no se marea ni corrompe 
 *   la variable a mitad de camino.
 * - Para que la pantalla ST7796S no titile toda a cada rato (flickering), 
 *   en vez de borrar todo el display tiramos un fillRect chiquito 
 *   únicamente en la zona del número.
 * - Quedó conectado al pin 5 del ESP32 con el rebote en 15ms.
 * - Agregamos la lectura de la veleta (los 2 cables que vienen de los 4 reed switch) 
 *   al pin analógico 36 (VP) adaptando los rangos al ADC del ESP32.
 */

 /* 
 detachInterrupt(): Lo usamos para frenar la cuenta mientras el micro hace las cuentas matemáticas, así no cambia los datos a mitad de camino.
 attachInterrupt(): Es volver a prender el ESP para que no se pierda ningún pulso nuevo.
 */