// Grafo.h
#ifndef GRAFO_H
#define GRAFO_H

#include "Punto.h"
#include "RutaGeografica.h"
#include <string>
#include <unordered_map>
#include <vector>
#include "TramoRuta.h"
struct Arista {
    std::string destino;
    double tiempo;        // en segundos
    std::string tipo;     // "caminar" o nombre de combi
};

class Grafo {
private:
    std::unordered_map<std::string, Punto> vertices;
    std::unordered_map<std::string, std::vector<Arista>> adyacencia;

public:
    void cargarDesdeGraphml(const std::string& archivo);
    void integrarRuta(const RutaGeografica& ruta);
    void agregarArista(const std::string& desde, const std::string& hasta, const std::string& tipo);
    
    std::vector<Punto> calcularRuta(const Punto& inicio, const Punto& fin); // (puedes dejar esta como fallback)
    std::vector<TramoRuta> Grafo::calcularRutaAvanzada(const Punto& inicio, const Punto& fin);
    std::string Grafo::encontrarNodoExistente(const Punto& p, double tolerancia) const ;
};

#endif
