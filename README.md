Pasos para ejecutar el proyecto:
ir al directorio build.
ejecutar los comandos

```bash
cmake "-DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake" ../src
```
```bash
cmake --build . 
```

Luego para ejecutar el proyecto 

se debera ejecutar el archivo RutasArequipa.exe dentro del directorio Debug.
```bash
./RutasArequipa.exe
```
