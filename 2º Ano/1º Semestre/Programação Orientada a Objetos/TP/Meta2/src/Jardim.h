#ifndef JARDIM_H
#define JARDIM_H

#include "Posicao.h"
#include <vector>

class Jardim {
    int linhas, colunas;
    Posicao *grade = nullptr;

public:
    Jardim() = default;
    Jardim(int linhas, int colunas);
    Jardim(const Jardim& outro);
    ~Jardim();

    Posicao& getPosicao(int linha, int coluna);
    const Posicao& getPosicao(int linha, int coluna) const;

    void mostrar() const;
    int getLinhas() const { return linhas; }
    int getColunas() const { return colunas; }

    std::vector<Posicao*> getVizinhas(int linha, int coluna);
};

#endif