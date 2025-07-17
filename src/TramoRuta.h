#pragma once
#include <string>
#include <vector>
#include "Punto.h"



struct TramoRuta {
    std::string tipo;
    std::vector<Punto> puntos;
    double tiempo;     // en minutos
    double distancia;  // en kilómetros

    TramoRuta(const std::string& tipo_,
              const std::vector<Punto>& puntos_,
              double tiempo_,
              double distancia_)
        : tipo(tipo_), puntos(puntos_), tiempo(tiempo_), distancia(distancia_) {}
};