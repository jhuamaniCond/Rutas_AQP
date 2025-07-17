// === src/Sistema.cpp ===
#include "Sistema.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream> // Para imprimir errores

std::vector<RutaGeografica> Sistema::obtenerTodasLasRutas()  const{
    return rutas;
}

void Sistema::cargarGrafo(const std::string& archivoGraphml) {
    try {
        std::cout << "Cargando grafo desde: " << archivoGraphml << std::endl;
        grafo.cargarDesdeGraphml(archivoGraphml);
        std::cout << "Grafo cargado exitosamente.\n";
    } catch (const std::exception& e) {
        std::cerr << "Error al cargar el grafo: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Error desconocido al cargar el grafo.\n";
    }
}

void Sistema::cargarRutasDesdeCarpeta(const std::string& carpeta) {
    try {
        std::cout << "Cargando rutas desde carpeta: " << carpeta << std::endl;
        for (const auto& entry : std::filesystem::directory_iterator(carpeta)) {
            if (entry.path().extension() == ".txt") {
                std::string nombre = entry.path().stem().string();
                std::string archivo = entry.path().string();
                std::cout << "Leyendo ruta: " << archivo << std::endl;
                cargarRutaCombiDesdeArchivo(nombre, archivo);
            }
        }
        std::cout << "Todas las rutas cargadas correctamente.\n";
    } catch (const std::exception& e) {
        std::cerr << "Error al cargar rutas desde carpeta: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Error desconocido al cargar rutas desde carpeta.\n";
    }
}

void Sistema::cargarRutaCombiDesdeArchivo(const std::string& nombre, const std::string& archivo) {
    try {
        std::ifstream file(archivo);
        if (!file.is_open()) {
            std::cerr << "No se pudo abrir el archivo: " << archivo << std::endl;
            return;
        }

        RutaGeografica ruta;
        ruta.id = nombre;
        ruta.nombreCorto = nombre;
        ruta.descripcion = "Ruta " + nombre;

        std::string linea;
        while (std::getline(file, linea)) {
            std::stringstream ss(linea);
            std::string lat_str, lon_str;

            if (std::getline(ss, lat_str, ',') && std::getline(ss, lon_str)) {
                double lat = std::stod(lat_str);
                double lon = std::stod(lon_str);
                ruta.puntos.emplace_back(lat, lon);
            }
        }

        std::cout << "Ruta cargada: " << nombre << " con " << ruta.puntos.size() << " puntos.\n";
        rutas.push_back(ruta);
        grafo.integrarRuta(ruta);
    } catch (const std::exception& e) {
        std::cerr << "Error al cargar ruta " << nombre << ": " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Error desconocido al cargar ruta: " << nombre << std::endl;
    }
}

std::vector<TramoRuta> Sistema::buscarRutaAvanzada(const Punto& origen, const Punto& destino) {
    try {
        std::cout << "Buscando ruta de (" << origen.lat << ", " << origen.lon << ") a ("
                  << destino.lat << ", " << destino.lon << ")" << std::endl;
        return grafo.calcularRutaAvanzada(origen, destino);
    } catch (const std::exception& e) {
        std::cerr << "Error al buscar ruta avanzada: " << e.what() << std::endl;
        return {};
    } catch (...) {
        std::cerr << "Error desconocido al buscar ruta avanzada.\n";
        return {};
    }
}
