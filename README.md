#  Sistema Embebido de Monitoreo Meteorológico Escolar

> **Proyecto Final de Carrera — Escuela Técnica N.º 25 “Fray Luis Beltrán”**
> **6.º año — Orientación Electrónica — Ciclo lectivo 2026**

Sistema embebido diseñado para realizar mediciones meteorológicas directamente desde la terraza de la institución, procesar los datos mediante un ESP32 y transmitirlos inalámbricamente hacia una unidad de visualización ubicada dentro de la escuela.



##  Integrantes

* **Carmen Besada**
* **Michelle de Mello**
* **Agustín Seminara**
* **Lucas Olla**
* **Joaquín Lequerica**
* **Juan Ignacio García**



## Descripción

Las condiciones meteorológicas pueden variar entre distintos puntos de una misma ciudad. Los datos proporcionados por aplicaciones y servicios meteorológicos son obtenidos a partir de estaciones ubicadas en determinados puntos geográficos, por lo que pueden existir diferencias entre esos valores y las condiciones presentes en el entorno inmediato de una institución.

Este proyecto propone desarrollar una **estación meteorológica electrónica autónoma**, instalada en la terraza de la Escuela Técnica N.º 25, capaz de medir diferentes variables ambientales de la zona.

La información obtenida será procesada por un **ESP32**, que posteriormente transmitirá los datos mediante **Wi-Fi** hacia un segundo ESP32 ubicado dentro de la escuela.

El segundo dispositivo funcionará como unidad de visualización y permitirá consultar las mediciones mediante un **display OLED**.

El proyecto también contempla el análisis de las señales obtenidas, filtrado, validación de mediciones, calibración de sensores, análisis de errores y evaluación de la repetibilidad de los resultados.



##  Objetivo general

Diseñar, construir e implementar una estación meteorológica electrónica instalada en la terraza de la institución, capaz de adquirir y procesar diferentes variables ambientales, transmitir los datos mediante comunicación inalámbrica y permitir su visualización desde un dispositivo ubicado dentro de la escuela.



##  Variables meteorológicas

La primera versión del sistema contempla la medición de:

| Variable                  | Sensor / sistema previsto | Tipo de señal       |
| ------------------------- | ------------------------- | ------------------- |
|  Temperatura           | SHT31                     | I²C                 |
|  Humedad relativa       | SHT31                     | I²C                 |
|  Presión atmosférica    | BMP280                    | I²C / SPI           |
|  Radiación ultravioleta | ML8511                    | Analógica           |
|  Velocidad del viento   | Anemómetro                | Pulsos              |
|  Dirección del viento   | Veleta resistiva          | Analógica           |
|  Precipitaciones       | Sensor de lluvia          | Analógica / digital |

> Los sensores indicados corresponden a las alternativas seleccionadas durante la etapa de anteproyecto. Podrán modificarse durante el desarrollo en función de disponibilidad, ensayos, precisión, costo y resultados obtenidos.



##  Arquitectura del sistema

El sistema estará dividido en **dos unidades físicas principales**.

###  1. Estación meteorológica — Terraza

Ubicada en el exterior de la institución.

Sus funciones principales serán:

* Adquirir las mediciones de los sensores.
* Procesar las señales.
* Filtrar las mediciones.
* Validar los valores obtenidos.
* Detectar valores anómalos.
* Calcular variables meteorológicas.
* Registrar las mediciones.
* Transmitir los datos mediante Wi-Fi.

**Controlador:** ESP32.

```text
┌─────────────────────────────┐
│   ESTACIÓN METEOROLÓGICA    │
│          TERRAZA            │
└──────────────┬──────────────┘
               │
     ┌─────────▼─────────┐
     │      Sensores     │
     ├───────────────────┤
     │ Temperatura       │
     │ Humedad           │
     │ Presión           │
     │ Radiación UV      │
     │ Velocidad viento  │
     │ Dirección viento  │
     │ Precipitaciones   │
     └─────────┬─────────┘
               │
               ▼
        ┌─────────────┐
        │    ESP32    │
        │ Adquisición │
        │ Procesado   │
        │ Validación  │
        └──────┬──────┘
               │
              Wi-Fi
               │
               ▼
```

###  2. Unidad de visualización — Aula

Ubicada dentro de la institución.

Sus funciones serán:

* Recibir los datos enviados por la estación.
* Procesar y organizar la información recibida.
* Mostrar las mediciones.
* Informar errores de comunicación cuando corresponda.

**Controlador:** segundo ESP32.

**Visualización:** display OLED SSD1306.

```text
              Wi-Fi
                │
                ▼
        ┌─────────────┐
        │    ESP32    │
        │  RECEPTOR   │
        └──────┬──────┘
               │
               ▼
        ┌─────────────┐
        │ OLED SSD1306│
        │             │
        │ Temperatura │
        │ Humedad     │
        │ Presión     │
        │ UV          │
        │ Viento      │
        │ Lluvia      │
        └─────────────┘
```



##  Funcionamiento

El funcionamiento general previsto es:

```text
        INICIO
          │
          ▼
 Inicialización ESP32
          │
          ▼
 Inicialización
    de sensores
          │
          ▼
 ¿Sensores OK?
      │       │
     NO      SÍ
      │       │
      │       ▼
      │   Adquisición
      │   de datos
      │       │
      │       ▼
      │   Validación
      │       │
      │       ▼
      │     Filtrado
      │       │
      │       ▼
      │ Cálculo de variables
      │       │
      │       ▼
      │   Registro
      │       │
      │       ▼
      │ Transmisión Wi-Fi
      │       │
      │       ▼
      │  ESP32 receptor
      │       │
      │       ▼
      │   Display OLED
      │       │
      └───────┴──────►
              │
              ▼
       Esperar intervalo
              │
              ▼
       Repetir medición
```

Si se detecta un error en un sensor o una pérdida de comunicación, el sistema deberá identificar la situación y evitar utilizar o mostrar datos que puedan considerarse inválidos.



##  Procesamiento de datos

Uno de los objetivos del proyecto es que las mediciones no sean simplemente leídas y mostradas, sino que sean sometidas a un procesamiento previo.

Se contempla implementar:

* Filtrado de ruido.
* Promediado de mediciones.
* Validación de rangos.
* Detección de valores anómalos.
* Conversión a unidades meteorológicas.
* Cálculo de velocidad del viento a partir de pulsos.
* Determinación de la dirección del viento.
* Registro de mediciones a intervalos definidos.
* Análisis de error.
* Evaluación de repetibilidad.
* Evaluación de estabilidad.

El procesamiento permitirá obtener información más consistente y facilitará posteriormente el análisis de los datos registrados.



##  Detección de precipitaciones

La primera versión contempla un sensor destinado a determinar la **presencia de precipitaciones**.

Es importante diferenciar esta función de una posible predicción meteorológica:

* **Detección de lluvia:** determina si existe presencia de precipitación mediante el sensor correspondiente.
* **Estimación de precipitaciones:** intenta determinar la posibilidad de lluvia futura utilizando la evolución de distintas variables meteorológicas.

La segunda función no forma parte del funcionamiento obligatorio de la primera versión y queda planteada como una posible mejora futura.



##  Comunicación

La comunicación entre ambas unidades se realizará mediante **Wi-Fi**.

```text
┌──────────────────┐
│ ESP32 - TERRAZA  │
│                  │
│     SENSORES     │
└────────┬─────────┘
         │
         │ Wi-Fi
         │
         ▼
┌──────────────────┐
│ ESP32 - AULA     │
│                  │
│    DISPLAY OLED  │
└──────────────────┘
```

El formato de los datos transmitidos será definido durante la etapa de desarrollo.



##  Hardware

### Control y comunicación

* 2 × ESP32
* Wi-Fi integrado

### Sensores

* SHT31
* BMP280
* ML8511
* Anemómetro
* Veleta resistiva
* Sensor de lluvia

### Visualización

* Display OLED SSD1306 128×64

### Otros componentes

* Resistencias
* Capacitores
* Cables
* Conectores
* Fuente de alimentación
* PCB o placa de montaje
* Caja protectora
* Estructura y soportes para sensores



##  Software

El desarrollo contempla el uso de:

* **Arduino IDE**
* **C/C++**
* Librerías para ESP32
* Comunicación I²C
* Comunicación SPI
* ADC del ESP32
* Wi-Fi
* Python para análisis de datos
* KiCad para diseño de circuitos y PCB



##  Mediciones y ensayos

El proyecto incluye una etapa específica de caracterización y validación de los sensores.

Se realizarán pruebas para analizar:

* Error de medición.
* Precisión.
* Repetibilidad.
* Estabilidad.
* Respuesta ante diferentes condiciones.
* Ruido en las señales.
* Comportamiento de los sensores durante períodos prolongados.

Cuando sea posible, las mediciones obtenidas serán comparadas con valores de referencia para determinar el comportamiento de cada sensor.



##  Registro y análisis

Las mediciones obtenidas podrán registrarse para estudiar su evolución a lo largo del tiempo.

Esto permitirá posteriormente:

* Generar gráficos.
* Comparar diferentes períodos.
* Analizar variaciones meteorológicas.
* Estudiar relaciones entre distintas variables.
* Evaluar el comportamiento de los sensores.
* Utilizar los datos como base para futuras mejoras del sistema.



##  Organización del proyecto

La estructura del repositorio se irá organizando a medida que avance el desarrollo.

Una estructura prevista es:

```text
Estacion-Meteorologica/
│
├── README.md
│
├── firmware/
│   ├── estacion/
│   └── receptor/
│
├── sensores/
│   ├── temperatura_humedad/
│   ├── presion/
│   ├── uv/
│   ├── viento/
│   └── lluvia/
│
├── comunicacion/
│
├── procesamiento/
│
├── diagramas/
│
├── pcb/
│
├── documentacion/
│
└── pruebas/
```

> La estructura podrá modificarse durante el desarrollo según las necesidades del proyecto.



## Distribución de tareas

### Carmen Besada — Sistema embebido y firmware

* Estructura general del programa.
* Configuración del ESP32.
* Adquisición de datos.
* Integración de librerías.
* Temporización.
* Integración de sensores.
* Optimización y corrección del firmware.

### Michelle de Mello — Temperatura, humedad y presión

* Investigación del SHT31 y BMP280.
* Pruebas individuales.
* Registro de mediciones.
* Comparación con referencias.
* Calibración.
* Análisis de errores.

### Agustín Seminara — Velocidad y dirección del viento

* Investigación del anemómetro.
* Lectura de pulsos.
* Cálculo de velocidad.
* Desarrollo de la veleta.
* Determinación de direcciones.
* Pruebas y calibración.

### Lucas Olla — Radiación UV y precipitaciones

* Investigación del ML8511.
* Lectura mediante ADC.
* Pruebas de radiación UV.
* Implementación del sensor de lluvia.
* Definición del criterio de detección.
* Calibración y análisis.

### Joaquín Lequerica — Procesamiento y validación

* Definición de rangos.
* Análisis del ruido.
* Filtrado digital.
* Validación de mediciones.
* Detección de valores anómalos.
* Análisis de errores.
* Organización de datos.

### Juan Ignacio García — Comunicación y visualización

* Configuración del segundo ESP32.
* Comunicación Wi-Fi.
* Formato de datos.
* Recepción.
* Comunicación con el OLED.
* Diseño de la interfaz.
* Gestión de errores de comunicación.



## 📋 Alcances obligatorios

La primera versión del proyecto deberá permitir:

* [ ] Medir temperatura.
* [ ] Medir humedad relativa.
* [ ] Medir presión atmosférica.
* [ ] Medir radiación ultravioleta.
* [ ] Medir velocidad del viento.
* [ ] Determinar la dirección del viento.
* [ ] Detectar presencia de precipitaciones.
* [ ] Procesar y validar las mediciones.
* [ ] Transmitir los datos mediante Wi-Fi.
* [ ] Recibir los datos mediante un segundo ESP32.
* [ ] Visualizar los datos mediante un display OLED.
* [ ] Realizar pruebas y calibración.
* [ ] Instalar la estación en la terraza.
* [ ] Validar el funcionamiento del sistema completo.



##  Mejoras futuras

Una vez alcanzado el funcionamiento básico, se podrán incorporar nuevas funcionalidades:

###  Interfaz web

Desarrollo de una página web para consultar las mediciones desde computadoras o teléfonos conectados a la red.

###  Históricos y gráficos

Almacenamiento de las mediciones y generación de gráficos para analizar la evolución de las variables meteorológicas.

###  Acceso desde celulares

Desarrollo de una interfaz adaptada a dispositivos móviles.

###  Alertas

Notificaciones ante determinadas condiciones meteorológicas.

### ☀️ Alimentación autónoma

Incorporación de:

* Panel solar.
* Batería.
* Sistema de regulación y carga.

Esto permitiría que la estación funcione de forma independiente de la alimentación eléctrica convencional.

###  Estimación de precipitaciones

Si se logra obtener una cantidad suficiente de datos reales, se podrá estudiar la utilización de técnicas de análisis de datos o aprendizaje automático para estimar la posibilidad de precipitaciones a corto plazo.

Esta función será independiente del sistema de detección directa de lluvia.

###  Uso institucional

Una posible evolución del proyecto consiste en desarrollar una página accesible para los alumnos y docentes de la institución, permitiendo consultar las condiciones meteorológicas actuales y utilizar la información como referencia para actividades escolares, por ejemplo, actividades de Educación Física.



##  Plan de desarrollo

El proyecto se divide en cinco etapas principales:

### 1. Investigación técnica

* Estudio de sensores.
* Hojas de datos.
* Interfaces.
* Métodos de medición.
* Definición de pruebas.

### 2. Desarrollo de subsistemas

* Firmware.
* Sensores.
* Procesamiento.
* Comunicación.
* Segundo ESP32.
* Display.

### 3. Integración y calibración

* Integración progresiva.
* Pruebas de comunicación.
* Validación.
* Calibración.
* Corrección de errores.

### 4. Construcción e instalación

* Estructura.
* Soportes.
* Protección.
* Cableado.
* Montaje.
* Instalación en terraza.

### 5. Validación y presentación

* Pruebas prolongadas.
* Registro de datos.
* Análisis.
* Calibración final.
* Documentación.
* Presentación del proyecto.



##  Costo estimado

Los valores son aproximados y pueden variar según el proveedor, disponibilidad y modelo de cada componente.

| Componente             |               Costo estimado |
| ---------------------- | ---------------------------: |
| ESP32 ×2               |                      $12.000 |
| SHT31                  |                      $11.000 |
| BMP280                 |                       $4.000 |
| ML8511                 |              $20.000–$30.000 |
| Anemómetro             |              $20.000–$50.000 |
| Veleta                 |              $10.000–$25.000 |
| Sensor de lluvia YL-83 |                $2.500–$5.000 |
| Display                |                      $25.000 |
| **Estimación total**   | **$190.000–$250.000 aprox.** |

> Los costos deberán actualizarse al momento de realizar la compra. También se deberá considerar el material disponible previamente en la institución.



##  Contexto académico

Este proyecto integra conocimientos correspondientes a distintas áreas de la orientación Electrónica:

* **Sistemas Electrónicos Embebidos**
* **Procesamiento Digital de Imagen y Sonido**
* **Laboratorio de Mediciones y Ensayos III**

Durante el desarrollo se aplicarán conocimientos relacionados con:

* Microcontroladores.
* Sensores.
* Señales analógicas y digitales.
* ADC.
* I²C.
* SPI.
* Comunicación inalámbrica.
* Programación.
* Filtrado digital.
* Calibración.
* Mediciones.
* Análisis de errores.
* Adquisición y procesamiento de datos.



##  Estado del proyecto

**En desarrollo — Proyecto Final de Carrera 2026**

Actualmente el proyecto se encuentra en etapa de desarrollo y pruebas de los distintos subsistemas.

Los componentes, circuitos, algoritmos y métodos de procesamiento podrán modificarse durante el desarrollo en función de los resultados obtenidos en los ensayos.

La documentación de este repositorio se actualizará a medida que avance el proyecto.



##  Licencia

Este proyecto fue desarrollado con fines **educativos y académicos** como parte del Proyecto Final de Carrera de la Escuela Técnica N.º 25 “Fray Luis Beltrán”.



##  Institución

**Escuela Técnica N.º 25 “Fray Luis Beltrán”**
**6.º año — Orientación Electrónica**
**Ciclo lectivo 2026**



<p align="center">
  <b> Sistema Embebido de Monitoreo Meteorológico Escolar</b><br>
  Proyecto Final de Carrera · Electrónica · 2026
</p>
