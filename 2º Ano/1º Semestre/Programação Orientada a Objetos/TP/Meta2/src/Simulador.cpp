#include "Simulador.h"
#include "Roseira.h"
#include "ErvaDaninha.h"
#include "PlantaExotica.h"
#include "Cacto.h"
#include "Visualizador.h"
#include "Regador.h"
#include "PacoteAdubo.h"
#include "TesouraPoda.h"
#include "FerramentaZ.h"
#include <iostream>

void Simulador::criar(int l, int c) {
    delete jardim;
    jardim = new Jardim(l, c);

    for (int i = 0; i < 3; ++i) {
        gerarFerramentaAleatoria(true);
    }

    std::cout << "Jardim " << l << "x" << c << " criado:\n";
    mostrar();
}

void Simulador::mostrar() const {
    if (jardim) {
        Visualizador::mostrar(*jardim, jardineiro);
    }
    else
        std::cout << "[ERRO] Jardim nao existe!\n";
}

void destruirPlanta(Planta* planta, Posicao& pos) {
    if (planta) {
        planta->liberarRecursos(pos);
        delete planta;
        pos.setPlanta(nullptr);
    }
}

void Simulador::passarInstante() {
    if (!jardim) return;

    jardineiro.resetLimites();

    int linhas = jardim->getLinhas();
    int colunas = jardim->getColunas();

    if (jardineiro.estaDentro()) {
        int l = jardineiro.getLinha();
        int c = jardineiro.getColuna();
        Posicao& posJardineiro = jardim->getPosicao(l, c);

        jardineiro.usaFerramentaAtiva(posJardineiro);
    }

    for (int l = 0; l < linhas; ++l) {
        for (int c = 0; c < colunas; ++c) {
            Posicao& pos = jardim->getPosicao(l, c);
            Planta* planta = pos.getPlanta();

            if (planta) {
                planta->passarInstante(pos);

                Roseira* r = dynamic_cast<Roseira*>(planta);
                if (r && r->deveMorrerPorSufocamento(jardim, l, c)) {
                    std::cout << "Roseira em (" << l << "," << c << ") morreu por sufocamento.\n";
                    destruirPlanta(planta, pos);
                    continue;
                }

                if (!planta->estaVivo()) {
                    std::cout << "Planta (" << planta->getRepresentacao() << ") em (" << l << "," << c << ") morreu.\n";
                    destruirPlanta(planta, pos);
                    continue;
                }

                ErvaDaninha* e = dynamic_cast<ErvaDaninha*>(planta);
                if (e && e->podeMultiplicar()) {
                    if (e->tentarMultiplicar(jardim, l, c)) {
                        std::cout << "Erva Daninha invadiu uma posicao vizinha.\n";
                    }
                }

                Cacto* cacto = dynamic_cast<Cacto*>(planta);
                if (cacto && cacto->podeMultiplicar()) {
                    if (cacto->tentarMultiplicar(jardim, l, c)) {
                         std::cout << "Cacto em multiplicou-se para uma posicao vazia.\n";
                    }
                }

                if (r && r->podeMultiplicar()) {
                    if (r->tentarMultiplicar(jardim, l, c)) {
                        std::cout << "Roseira em multiplicou-se para uma posicao vazia.\n";
                    }
                }

                PlantaExotica* x = dynamic_cast<PlantaExotica*>(planta);
                if (x) {
                    std::vector<Posicao*> vizinhas = jardim->getVizinhas(l, c);
                    for (Posicao* vizinha : vizinhas) {
                        vizinha->adicionarAgua(Settings::PlantaExotica::doacao_agua_vizinha);
                    }
                }
            }
        }
    }
    if (jardineiro.estaDentro()) {
        Ferramenta* ativa = jardineiro.getFerramentaNaMao();

        if (ativa != nullptr && ativa->getSimbolo() == 'z') {
            FerramentaZ* iman = dynamic_cast<FerramentaZ*>(ativa);
            if (iman != nullptr) {
                int totalApanhadas = iman->usar(*jardim, jardineiro);

                for (int i = 0; i < totalApanhadas; ++i) {
                    this->gerarFerramentaAleatoria(false);
                }
            }
        }
    }
}

Ferramenta* criarFerramentaAleatoria() {
    int tipo = std::rand() % 4;

    switch (tipo) {
        case 0: return new Regador();
        case 1: return new PacoteAdubo();
        case 2: return new TesouraPoda();
        case 3: return new FerramentaZ();
        default: return nullptr;
    }
}

void Simulador::gerarFerramentaAleatoria(bool inicial) {
    if (!jardim) return;

    int linhas = jardim->getLinhas();
    int colunas = jardim->getColunas();

    int l, c;

    for (int i = 0; i < 100; ++i) {
        l = std::rand() % linhas;
        c = std::rand() % colunas;

        Posicao& pos = jardim->getPosicao(l, c);

        if (!pos.getFerramenta()) {
            Ferramenta* novaFerramenta = criarFerramentaAleatoria();
            if (novaFerramenta) {
                pos.setFerramenta(novaFerramenta);
                std::cout << "[INFO] Nova Ferramenta '" << novaFerramenta->getSimbolo()
                          << "' (Serie " << novaFerramenta->getNumeroSerie()
                          << ") colocada em: " << (char)('A' + l) << (char)('A' + c)
                          << (inicial ? " (Inicial)." : " (Por magia).") << "\n";
                return;
            }
        }
    }
}

void Simulador::listarPlantas() const {
    if (!jardim) return;

    int linhas = jardim->getLinhas();
    int colunas = jardim->getColunas();
    bool encontrou_planta = false;

    std::cout << "\n--- LISTAGEM DE PLANTAS E SOLO ---\n";

    for (int l = 0; l < linhas; ++l) {
        for (int c = 0; c < colunas; ++c) {
            const Posicao& pos = jardim->getPosicao(l, c);
            Planta* planta = pos.getPlanta();

            if (planta) {
                encontrou_planta = true;
                pos.listarRecursos();
                planta->listarPropriedades();
                std::cout << "\n----------------------------------------\n";
            }
        }
    }

    if (!encontrou_planta) {
        std::cout << "[INFO] Nenhuma planta encontrada no Jardim.\n";
    }

    std::cout << "--- FIM DA LISTAGEM DE PLANTAS ---\n";
}

void Simulador::listarPropriedadesPlanta(int linha, int coluna) const {
    if (!jardim) {
        std::cout << "[ERRO] O Jardim nao existe.\n";
        return;
    }

    const Posicao& pos = jardim->getPosicao(linha, coluna);
    Planta* planta = pos.getPlanta();

    if (planta) {
        pos.listarRecursos();
        planta->listarPropriedades();
        std::cout << "\n----------------------------------------\n";
    } else {
        std::cout << "[INFO] Nenhuma planta encontrada na posicao "
                  << (char)('A' + linha) << (char)('A' + coluna)
                  << ".\n";
    }
}

void Simulador::listarArea() const {
    if (!jardim) return;

    int linhas = jardim->getLinhas();
    int colunas = jardim->getColunas();
    bool encontrou_conteudo = false;

    std::cout << "\n--- LISTAGEM DE AREA (CONTEUDO NAO VAZIO) ---\n";

    for (int l = 0; l < linhas; ++l) {
        for (int c = 0; c < colunas; ++c) {
            const Posicao& pos = jardim->getPosicao(l, c);
            Planta* planta = pos.getPlanta();
            Ferramenta* ferramenta = pos.getFerramenta();

            bool jardineiro_aqui = jardineiro.estaDentro() &&
                                   jardineiro.getLinha() == l &&
                                   jardineiro.getColuna() == c;

            if (planta || ferramenta || jardineiro_aqui) {
                encontrou_conteudo = true;
                pos.listarRecursos();
                std::cout << "Conteudo na Posicao:\n";

                if (jardineiro_aqui) {
                    std::cout << "  - Jardineiro (*) (Prioridade de visualizacao mais alta)\n";
                }

                if (planta) {
                    std::cout << "  - Planta ('" << planta->getRepresentacao() << "'):\n";
                    planta->listarPropriedades();
                }

                if (ferramenta) {
                    std::cout << "  - Ferramenta no Solo ('" << ferramenta->getSimbolo() << "'):\n";
                    ferramenta->imprimirDetalhes();
                    std::cout << "\n";
                }

                std::cout << "----------------------------------------\n";
            }
        }
    }

    if (!encontrou_conteudo) {
        std::cout << "[INFO] Nenhuma posicao com conteudo (planta, ferramenta ou jardineiro).\n";
    }

    std::cout << "--- FIM DA LISTAGEM DE AREA ---\n";
}

void Simulador::gravar(const std::string& nome) {
    if (!jardim) {
        std::cout << "[ERRO] Nao existe Jardim para gravar (use 'cria').\n";
        return;
    }
    if (copias.count(nome)) {
        std::cout << "[ERRO] Ja existe uma copia gravada com o nome '" << nome << "'. Apague-a primeiro.\n";
        return;
    }

    EstadoJardim* novoEstado = new EstadoJardim();
    novoEstado->jardimCopia = new Jardim(*jardim);
    novoEstado->jardineiroCopia = jardineiro;

    copias[nome] = novoEstado;
    std::cout << "[INFO] Estado do Jardim gravado com sucesso com o nome: " << nome << "\n";
}

void Simulador::recuperar(const std::string& nome) {
    if (copias.find(nome) == copias.end()) {
        std::cout << "[ERRO] Copia com o nome '" << nome << "' nao encontrada.\n";
        return;
    }

    EstadoJardim* estado = copias[nome];
    delete jardim;

    jardim = estado->jardimCopia;
    jardineiro = estado->jardineiroCopia;

    estado->jardimCopia = nullptr;
    delete estado;
    copias.erase(nome);

    std::cout << "[INFO] Estado '" << nome << "' recuperado. A copia foi eliminada.\n";
    mostrar();
}

void Simulador::apagar(const std::string& nome) {
     if (copias.find(nome) == copias.end()) {
        std::cout << "[ERRO] Copia com o nome '" << nome << "' nao encontrada.\n";
        return;
    }

    delete copias[nome];
    copias.erase(nome);
    std::cout << "[INFO] Copia '" << nome << "' apagada com sucesso.\n";
}