#include "Jardim.h"
#include "Visualizador.h"

Jardim::Jardim(int l, int c) : linhas(l), colunas(c) {
    grade.resize(linhas, std::vector<Posicao>(colunas));
}

void Jardim::mostrar() const {
    Visualizador::mostrar(grade);
}
