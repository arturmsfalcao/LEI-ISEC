#ifndef JARDIM_H
#define JARDIM_H

#include <vector>
#include "Posicao.h"

class Jardim {
    int linhas, colunas;
    std::vector<std::vector<Posicao>> grade;

public:
    Jardim() = default;
    Jardim(int linhas, int colunas);

    void mostrar() const;
    int getLinhas() const { return linhas; }
    int getColunas() const { return colunas; }
};

#endif
