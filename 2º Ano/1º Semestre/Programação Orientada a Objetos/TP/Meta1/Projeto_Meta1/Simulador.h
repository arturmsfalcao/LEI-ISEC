#ifndef SIMULADOR_H
#define SIMULADOR_H

#include "Jardim.h"

class Simulador {
    Jardim jardim;
    bool jardimCriado = false;

public:
    void criar(int l, int c);
};

#endif
