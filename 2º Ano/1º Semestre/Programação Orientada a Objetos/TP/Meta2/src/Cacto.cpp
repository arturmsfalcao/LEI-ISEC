#include "Cacto.h"
#include "Jardim.h"
#include "Settings.h"
#include "Posicao.h"
#include <algorithm>
#include <cmath>
#include <iostream>

Cacto::Cacto() {
    total_nutrientes_absorvidos = 0.0;
    instantes_agua_alta = 0;
    instantes_nutrientes_zero = 0;
}

Cacto::Cacto(const Cacto& outro) : Planta(outro) {
    instantes_encharcado = outro.instantes_encharcado;
    instantes_desde_multiplicacao = outro.instantes_desde_multiplicacao;
}

char Cacto::getRepresentacao() const {
    return 'c';
}

bool Cacto::estaVivo() const {
    bool morte_agua = instantes_agua_alta >= Settings::Cacto::morre_agua_solo_instantes;
    bool morte_nutrientes = instantes_nutrientes_zero >= Settings::Cacto::morre_nutrientes_solo_instantes;

    return !(morte_agua || morte_nutrientes);
}

void Cacto::passarInstante(Posicao &pos) {
    double agua_no_solo = pos.getAgua();
    double agua_absorvida = agua_no_solo * (Settings::Cacto::absorcao_agua_percentagem / 100.0);

    pos.setAgua(agua_no_solo - (int)std::round(agua_absorvida));
    agua += (int)std::round(agua_absorvida);

    double nutrientes_no_solo = pos.getNutrientes();
    double nutrientes_absorver = std::min(nutrientes_no_solo, (double)Settings::Cacto::absorcao_nutrientes);

    pos.setNutrientes(nutrientes_no_solo - (int)std::round(nutrientes_absorver));
    nutrientes += (int)std::round(nutrientes_absorver);

    total_nutrientes_absorvidos += nutrientes_absorver;

    if (pos.getAgua() > Settings::Cacto::morre_agua_solo_maior) {
        instantes_agua_alta++;
    } else {
        instantes_agua_alta = 0;
    }

    if (pos.getNutrientes() < Settings::Cacto::morre_nutrientes_solo_menor) {
        instantes_nutrientes_zero++;
    } else {
        instantes_nutrientes_zero = 0;
    }
}

void Cacto::liberarRecursos(Posicao &pos) {
    pos.adicionarNutrientes(total_nutrientes_absorvidos);
}

bool Cacto::podeMultiplicar() const {
    return nutrientes > Settings::Cacto::multiplica_nutrientes_maior &&
           agua > Settings::Cacto::multiplica_agua_maior;
}

Cacto* Cacto::tentarMultiplicar(Jardim* j, int linha, int coluna) {
    if (!podeMultiplicar() || !j) return nullptr;

    int dl[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dc[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    for (int i = 0; i < 8; ++i) {
        int nl = linha + dl[i];
        int nc = coluna + dc[i];

        if (nl < 0 || nl >= j->getLinhas() || nc < 0 || nc >= j->getColunas()) continue;

        Posicao& p = j->getPosicao(nl, nc);
        if (!p.getPlanta()) {
            Cacto* novo = new Cacto();

            double nova_agua = this->agua / 2.0;
            double novos_nutrientes = this->nutrientes / 2.0;

            novo->agua = (int)std::round(nova_agua);
            novo->nutrientes = (int)std::round(novos_nutrientes);

            this->agua = (int)std::round(nova_agua);
            this->nutrientes = (int)std::round(novos_nutrientes);

            p.setPlanta(novo);
            return novo;
        }
    }
    return nullptr;
}

Planta* Cacto::clone() const {
    return new Cacto(*this);
}

void Cacto::listarPropriedades() const {
    std::cout << "\n--- PROPRIEDADES DO CACTO [" << getRepresentacao() << "] ---\n";
    std::cout << "Beleza: " << getBeleza() << std::endl;
    std::cout << "Reserva Agua: " << agua << "\n";
    std::cout << "Reserva Nutrientes: " << nutrientes << "\n";
    std::cout << "Nutrientes Totais Absorvidos: " << total_nutrientes_absorvidos << "\n";
    std::cout << "Instantes Agua Alta (Risco Morte): " << instantes_agua_alta << "/" << Settings::Cacto::morre_agua_solo_instantes << "\n";
    std::cout << "Instantes Nutrientes Zero: " << instantes_nutrientes_zero << "/" << Settings::Cacto::morre_nutrientes_solo_menor << "\n";
    std::cout << "--- FIM PROPRIEDADES ---\n";
}