#ifndef JARDINEIRO_H
#define JARDINEIRO_H

class Ferramenta;

#include <vector>
#include "Ferramenta.h"

class Jardineiro {
    bool dentro = false;
    int linha = 0, coluna = 0;

    std::vector<Ferramenta*> ferramentas;
    Ferramenta* ferramentaNaMao = nullptr;

    int movimentos_atuais = 0;
    int entradas_saidas_atuais = 0;
    int plantacoes_atuais = 0;
    int colheitas_atuais = 0;

public:
    Jardineiro() = default;
    ~Jardineiro();

    bool estaDentro() const { return dentro; }
    int getLinha() const { return linha; }
    int getColuna() const { return coluna; }
    void entra(int l, int c);
    void sai();
    bool move(char direcao, int maxLinhas, int maxColunas);

    void resetLimites();
    bool podeMover() const;
    bool podeEntrarSair() const;
    bool podePlantar() const;
    bool podeColher() const;

    Ferramenta* getFerramentaNaMao() const { return ferramentaNaMao; }
    void adicionarFerramenta(Ferramenta* f);
    Ferramenta* removerFerramenta(int numeroSerie);
    void usaFerramentaAtiva(Posicao &pos);
    void apanhaFerramenta(Posicao& pos);
    void pega(int numeroSerie);
    void larga();
    void listarFerramentas() const;

    void incrementaPlantacoes();
    void incrementaColheitas();
    void colhe(Posicao& pos);
};

#endif