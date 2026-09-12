cmake_minimum_required(VERSION 3.30)
project(TRABALHO_FINAL)

set(CMAKE_CXX_STANDARD 20)

add_executable(start
        src/main.cpp
        src/LeitorComandos.cpp
        src/Simulador.cpp
        src/Jardim.cpp
        src/Posicao.cpp
        src/Jardineiro.cpp
        src/Planta.cpp
        src/Visualizador.cpp
        src/Cacto.cpp
        src/ErvaDaninha.cpp
        src/Roseira.cpp
        src/PlantaExotica.cpp
        src/Ferramenta.cpp
        src/Regador.cpp
        src/PacoteAdubo.cpp
        src/TesouraPoda.cpp
        src/FerramentaZ.cpp
)