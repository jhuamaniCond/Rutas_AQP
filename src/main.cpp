// === src/main.cpp ===
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include "Sistema.h"
#include "TramoRuta.h"
using namespace web;
using namespace http;
using namespace http::experimental::listener;

Sistema sistema;

std::string to_ascii_string(const utility::string_t& input) {
    std::string result;
    for (wchar_t wc : input) result += static_cast<char>(wc);
    return result;
}

void enviar_json(http_request request, const json::value& body) {
    http_response response(status_codes::OK);
    response.headers().add(U("Content-Type"), U("application/json"));
    response.set_body(body);
    request.reply(response);
}
bool ends_with(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void handle_get(http_request request) {
    auto path = to_ascii_string(request.relative_uri().path());

    if (path == "/rutas/listar") {
        json::value rutas_json = json::value::array();
        const auto& rutas = sistema.obtenerTodasLasRutas(); // Este método lo implementamos abajo

        int i = 0;
        for (const auto& ruta : rutas) {
            json::value ruta_json;
            ruta_json[U("nombre")] = json::value::string(utility::conversions::to_string_t(ruta.nombreCorto)); 
            json::value puntos = json::value::array();
            int j = 0;
            for (const auto& p : ruta.puntos) {
                json::value punto;
                punto[U("lat")] = p.lat;
                punto[U("lng")] = p.lon;
                puntos[j++] = punto;
            }
            ruta_json[U("puntos")] = puntos;
            rutas_json[i++] = ruta_json;
        }

        enviar_json(request, rutas_json);
    } else {
        // Servir archivos estáticos de public/
        std::string archivo = "./public" + (path == "/" ? "/index.html" : path);
        std::ifstream file(archivo, std::ios::binary);
        if (!file.is_open()) {
            request.reply(status_codes::NotFound, "Archivo no encontrado");
            return;
        }

        std::ostringstream ss;
        ss << file.rdbuf();
        std::string contenido = ss.str();

        http_response response(status_codes::OK);
        if (ends_with(archivo, ".html")) response.headers().add(U("Content-Type"), U("text/html"));
        else if (ends_with(archivo, ".js")) response.headers().add(U("Content-Type"), U("application/javascript"));
        else if (ends_with(archivo, ".css")) response.headers().add(U("Content-Type"), U("text/css"));
        else response.headers().add(U("Content-Type"), U("application/octet-stream"));

        response.set_body(contenido);
        request.reply(response);
    }
}

void handle_post(http_request request) {
    request.extract_json().then([request](json::value body) {
        try {
            std::string operacion = to_ascii_string(body[U("operacion")].as_string());

            if (operacion == "buscar_ruta") {
                double lat1 = body[U("origen")][U("lat")].as_double();
                double lon1 = body[U("origen")][U("lon")].as_double();
                double lat2 = body[U("destino")][U("lat")].as_double();
                double lon2 = body[U("destino")][U("lon")].as_double();

                Punto origen(lat1, lon1);
                Punto destino(lat2, lon2);
                std::cout << "antes de ruta segmentada:\n";

                // Nueva ruta segmentada
                auto rutaSegmentada = sistema.buscarRutaAvanzada(origen, destino);

                // DEBUG: Imprimir la ruta segmentada
                std::cout << "[DEBUG] Ruta segmentada:\n";

                json::value result = json::value::array();
                int i = 0;

                for (const auto& tramo : rutaSegmentada) {
                    json::value obj;
                    obj[U("tipo")] = json::value::string(utility::conversions::to_string_t(tramo.tipo));
                    obj[U("tiempo")] = json::value::number(tramo.tiempo);
                    obj[U("distancia")] = json::value::number(tramo.distancia);

                    json::value puntos = json::value::array();
                    int j = 0;
                    for (const auto& p : tramo.puntos) {
                        json::value punto;
                        punto[U("lat")] = json::value::number(p.lat);
                        punto[U("lon")] = json::value::number(p.lon);
                        puntos[j++] = punto;
                    }

                    obj[U("puntos")] = puntos;
                    result[i++] = obj;
                }

                enviar_json(request, result);
            }

        } catch (...) {
            request.reply(status_codes::BadRequest, U("Error en JSON"));
        }
    });
}



int main() {
    sistema.cargarGrafo("./data/arequipa2.graphml");

    sistema.cargarRutasDesdeCarpeta("rutas/");

    http_listener listener(U("http://localhost:5000"));
    listener.support(methods::POST, handle_post);
    listener.support(methods::GET, handle_get);
    listener.open().wait();
    std::cout << ".---------------------------------------------------------------------" << std::endl;
    std::cout << "Servidor escuchando en http://localhost:5000" << std::endl;
    std::string dummy;
    std::getline(std::cin, dummy);
    listener.close().wait();
    return 0;
}
