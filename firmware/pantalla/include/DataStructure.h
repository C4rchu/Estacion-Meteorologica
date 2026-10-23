#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

struct WeatherData {
    float tempPromedio;
    float humedad;
    float presion;
    float velocidadViento;
    char direccionViento[10];
    float uv;
    bool llueve; // Del YL83
};

#endif
