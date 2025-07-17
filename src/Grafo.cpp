#include "Grafo.h"
#include "pugixml.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <limits>
#include <queue>
#include <iomanip> // para std::setprecision

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif



double distancia(const Punto& a, const Punto& b);

void Grafo::agregarArista(const std::string& desde, const std::string& hasta, const std::string& tipo) {
    double dist = distancia(vertices[desde], vertices[hasta]); // en grados geográficos
    double velocidad = (tipo == "caminar") ? 6.0 : 30.0; // km/h
    double tiempo = (dist/ velocidad) * 60.0; // conversión a minutos

    adyacencia[desde].push_back({hasta, tiempo, tipo});
    adyacencia[hasta].push_back({desde, tiempo, tipo}); // bidireccional

    
}

void Grafo::cargarDesdeGraphml(const std::string& filename) {
    pugi::xml_document doc;
    if (!doc.load_file(filename.c_str())) {
        throw std::runtime_error("No se pudo cargar el archivo .graphml");
    }

    auto graph = doc.child("graphml").child("graph");

    for (auto node : graph.children("node")) {
        std::string id = node.attribute("id").as_string();
        double lat = 0.0, lon = 0.0;

        for (auto data : node.children("data")) {
            std::string key = data.attribute("key").as_string();
            std::string value = data.text().as_string();

            if (key == "d4") {
                lat = std::stod(value);
            } else if (key == "d5") {
                lon = std::stod(value);
            }
        }

        vertices[id] = Punto{lat, lon};
    }

    for (auto edge : graph.children("edge")) {
        std::string from = edge.attribute("source").as_string();
        std::string to = edge.attribute("target").as_string();

        double dist = distancia(vertices[from], vertices[to]); // en km
        double tiempo = (dist / 6.0) * 60.0; // 6 km/h → minutos

        agregarArista(from, to, "caminar");
    }

    std::cout << "[INFO] Cargado grafo con " << vertices.size() << " nodos.\n";
}
std::string Grafo::encontrarNodoExistente(const Punto& p, double tolerancia) const {
    for (const auto& [id, punto] : vertices) {
        if (std::abs(p.lat - punto.lat) < tolerancia && std::abs(p.lon - punto.lon) < tolerancia) {
            return id;
        }
    }
    return ""; // No encontrado
}
void Grafo::integrarRuta(const RutaGeografica& ruta) {
    std::cout << "[INFO] Integrando ruta " << ruta.id << " con " << ruta.puntos.size() << " puntos.\n";
    std::cout << "[DEBUG] Nodos conectados para la ruta " << ruta.id << ":\n";

    std::vector<std::string> nodosRuta;

    for (const Punto& p : ruta.puntos) {
        std::string nodoExistente = encontrarNodoExistente(p, 1e-5);
        if (!nodoExistente.empty()) {
            nodosRuta.push_back(nodoExistente);
        } else {
            std::string nuevoId = "ruta_" + ruta.id + "_" + std::to_string(nodosRuta.size());
            vertices[nuevoId] = p;
            nodosRuta.push_back(nuevoId);
        }
    }

    // Conectar los nodos en secuencia
    for (size_t i = 0; i + 1 < nodosRuta.size(); ++i) {
        agregarArista(nodosRuta[i], nodosRuta[i + 1], ruta.id);
        const auto& p = vertices[nodosRuta[i]];
        std::cout << " - " << nodosRuta[i] << " (" << p.lat << ", " << p.lon << ")\n";
    }

    // Mostrar último nodo
    const auto& pUltimo = vertices[nodosRuta.back()];
    std::cout << " - " << nodosRuta.back() << " (" << pUltimo.lat << ", " << pUltimo.lon << ")\n";
}


double distancia(const Punto& a, const Punto& b) {
    const double R = 6371.0; // Radio de la Tierra en km
    double lat1 = a.lat * M_PI / 180.0;
    double lon1 = a.lon * M_PI / 180.0;
    double lat2 = b.lat * M_PI / 180.0;
    double lon2 = b.lon * M_PI / 180.0;

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double a_hav = sin(dlat / 2) * sin(dlat / 2) +
                   cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a_hav), sqrt(1 - a_hav));
    return R * c;
}

std::vector<Punto> Grafo::calcularRuta(const Punto& inicio, const Punto& fin) {
    std::string inicioId, finId;
    double minInicio = std::numeric_limits<double>::max();
    double minFin = std::numeric_limits<double>::max();

    for (const auto& [id, p] : vertices) {
        double d1 = distancia(p, inicio);
        if (d1 < minInicio) {
            minInicio = d1;
            inicioId = id;
        }

        double d2 = distancia(p, fin);
        if (d2 < minFin) {
            minFin = d2;
            finId = id;
        }
    }

    if (inicioId.empty() || finId.empty()) {
        throw std::runtime_error("No se pudo encontrar nodos cercanos.");
    }

    std::unordered_map<std::string, double> dist;
    std::unordered_map<std::string, std::string> prev;
    std::priority_queue<std::pair<double, std::string>,
                        std::vector<std::pair<double, std::string>>,
                        std::greater<>> pq;

    for (const auto& [id, _] : vertices)
        dist[id] = std::numeric_limits<double>::infinity();

    dist[inicioId] = 0.0;
    pq.emplace(0.0, inicioId);

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (u == finId) break;

        for (const auto& arista : adyacencia[u]) {
            const std::string& destino = arista.destino;
            double alt = dist[u] + distancia(vertices[u], vertices[destino]);
            if (alt < dist[destino]) {
                dist[destino] = alt;
                prev[destino] = u;
                pq.emplace(alt, destino);
            }
        }
    }

    std::vector<Punto> camino;
    for (std::string at = finId; !at.empty(); at = prev[at]) {
        camino.push_back(vertices[at]);
        if (at == inicioId) break;
    }

    std::reverse(camino.begin(), camino.end());
    return camino;
}
std::vector<TramoRuta> Grafo::calcularRutaAvanzada(const Punto& inicio, const Punto& fin) {
    std::string inicioId, finId;
    double minInicio = std::numeric_limits<double>::max(), minFin = minInicio;

    // Buscar los nodos más cercanos al inicio y fin
    for (const auto& [id, punto] : vertices) {
        double d1 = distancia(punto, inicio);
        if (d1 < minInicio) {
            minInicio = d1;
            inicioId = id;
        }
        double d2 = distancia(punto, fin);
        if (d2 < minFin) {
            minFin = d2;
            finId = id;
        }
    }
    std::cout << "[DEBUG] Nodo más cercano al INICIO: " << inicioId 
            << " (" << vertices[inicioId].lat << ", " << vertices[inicioId].lon << ")\n";
    std::cout << "[DEBUG] Nodo más cercano al FIN: " << finId 
            << " (" << vertices[finId].lat << ", " << vertices[finId].lon << ")\n";

    std::cout << "\n[DEBUG] Aristas conectadas al nodo de INICIO (" << inicioId << "):\n";
    for (const auto& arista : adyacencia[inicioId]) {
        std::cout << " - destino: " << arista.destino << " | tipo: " << arista.tipo 
                << " | tiempo: " << arista.tiempo << " min\n";
    }

    std::cout << "\n[DEBUG] Aristas conectadas al nodo de FIN (" << finId << "):\n";
    for (const auto& arista : adyacencia[finId]) {
        std::cout << " - destino: " << arista.destino << " | tipo: " << arista.tipo 
                << " | tiempo: " << arista.tiempo << " min\n";
    }

    using NodoInfo = std::tuple<double, std::string, std::string>; // tiempo, nodo, tipo
    std::priority_queue<NodoInfo, std::vector<NodoInfo>, std::greater<>> pq;
    std::unordered_map<std::string, double> tiempo;
    std::unordered_map<std::string, std::string> anterior;
    std::unordered_map<std::string, std::string> tipoArista;

    for (const auto& [id, _] : vertices)
        tiempo[id] = std::numeric_limits<double>::infinity();

    tiempo[inicioId] = 0.0;
    pq.emplace(0.0, inicioId, "caminar");

    while (!pq.empty()) {
        auto [t, u, tipo] = pq.top(); pq.pop();
        if (u == finId) break;

        for (const auto& arista : adyacencia[u]) {
            double nuevoTiempo = t + arista.tiempo;
            if (nuevoTiempo < tiempo[arista.destino]) {
                tiempo[arista.destino] = nuevoTiempo;
                anterior[arista.destino] = u;
                tipoArista[arista.destino] = arista.tipo;
                pq.emplace(nuevoTiempo, arista.destino, arista.tipo);
            }
        }
    }

    std::vector<std::tuple<std::string, double>> resumenTramos;

    std::string actual = finId;

    if (anterior.find(actual) == anterior.end()) {
        std::cerr << "[WARN] No se encontró ruta entre los puntos.\n";
        return {};
    }

    std::vector<TramoRuta> resultado;

    std::vector<Punto> tramo;
    std::string tipoActual = tipoArista[actual];
    double tiempoTramoAcumulado = 0.0;
    double distanciaTramoAcumulada = 0.0;

    tramo.push_back(vertices[actual]);

    std::cout << "\n[DEBUG] Tiempo acumulado hasta nodo final (" << finId << "): "
            << tiempo[finId] << " minutos\n";

    while (anterior.find(actual) != anterior.end()) {
        std::string prev = anterior[actual];
        std::string tipo = tipoArista[actual];

        const Punto& puntoPrev = vertices[prev];
        const Punto& puntoActual = vertices[actual];

        double d = distancia(puntoPrev, puntoActual); // en km reales
        double velocidad = (tipo == "caminar") ? 6.0 : 30.0;
        double tiempoTramo = (d / velocidad) * 60.0; // en minutos

        std::cout << "[DEBUG] Tramo " << tipo << " " << prev << " (" << puntoPrev.lat << ", " << puntoPrev.lon << ")"
                << " -> " << actual << " (" << puntoActual.lat << ", " << puntoActual.lon << ")"
                << " | distancia: " << d << " km"
                << " | tiempo estimado: " << tiempoTramo << " min\n";

        if (tipo != tipoActual) {
            std::reverse(tramo.begin(), tramo.end());
            resultado.push_back({tipoActual, tramo, tiempoTramoAcumulado, distanciaTramoAcumulada});

            tramo.clear();
            tipoActual = tipo;
            tiempoTramoAcumulado = 0.0;
            distanciaTramoAcumulada = 0.0;
        }

        tiempoTramoAcumulado += tiempoTramo;
        distanciaTramoAcumulada += d;
        tramo.push_back(puntoPrev);
        actual = prev;
    }

    // Agregar último tramo
    if (!tramo.empty()) {
        std::reverse(tramo.begin(), tramo.end());
        resultado.push_back({tipoActual, tramo, tiempoTramoAcumulado, distanciaTramoAcumulada});
    }

    std::reverse(resultado.begin(), resultado.end());

    std::cout << "\n========= Resumen del recorrido =========\n";
    double totalTiempo = 0.0;
    double totalDistancia = 0.0;
    for (const auto& tramo : resultado) {
        std::cout << " - " << tramo.tipo << ": " << std::fixed << std::setprecision(2)
                << tramo.distancia << " km, " << tramo.tiempo << " minutos\n";
        totalTiempo += tramo.tiempo;
        totalDistancia += tramo.distancia;
    }
    std::cout << "Tiempo total: " << totalTiempo << " minutos\n";
    std::cout << "Distancia total: " << totalDistancia << " km\n";
    std::cout << "=========================================\n";
    return resultado;
}
