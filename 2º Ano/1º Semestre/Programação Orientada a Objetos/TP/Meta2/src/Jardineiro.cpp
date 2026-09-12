#include "Jardineiro.h"
#include "Posicao.h"
#include "Settings.h"
#include <iostream>
#include <algorithm>
#include <vector>

Jardineiro::~Jardineiro() {
    for (Ferramenta* f : ferramentas) {
        delete f;
    }
}

void Jardineiro::resetLimites() {
    movimentos_atuais = 0;
    entradas_saidas_atuais = 0;
    plantacoes_atuais = 0;
    colheitas_atuais = 0;
}

bool Jardineiro::podeMover() const {
    return movimentos_atuais < Settings::Jardineiro::max_movimentos;
}

bool Jardineiro::podeEntrarSair() const {
    return entradas_saidas_atuais < Settings::Jardineiro::max_entradas_saidas;
}

bool Jardineiro::podePlantar() const {
    return plantacoes_atuais < Settings::Jardineiro::max_plantacoes;
}

bool Jardineiro::podeColher() const {
    return colheitas_atuais < Settings::Jardineiro::max_colheitas;
}

void Jardineiro::entra(int l, int c) {
    if (podeEntrarSair()) {
        dentro = true;
        linha = l;
        coluna = c;
        entradas_saidas_atuais++;
        std::cout << "Jardineiro entra em: " << (char)('A' + l) << (char)('A' + c) << std::endl;
    } else {
        std::cout << "[INFO] Limite de entradas/saídas por instante esgotado.\n";
    }
}

void Jardineiro::sai() {
    if (podeEntrarSair()) {
        dentro = false;
        entradas_saidas_atuais++;
        std::cout << "Jardineiro sai do jardim.\n";
    } else {
        std::cout << "[INFO] Limite de entradas/saídas por instante esgotado.\n";
    }
}

bool Jardineiro::move(char direcao, int maxLinhas, int maxColunas) {
    if (!dentro) {
        std::cout << "[ERRO] Jardineiro nao esta no jardim para se mover.\n";
        return false;
    }
    if (!podeMover()) {
        std::cout << "[INFO] Limite de movimentos por instante esgotado.\n";
        return false;
    }

    int novaLinha = linha;
    int novaColuna = coluna;

    switch (direcao) {
        case 'c': novaLinha--; break;
        case 'b': novaLinha++; break;
        case 'e': novaColuna--; break;
        case 'd': novaColuna++; break;
        default: return false;
    }

    if (novaLinha >= 0 && novaLinha < maxLinhas && novaColuna >= 0 && novaColuna < maxColunas) {
        linha = novaLinha;
        coluna = novaColuna;
        movimentos_atuais++;
        return true;
    }
    std::cout << "[INFO] Movimento bloqueado: Jardineiro no limite do jardim.\n";
    return false;
}

void Jardineiro::adicionarFerramenta(Ferramenta* f) {
    if (f) {
        ferramentas.push_back(f);
    }
}

Ferramenta* Jardineiro::removerFerramenta(int numeroSerie) {
    auto it = std::find_if(ferramentas.begin(), ferramentas.end(),
        [numeroSerie](Ferramenta* f){ return f->getNumeroSerie() == numeroSerie; });

    if (it != ferramentas.end()) {
        Ferramenta* f = *it;

        if (ferramentaNaMao == f) {
            ferramentaNaMao = nullptr;
        }

        ferramentas.erase(it);
        return f;
    }
    return nullptr;
}

void Jardineiro::apanhaFerramenta(Posicao& pos) {
    Ferramenta* f = pos.getFerramenta();

    if (f) {
        pos.setFerramenta(nullptr);
        adicionarFerramenta(f);
        std::cout << "[INFO] Ferramenta '" << f->getSimbolo()
                  << "' (Serie " << f->getNumeroSerie() << ") apanhada automaticamente.\n";
    }
}

void Jardineiro::pega(int numeroSerie) {
    for (Ferramenta* f : ferramentas) {
        if (f->getNumeroSerie() == numeroSerie) {
            ferramentaNaMao = f;
            std::cout << "Ferramenta " << numeroSerie << " na mao.\n";
            return;
        }
    }
    std::cout << "[ERRO] Ferramenta com numero de serie " << numeroSerie << " nao encontrada.\n";
}

void Jardineiro::larga() {
    if (ferramentaNaMao) {
        ferramentaNaMao = nullptr;
        std::cout << "Ferramenta guardada.\n";
    } else {
        std::cout << "[INFO] Nenhuma ferramenta na mao para guardar.\n";
    }
}

void Jardineiro::usaFerramentaAtiva(Posicao &pos) {
    if (ferramentaNaMao) {
        ferramentaNaMao->usar(pos);

        if (ferramentaNaMao->estaGasta()) {
            std::cout << "[INFO] Ferramenta " << ferramentaNaMao->getNumeroSerie()
                      << " gasta e descartada.\n";

            Ferramenta* f_gasta = removerFerramenta(ferramentaNaMao->getNumeroSerie());
            if (f_gasta) {
                delete f_gasta;
            }
            ferramentaNaMao = nullptr;
        }
    }
}

void Jardineiro::listarFerramentas() const {
    if (ferramentas.empty()) {
        std::cout << "\n[INFO] O Jardineiro nao transporta nenhuma ferramenta.\n";
        return;
    }

    std::cout << "\n--- INVENTARIO DE FERRAMENTAS ---\n";

    for (const Ferramenta* f : ferramentas) {
        std::cout << "Serie " << f->getNumeroSerie() << " [" << f->getSimbolo() << "] - ";
        f->imprimirDetalhes();

        if (f == ferramentaNaMao) {
            std::cout << " (NA MAO - ATIVA)";
        }
        std::cout << "\n";
    }
    std::cout << "---------------------------------\n";
}

void Jardineiro::incrementaColheitas() {
    colheitas_atuais++;
}

void Jardineiro::incrementaPlantacoes() {
    plantacoes_atuais++;
}

void Jardineiro::colhe(Posicao& pos) {
    if (!podeColher()) {
        std::cout << "[INFO] Limite de colheitas (max " << Settings::Jardineiro::max_colheitas << ") por instante esgotado.\n";
        return;
    }

    Planta* planta = pos.getPlanta();

    if (!planta) {
        std::cout << "[ERRO] Nao ha planta para colher na posicao.\n";
        return;
    }

    planta->liberarRecursos(pos);
    delete planta;
    pos.setPlanta(nullptr);
    incrementaColheitas();

    std::cout << "[INFO] Planta colhida e removida da posicao "
              << (char)('A' + pos.getLinha()) << (char)('A' + pos.getColuna())
              << ".\n";
}