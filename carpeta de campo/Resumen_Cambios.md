# Resumen de Cambios

**Fecha:** 15 de octubre de 2026
**Autor:** Carmen Besada

**Descripción:**
Se corrigió la configuración de la pantalla TFT en `firmware/pantalla/src/main.cpp`.
Se reemplazó la librería `TFT_eSPI` por `Adafruit_ST7796S` y se ajustó la inicialización para asegurar que la pantalla funcione correctamente, utilizando como referencia el código probado en `Anemometro.ino`.
