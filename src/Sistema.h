// === src/Sistema.h ===
#pragma once
#include <vector>
#include <string>
#include "Punto.h"
#include "RutaGeografica.h"
#include "Grafo.h"


class Sistema {
    private:
        Grafo grafo;
        std::vector<RutaGeografica> rutas;
        std::unordered_map<std::string, std::vector<Punto>> rutasCombis;
    public:
        void cargarGrafo(const std::string& archivoGraphml);
        void cargarRutasDesdeCarpeta(const std::string& carpeta);
        void cargarRutaCombiDesdeArchivo(const std::string& nombre, const std::string& archivo);

        std::vector<TramoRuta> buscarRutaAvanzada(const Punto& origen, const Punto& destino);
        std::vector<RutaGeografica> obtenerTodasLasRutas() const;
    };
