#ifndef VISUALIZADOR_H
#define VISUALIZADOR_H

#include <vector>
#include "Posicao.h"

class Visualizador {
public:
    static void mostrar(const std::vector<std::vector<Posicao>> &grade);
};

#endif
