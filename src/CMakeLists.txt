cmake_minimum_required(VERSION 3.15)
project(RutasArequipa)

# Usa C++17 o superior
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Habilitar vcpkg manifest si estás usando uno (opcional)
# set(VCPKG_FEATURE_FLAGS "manifests")

# Si usas vcpkg de forma clásica (no manifiesto), define el path
# Solo si CMake no lo detecta automáticamente
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

# Archivos fuente
file(GLOB SOURCES "*.cpp")
add_executable(RutasArequipa ${SOURCES})

# Librerías que vas a usar
find_package(pugixml CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)

# Vincular librerías al ejecutable
target_link_libraries(RutasArequipa PRIVATE pugixml cpprestsdk::cpprest)
