#include "Ervadaninha.h"
#include "Jardim.h"
#include "Settings.h"
#include "Posicao.h"
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <iostream>

ErvaDaninha::ErvaDaninha() {
    agua = Settings::ErvaDaninha::inicial_agua;
    nutrientes = Settings::ErvaDaninha::inicial_nutrientes;
    instantes_vida = 0;
    instantes_ultima_multiplicacao = 0;
}

ErvaDaninha::ErvaDaninha(const ErvaDaninha& outra) : Planta(outra) {
    instantes_ultima_multiplicacao = outra.instantes_ultima_multiplicacao;
}

char ErvaDaninha::getRepresentacao() const {
    return 'e';
}

bool ErvaDaninha::estaVivo() const {
    return instantes_vida < Settings::ErvaDaninha::morre_instantes;
}

void ErvaDaninha::passarInstante(Posicao &pos) {
    instantes_vida++;
    instantes_ultima_multiplicacao++;

    double agua_absorver = std::min((double)pos.getAgua(), (double)Settings::ErvaDaninha::absorcao_agua);
    pos.setAgua(pos.getAgua() - (int)agua_absorver);
    agua += (int)agua_absorver;

    double nutrientes_absorver = std::min((double)pos.getNutrientes(), (double)Settings::ErvaDaninha::absorcao_nutrientes);
    pos.setNutrientes(pos.getNutrientes() - (int)nutrientes_absorver);
    nutrientes += (int)nutrientes_absorver;
}

void ErvaDaninha::liberarRecursos(Posicao &pos) {
}

bool ErvaDaninha::podeMultiplicar() const {
    return nutrientes > Settings::ErvaDaninha::multiplica_nutrientes_maior &&
           instantes_ultima_multiplicacao >= Settings::ErvaDaninha::multiplica_instantes;
}

void ErvaDaninha::resetMultiplicacao() {
    instantes_ultima_multiplicacao = 0;
}

Planta* ErvaDaninha::tentarMultiplicar(Jardim* j, int linha, int coluna) {
    if (!podeMultiplicar() || !j) return nullptr;

    int dl[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dc[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    int start_index = std::rand() % 8;

    for (int i = 0; i < 8; ++i) {
        int index = (start_index + i) % 8;
        int nl = linha + dl[index];
        int nc = coluna + dc[index];

        if (nl < 0 || nl >= j->getLinhas() || nc < 0 || nc >= j->getColunas()) continue;

        Posicao& p = j->getPosicao(nl, nc);

        ErvaDaninha* nova = new ErvaDaninha();

        if (Planta* planta_existente = p.getPlanta()) {
            planta_existente->liberarRecursos(p);
            delete planta_existente;
        }

        p.setPlanta(nova);
        resetMultiplicacao();

        return nova;
    }

    return nullptr;
}

Planta* ErvaDaninha::clone() const {
    return new ErvaDaninha(*this);
}

void ErvaDaninha::listarPropriedades() const {
    std::cout << "\n--- PROPRIEDADES DA ERVA DANINHA [" << getRepresentacao() << "] ---\n";
    std::cout << "Beleza: " << getBeleza() << std::endl;
    std::cout << "Reserva Agua: " << agua << "\n";
    std::cout << "Reserva Nutrientes: " << nutrientes << "\n";
    std::cout << "Instantes de Vida: " << instantes_vida << "/" << Settings::ErvaDaninha::morre_instantes << "\n";
    std::cout << "Instantes Apos Multiplicacao: " << instantes_ultima_multiplicacao << "/" << Settings::ErvaDaninha::multiplica_instantes << "\n";
    std::cout << "--- FIM PROPRIEDADES ---\n";
}