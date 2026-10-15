#ifndef POSICAO_H
#define POSICAO_H

#include "Planta.h"
#include "Ferramenta.h"

class Posicao {
    int agua;
    int nutrientes;
    int linha;
    int coluna;
    Planta* planta = nullptr;
    Ferramenta* ferramenta = nullptr;

public:
    Posicao();

    void setCoordenadas(int l, int c);

    int getLinha() const { return linha; }
    int getColuna() const { return coluna; }

    char getSimbolo() const;

    int getAgua() const { return agua; }
    void setAgua(int a) { agua = a; }

    int getNutrientes() const { return nutrientes; }
    void setNutrientes(int n) { nutrientes = n; }

    void adicionarAgua(double quantidade) {
        agua += (int)quantidade;
    }

    void adicionarNutrientes(double quantidade) {
        nutrientes += (int)quantidade;
    }

    void listarRecursos() const;

    Planta* getPlanta() const { return planta; }
    void setPlanta(Planta* p) { planta = p; }

    Ferramenta* getFerramenta() const { return ferramenta; }
    void setFerramenta(Ferramenta* f) { ferramenta = f; }
};

#endif