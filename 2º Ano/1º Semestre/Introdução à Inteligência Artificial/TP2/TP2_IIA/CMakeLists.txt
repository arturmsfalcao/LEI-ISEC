cmake_minimum_required(VERSION 3.30)
project(TP2_IIA C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(TP2_IIA
        main.c
    utils.c
    utils.h
    trepa_colinas.c
    trepa_colinas.h
    funcao.c
    funcao.h
    evolutivo.c
    evolutivo.h
    hibrido.c
    hibrido.h)
