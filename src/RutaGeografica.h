// === src/RutaGeografica.h ===
#pragma once
#include <vector>
#include <string>
#include "Punto.h"

struct RutaGeografica {
    std::string id;
    std::string nombreCorto;
    std::string descripcion;
    std::vector<Punto> puntos;
};