#ifndef LEITORCOMANDOS_H
#define LEITORCOMANDOS_H

#include <string>
#include "Simulador.h"

class LeitorComandos {
    Simulador simulador;

public:
    void iniciar();
    void processarLinha(const std::string &linha);
};

#endif
