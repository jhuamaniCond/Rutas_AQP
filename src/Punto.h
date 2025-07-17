
// === src/Punto.h ===
#pragma once
struct Punto {
    double lat, lon;
    Punto() : lat(0), lon(0) {}
    Punto(double lat_, double lon_) : lat(lat_), lon(lon_) {}
};