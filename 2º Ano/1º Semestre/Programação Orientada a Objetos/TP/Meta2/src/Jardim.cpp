#include "Jardim.h"
#include <iostream>

Jardim::Jardim(int l, int c) : linhas(l), colunas(c) {
    if (linhas > 0 && colunas > 0) {
        grade = new Posicao[linhas * colunas];

        for (int l_idx = 0; l_idx < linhas; ++l_idx) {
            for (int c_idx = 0; c_idx < colunas; ++c_idx) {
                Posicao& pos = getPosicao(l_idx, c_idx);
                pos.setCoordenadas(l_idx, c_idx);
            }
        }
    }
}

Jardim::Jardim(const Jardim& outro) : linhas(outro.linhas), colunas(outro.colunas) {
    if (linhas > 0 && colunas > 0) {
        grade = new Posicao[linhas * colunas];
    } else {
        grade = nullptr;
        return;
    }

    for (int l = 0; l < linhas; ++l) {
        for (int c = 0; c < colunas; ++c) {
            int index = l * colunas + c;

            grade[index] = outro.grade[index];

            Planta* plantaOriginal = outro.grade[index].getPlanta();
            if (plantaOriginal) {
                grade[index].setPlanta(plantaOriginal->clone());
            }

            Ferramenta* ferramentaOriginal = outro.grade[index].getFerramenta();
            if (ferramentaOriginal) {
                grade[index].setFerramenta(ferramentaOriginal->clone());
            }
        }
    }
}

Jardim::~Jardim() {
    delete[] grade;
}

Posicao& Jardim::getPosicao(int linha, int coluna) {
    if(linha < 0 || linha >= linhas || coluna < 0 || coluna >= colunas) {
        throw std::out_of_range("Posicao fora do jardim");
    }
    return grade[linha * colunas + coluna];
}

const Posicao& Jardim::getPosicao(int linha, int coluna) const {
    if(linha < 0 || linha >= linhas || coluna < 0 || coluna >= colunas) {
        throw std::out_of_range("Posicao fora do jardim");
    }
    return grade[linha * colunas + coluna];
}

void Jardim::mostrar() const {
    std::cout << "  ";
    for (int c = 0; c < colunas; ++c)
        std::cout << static_cast<char>('A' + c);
    std::cout << std::endl;

    for (int l = 0; l < linhas; ++l) {
        std::cout << static_cast<char>('A' + l);
        for (int c = 0; c < colunas; ++c) {
            std::cout << getPosicao(l,c).getSimbolo();
        }
        std::cout << std::endl;
    }
}

std::vector<Posicao*> Jardim::getVizinhas(int linha, int coluna) {
    std::vector<Posicao*> vizinhas;
    for (int dl = -1; dl <= 1; dl++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dl == 0 && dc == 0) continue;
            int nl = linha + dl;
            int nc = coluna + dc;
            if (nl >= 0 && nl < linhas && nc >= 0 && nc < colunas) {
                vizinhas.push_back(&getPosicao(nl, nc));
            }
        }
    }
    return vizinhas;
}