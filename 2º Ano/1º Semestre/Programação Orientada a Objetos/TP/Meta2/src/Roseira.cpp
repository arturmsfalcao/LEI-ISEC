#include "Roseira.h"
#include "Jardim.h"
#include "Settings.h"
#include "Posicao.h"
#include <cstdlib>
#include <iostream>

Roseira::Roseira() {
    agua = Settings::Roseira::inicial_agua;
    nutrientes = Settings::Roseira::inicial_nutrientes;
    total_agua_absorvida = 0.0;
    total_nutrientes_absorvidos = 0.0;
}

Roseira::Roseira(const Roseira& outra) : Planta(outra) {
    total_agua_absorvida = outra.total_agua_absorvida;
    total_nutrientes_absorvidos = outra.total_nutrientes_absorvidos;
}

char Roseira::getRepresentacao() const {
    return 'r';
}

bool Roseira::estaVivo() const {
    return !(agua <= Settings::Roseira::morre_agua_menor ||
             nutrientes <= Settings::Roseira::morre_nutrientes_menor ||
             nutrientes >= Settings::Roseira::morre_nutrientes_maior);
}

void Roseira::passarInstante(Posicao &pos) {
    agua -= Settings::Roseira::perda_agua;
    nutrientes -= Settings::Roseira::perda_nutrientes;

    double absorcao_agua = Settings::Roseira::absorcao_agua;
    double absorcao_nutrientes = Settings::Roseira::absorcao_nutrientes;

    double agua_a_tirar = std::min((double)pos.getAgua(), absorcao_agua);
    double nutrientes_a_tirar = std::min((double)pos.getNutrientes(), absorcao_nutrientes);

    pos.setAgua(pos.getAgua() - (int)agua_a_tirar);
    pos.setNutrientes(pos.getNutrientes() - (int)nutrientes_a_tirar);

    agua += (int)agua_a_tirar;
    nutrientes += (int)nutrientes_a_tirar;

    total_agua_absorvida += agua_a_tirar;
    total_nutrientes_absorvidos += nutrientes_a_tirar;
}

bool Roseira::deveMorrerPorSufocamento(Jardim* j, int linha, int coluna) const {
    if (!j) return false;

    int dl[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dc[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    for (int i = 0; i < 8; ++i) {
        int nl = linha + dl[i];
        int nc = coluna + dc[i];

        if (nl < 0 || nl >= j->getLinhas() || nc < 0 || nc >= j->getColunas()) {
            continue;
        }

        Posicao& p = j->getPosicao(nl, nc);

        if (!p.getPlanta()) {
            return false;
        }
    }

    return true;
}

void Roseira::liberarRecursos(Posicao &pos) {
    pos.adicionarAgua(total_agua_absorvida / 2.0);
    pos.adicionarNutrientes(total_nutrientes_absorvidos / 2.0);
}

Roseira* Roseira::tentarMultiplicar(Jardim* j, int linha, int coluna) {
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

        if (!p.getPlanta()) {
            Roseira* nova = new Roseira();

            int aguaPartilhada = this->agua / 2;
            nova->agua = aguaPartilhada;
            this->agua = aguaPartilhada;

            nova->nutrientes = Settings::Roseira::nova_nutrientes;
            this->nutrientes = Settings::Roseira::original_nutrientes;

            p.setPlanta(nova);
            return nova;
        }
    }
    return nullptr;
}

bool Roseira::podeMultiplicar() const {
    return nutrientes > Settings::Roseira::multiplica_nutrientes_maior;
}

Planta* Roseira::clone() const {
    return new Roseira(*this);
}

void Roseira::listarPropriedades() const {
    std::cout << "\n--- PROPRIEDADES DA ROSEIRA [" << getRepresentacao() << "] ---\n";
    std::cout << "Beleza: " << getBeleza() << std::endl;
    std::cout << "Reserva Agua: " << agua << "\n";
    std::cout << "Reserva Nutrientes: " << nutrientes << "\n";
    std::cout << "Total Agua Absorvida (Vida): " << total_agua_absorvida << "\n";
    std::cout << "Total Nutrientes Absorvidos (Vida): " << total_nutrientes_absorvidos << "\n";
    std::cout << "--- FIM PROPRIEDADES ---\n";
}