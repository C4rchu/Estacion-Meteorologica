#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

struct __attribute__((packed)) WeatherData {
    float tempPromedio;
    float humedad;
    float presion;
    float velocidadViento;
    char direccionViento[10];
    float uv;
    bool llueve; // Del YL83
    float lluviaAcumulada;
};

#endif
