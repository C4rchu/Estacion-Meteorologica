#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SPI.h>
#include <TFT_eSPI.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;
TFT_eSPI tft = TFT_eSPI();

// Control de páginas: 0 = Datos+Confort, 1 = Temp y Humedad, 2 = Presión y Pronóstico
uint8_t paginaActual = 0;
const uint8_t TOTAL_PAGINAS = 3;

// Historial de datos (240 puntos de muestreo)
const int MAX_PUNTOS = 240;
float historialTemp[MAX_PUNTOS];
float historialHum[MAX_PUNTOS];
float historialPres[MAX_PUNTOS];
int indiceHistorial = 0;

// Control de tiempo sin delay
unsigned long tiempoAnteriorLectura = 0;
const unsigned long intervaloLectura = 1000; // 1 segundo

unsigned long tiempoAnteriorTouch = 0;
const unsigned long intervaloTouch = 200; // Anti-rebote del touch

// Declaración de funciones
void dibujarPagina0_Datos(float temp, float hum, float pres);
void dibujarPagina1_GraficoTempHum();
void dibujarPagina2_GraficoPresion(float presActual);
void agregarPuntosHistorial(float temp, float hum, float pres);
void dibujarBarraConfort(float temp, float hum);

void setup() {
  Serial.begin(115200);

  // Inicialización I2C (SDA: G21, SCL: G22)
  Wire.begin(21, 22);

  // Inicialización de la Pantalla
  tft.init();
  tft.setRotation(0); // Orientación vertical 320x480
  tft.fillScreen(TFT_BLACK);

  // Calibración táctil
  uint16_t calData[5] = { 275, 3620, 264, 3532, 1 };
  tft.setTouch(calData);

  // Inicialización del BME280
  if (!bme.begin(0x76, &Wire)) {
    Serial.println(F("Error: BME280 no encontrado"));
    while (1);
  }

  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1,
                  Adafruit_BME280::SAMPLING_X1,
                  Adafruit_BME280::SAMPLING_X1,
                  Adafruit_BME280::FILTER_OFF);

  // Inicializar arrays en cero
  for (int i = 0; i < MAX_PUNTOS; i++) {
    historialTemp[i] = 0;
    historialHum[i] = 0;
    historialPres[i] = 0;
  }
}

void loop() {
  unsigned long tiempoActual = millis();

  // --- LECTURA DEL TÁCTIL (Cambio de Página) ---
  if (tiempoActual - tiempoAnteriorTouch >= intervaloTouch) {
    uint16_t touchX = 0, touchY = 0;
    bool presionado = tft.getTouch(&touchX, &touchY);

    if (presionado) {
      tiempoAnteriorTouch = tiempoActual;
      
      // Avanzar a la siguiente página de forma circular (0 -> 1 -> 2 -> 0)
      paginaActual = (paginaActual + 1) % TOTAL_PAGINAS;
      tft.fillScreen(TFT_BLACK);

      bme.takeForcedMeasurement();
      float temp = bme.readTemperature();
      float hum = bme.readHumidity();
      float pres = bme.readPressure() / 100.0F;

      if (paginaActual == 0)      dibujarPagina0_Datos(temp, hum, pres);
      else if (paginaActual == 1) dibujarPagina1_GraficoTempHum();
      else if (paginaActual == 2) dibujarPagina2_GraficoPresion(pres);
    }
  }

  // --- LECTURA PERIÓDICA DEL SENSOR (1 vez por segundo) ---
  if (tiempoActual - tiempoAnteriorLectura >= intervaloLectura) {
    tiempoAnteriorLectura = tiempoActual;

    bme.takeForcedMeasurement();
    float temp = bme.readTemperature();
    float hum = bme.readHumidity();
    float pres = bme.readPressure() / 100.0F;

    // Guardar lecturas en el historial
    agregarPuntosHistorial(temp, hum, pres);

    // Refrescar sólo la página que está visible
    if (paginaActual == 0)      dibujarPagina0_Datos(temp, hum, pres);
    else if (paginaActual == 1) dibujarPagina1_GraficoTempHum();
    else if (paginaActual == 2) dibujarPagina2_GraficoPresion(pres);
  }
}

// PÁGINA 0: DATOS NUMÉRICOS Y INDICADOR DE CONFORT
void dibujarPagina0_Datos(float temp, float hum, float pres) {
  // Encabezado
  tft.fillRect(0, 0, 320, 40, TFT_NAVY);
  tft.setTextColor(TFT_WHITE, TFT_NAVY);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("PAGINA 1/3: DATOS [Toca ->]", 160, 20, 4);

  // Temperatura
  tft.drawRoundRect(10, 50, 300, 85, 8, TFT_DARKCYAN);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.drawString("TEMPERATURA", 160, 65, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(String(temp, 1) + " C", 160, 100, 6);

  // Humedad
  tft.drawRoundRect(10, 145, 300, 85, 8, TFT_DARKCYAN);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString("HUMEDAD RELATIVA", 160, 160, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(String(hum, 1) + " %", 160, 195, 6);

  // Presión
  tft.drawRoundRect(10, 240, 300, 85, 8, TFT_DARKCYAN);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("PRESION ATMOSFERICA", 160, 255, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(String(pres, 1) + " hPa", 160, 290, 6);

  // Barra de Confort Térmico
  dibujarBarraConfort(temp, hum);

  tft.setTextColor(TFT_SILVER, TFT_BLACK);
  tft.drawString("Toca la pantalla para cambiar de vista", 160, 450, 2);
}

// PÁGINA 1: GRÁFICO COMBINADO (TEMPERATURA Y HUMEDAD)
void dibujarPagina1_GraficoTempHum() {
  tft.fillRect(0, 0, 320, 40, TFT_DARKGREEN);
  tft.setTextColor(TFT_WHITE, TFT_DARKGREEN);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("PAGINA 2/3: TEMP & HUM", 160, 20, 4);

  int xOrigen = 35;
  int yOrigen = 380;
  int ancho = 260;
  int alto = 280;

  // Marco
  tft.drawRect(xOrigen, yOrigen - alto, ancho, alto, TFT_WHITE);

  // Cuadrícula horizontal
  for (int i = 0; i <= 4; i++) {
    int y = yOrigen - (i * (alto / 4));
    tft.drawFastHLine(xOrigen, y, ancho, TFT_DARKGREY);
  }

  // Leyenda de Colores
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.drawString("Temp (C)", 90, 60, 2);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString("Hum (%)", 230, 60, 2);

  // Curva de datos
  for (int i = 0; i < MAX_PUNTOS - 1; i++) {
    if (historialTemp[i] > 0 && historialTemp[i + 1] > 0) {
      int x1 = xOrigen + i;
      int x2 = xOrigen + i + 1;

      // Temperatura: Mapeo de 0°C a 50°C
      int yT1 = map((int)(historialTemp[i] * 10), 0, 500, yOrigen, yOrigen - alto);
      int yT2 = map((int)(historialTemp[i + 1] * 10), 0, 500, yOrigen, yOrigen - alto);
      tft.drawLine(x1, yT1, x2, yT2, TFT_ORANGE);

      // Humedad: Mapeo de 0% a 100%
      int yH1 = map((int)(historialHum[i] * 10), 0, 1000, yOrigen, yOrigen - alto);
      int yH2 = map((int)(historialHum[i + 1] * 10), 0, 1000, yOrigen, yOrigen - alto);
      tft.drawLine(x1, yH1, x2, yH2, TFT_CYAN);
    }
  }

  tft.setTextColor(TFT_SILVER, TFT_BLACK);
  tft.drawString("Toca para ir a Presion Barometrica", 160, 440, 2);
}

// PÁGINA 2: GRÁFICO DE PRESIÓN & PRONÓSTICO
void dibujarPagina2_GraficoPresion(float presActual) {
  tft.fillRect(0, 0, 320, 40, TFT_MAGENTA);
  tft.setTextColor(TFT_WHITE, TFT_MAGENTA);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("PAGINA 3/3: PRESION", 160, 20, 4);

  int xOrigen = 40;
  int yOrigen = 340;
  int ancho = 250;
  int alto = 220;

  tft.drawRect(xOrigen, yOrigen - alto, ancho, alto, TFT_WHITE);

  // Cuadrícula
  for (int i = 0; i <= 4; i++) {
    int y = yOrigen - (i * (alto / 4));
    tft.drawFastHLine(xOrigen, y, ancho, TFT_DARKGREY);
  }

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("TENDENCIA BAROMETRICA (hPa)", 160, 60, 2);

  // Curva de Presión (Mapeo de 950 a 1050 hPa)
  for (int i = 0; i < MAX_PUNTOS - 1; i++) {
    if (historialPres[i] > 0 && historialPres[i + 1] > 0) {
      int x1 = xOrigen + i;
      int x2 = xOrigen + i + 1;

      int yP1 = map((int)(historialPres[i] * 10), 9500, 10500, yOrigen, yOrigen - alto);
      int yP2 = map((int)(historialPres[i + 1] * 10), 9500, 10500, yOrigen, yOrigen - alto);
      tft.drawLine(x1, yP1, x2, yP2, TFT_YELLOW);
    }
  }

  // Cuadro de Pronóstico Estimado
  tft.drawRoundRect(20, 360, 280, 65, 6, TFT_GOLD);
  tft.setTextColor(TFT_GOLD, TFT_BLACK);
  tft.drawString("ESTADO ESTIMADO:", 160, 375, 2);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  if (presActual > 1015.0) {
    tft.drawString("TIEMPO ESTABLE / SOLEADO", 160, 400, 2);
  } else if (presActual >= 1005.0 && presActual <= 1015.0) {
    tft.drawString("CLIMA VARIABLE", 160, 400, 2);
  } else {
    tft.drawString("BAJA PRESION / POSIBLE LLUVIA", 160, 400, 2);
  }

  tft.setTextColor(TFT_SILVER, TFT_BLACK);
  tft.drawString("Toca para volver a Datos Principales", 160, 450, 2);
}

// FUNCIONES AUXILIARES
void agregarPuntosHistorial(float temp, float hum, float pres) {
  if (indiceHistorial < MAX_PUNTOS) {
    historialTemp[indiceHistorial] = temp;
    historialHum[indiceHistorial] = hum;
    historialPres[indiceHistorial] = pres;
    indiceHistorial++;
  } else {
    for (int i = 0; i < MAX_PUNTOS - 1; i++) {
      historialTemp[i] = historialTemp[i + 1];
      historialHum[i] = historialHum[i + 1];
      historialPres[i] = historialPres[i + 1];
    }
    historialTemp[MAX_PUNTOS - 1] = temp;
    historialHum[MAX_PUNTOS - 1] = hum;
    historialPres[MAX_PUNTOS - 1] = pres;
  }
}

void dibujarBarraConfort(float temp, float hum) {
  tft.drawRoundRect(10, 335, 300, 80, 8, TFT_DARKGREY);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("CONFORT THERMAL", 160, 350, 2);

  // Barra de rangos de colores
  tft.fillRect(30, 380, 70, 12, TFT_BLUE);     // Frío
  tft.fillRect(100, 380, 120, 12, TFT_GREEN);  // Confortable
  tft.fillRect(220, 380, 70, 12, TFT_RED);     // Calor

  // Mapear posición del indicador según temperatura (10°C a 40°C)
  int posX = map((int)temp, 10, 40, 30, 290);
  posX = constrain(posX, 30, 290);

  // Indicador
  tft.fillTriangle(posX - 5, 373, posX + 5, 373, posX, 379, TFT_WHITE);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  if (temp < 18.0) {
    tft.drawString("AMB. FRIO", 160, 402, 2);
  } else if (temp >= 18.0 && temp <= 26.0) {
    tft.drawString("AMB. IDEAL", 160, 402, 2);
  } else {
    tft.drawString("AMB. CALUROSO", 160, 402, 2);
  }
}
