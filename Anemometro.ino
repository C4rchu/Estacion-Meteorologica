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
  if (sensorValue >= 2000 && sensorValue <= 3800) return "Sur (S)";  
  if (sensorValue >= 1000 && sensorValue < 2000)  return "Oeste (W)";  
  if (sensorValue >= 400  && sensorValue < 1000)  return "Norte (N)";
  if (sensorValue < 400)                          return "Este (E)";
  
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

  // Pantalla de presentación inicial
  tft.drawRect(10, 10, 460, 300, ST77XX_RED); // Marco exterior
  
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.setCursor(40, 40);
  tft.println("ESTACION METEOROLOGICA");

  tft.setTextSize(2);
  tft.setCursor(40, 110);
  tft.println("Iniciando sistema...");

  lastTimeStartup = millis();
  lastTimeSpeed = millis();
  lastTimeLCD = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. Pantalla de presentación inicial de 2 segundos sin delay
  if (startupPhase) {
    if (currentMillis - lastTimeStartup >= 2000) {
      // Limpiamos la zona de la presentación para dejar la interfaz fija
      tft.fillRect(30, 100, 400, 40, ST77XX_BLACK);
      
      tft.setTextSize(2);
      tft.setTextColor(ST77XX_WHITE);
      
      tft.setCursor(40, 100);
      tft.println("Velocidad del Viento:");

      tft.setCursor(40, 200);
      tft.println("Direccion del Viento:");
      
      startupPhase = false;
    }
    return;  
  }

  // 2. Leemos la veleta constantemente de forma analógica (los 2 cables que vienen de los 4 reed switch)
  int rawVeleta = analogRead(veletaPin);
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
    tft.fillRect(40, 130, 300, 50, ST77XX_BLACK);
    tft.setTextColor(ST77XX_GREEN);
    tft.setTextSize(4);
    tft.setCursor(40, 135);
    tft.print(windSpeed, 1); // Con 1 decimal

    tft.setTextSize(2);
    tft.setCursor(210, 155);
    tft.setTextColor(ST77XX_WHITE);
    tft.print("km/h");

    // --- Renderizar Dirección (de los 4 reed switch) ---
    tft.fillRect(40, 230, 380, 50, ST77XX_BLACK);
    tft.setTextColor(ST77XX_CYAN);
    tft.setTextSize(4);
    tft.setCursor(40, 235);
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