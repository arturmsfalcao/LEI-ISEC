#include "LeitorComandos.h"
#include "Cacto.h"
#include "ErvaDaninha.h"
#include "Roseira.h"
#include "PlantaExotica.h"
#include "Simulador.h"
#include "Jardim.h"
#include "Ferramenta.h"
#include "Regador.h"
#include "PacoteAdubo.h"
#include "TesouraPoda.h"
#include "FerramentaZ.h"
#include "Settings.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

using namespace std;

bool converterPosicao(const std::string& pos, int maxLinhas, int maxColunas, int& linha, int& coluna) {
    if (pos.size() != 2 || !std::isalpha(pos[0]) || !std::isalpha(pos[1])) {
        return false;
    }

    linha = std::toupper(pos[0]) - 'A';
    coluna = std::toupper(pos[1]) - 'A';

    if (linha < 0 || linha >= maxLinhas || coluna < 0 || coluna >= maxColunas) {
        return false;
    }
    return true;
}

void LeitorComandos::iniciar() {
    string linha;
    string comando;

    do {
        cout << "> ";
        getline(cin, linha);

        if (linha.empty())
            continue;

        istringstream buffer_saida(linha);
        buffer_saida >> comando;

        if (comando != "fim") {
            processarLinha(linha);
        }

    } while (comando != "fim");
}

void LeitorComandos::processarLinha(const std::string &linha_str) {
    if (linha_str.empty()) return;

    istringstream buffer_entrada(linha_str);
    string comando;
    buffer_entrada >> comando;

    string pos, tipo, nome_ficheiro, nome_copia;
    int l, c, n, raio, l_coor, c_coor, indice;
    char tipo_char;

    if (comando == "jardim") {
        if (buffer_entrada >> l >> c) {
            if (l >= 1 && l <= 26 && c >= 1 && c <= 26) {
                if (simulador.getJardim() != nullptr) {
                    cout << "[ERRO] O Jardim ja foi criado. Comando 'jardim' so pode ser executado uma vez.\n";
                    return;
                }
                simulador.criar(l, c);
            } else {
                cout << "[ERRO] Sintaxe: Maximo de 26 linhas e 26 colunas\n";
            }
        } else {
            cout << "[ERRO] Sintaxe: jardim <linhas> <colunas>\n";
        }
    }
    else if (comando == "executa") {
        if (buffer_entrada >> nome_ficheiro) {
            ifstream ficheiro(nome_ficheiro);

            if (!ficheiro.is_open()) {
                cout << "[ERRO] Erro: Nao foi possivel abrir o ficheiro '" << nome_ficheiro << "'.\n";
            } else {
                cout << "[INFO] A executar comandos do ficheiro: " << nome_ficheiro << "\n";
                string comando_ficheiro;

                while (getline(ficheiro, comando_ficheiro)) {
                    if (comando_ficheiro.empty()) continue;
                    cout << "> " << comando_ficheiro << endl;
                    processarLinha(comando_ficheiro);
                }
                cout << "[INFO] Execucao do ficheiro '" << nome_ficheiro << "' terminada.\n";
            }
        } else {
            cout << " [ERRO] Sintaxe: executa <nome-do-ficheiro>\n";
        }
    }
    else if (comando == "avanca") {
        n = 1;
        if (!simulador.getJardim()) {
            cout << "[ERRO] O Jardim ainda nao foi criado. Use o comando 'jardim'.\n";
            return;
        }

        if (buffer_entrada >> n) {
            if (n <= 0) {
                 cout << "[ERRO]: O numero de instantes deve ser positivo.\n";
                 return;
            }
        }

        for (int t = 0; t < n; t++) {
            simulador.passarInstante();
        }
        simulador.mostrar();
    }
    else if (comando == "lplantas") {
        if (!simulador.getJardim()) {
            cout << "[ERRO] O Jardim nao existe.\n";
            return;
        }
        simulador.listarPlantas();
    }
    else if (comando == "lplanta") {
        if (buffer_entrada >> pos) {
            Jardim* j = simulador.getJardim();
            if (!j) {
                cout << "[ERRO] O Jardim nao existe.\n";
                return;
            }

            if (!converterPosicao(pos, j->getLinhas(), j->getColunas(), l_coor, c_coor)) {
                cout << "[ERRO] Posicao invalida ou fora do jardim.\n";
                return;
            }

            simulador.listarPropriedadesPlanta(l_coor, c_coor);
        } else {
            cout << " [ERRO] Sintaxe: lplanta <lc>\n";
        }
    }
    else if (comando == "larea") {
        if (!simulador.getJardim()) {
            cout << "[ERRO] O Jardim nao existe.\n";
            return;
        }
        simulador.listarArea();
    }
    else if (comando == "lsolo") {
        raio = 0;

        if (buffer_entrada >> pos) {
            Jardim* j = simulador.getJardim();
            if (!j) {
                cout << "[ERRO] O Jardim nao existe.\n";
                return;
            }

            if (!converterPosicao(pos, j->getLinhas(), j->getColunas(), l_coor, c_coor)) {
                cout << "[ERRO] Posicao central invalida ou fora do jardim.\n";
                return;
            }

            if (buffer_entrada >> raio && raio < 0) {
                 cout << "[ERRO] O raio deve ser um numero nao negativo.\n";
                 return;
            }

            int min_l = std::max(0, l_coor - raio);
            int max_l = std::min(j->getLinhas() - 1, l_coor + raio);
            int min_c = std::max(0, c_coor - raio);
            int max_c = std::min(j->getColunas() - 1, c_coor + raio);

            std::cout << "\n--- LISTAGEM DE SOLO (Raio: " << raio << ") ---\n";

            for (int l_i = min_l; l_i <= max_l; ++l_i) {
                for (int c_i = min_c; c_i <= max_c; ++c_i) {
                    Posicao& targetPos = j->getPosicao(l_i, c_i);
                    targetPos.listarRecursos();
                }
            }
            std::cout << "--- FIM DA LISTAGEM DE SOLO ---\n";

        } else {
            cout << " [ERRO] Sintaxe: lsolo <lc> [n]\n";
        }
    }
    else if (comando == "lferr") {
        simulador.getJardineiro().listarFerramentas();
    }
    else if (comando == "colhe") {
        if (buffer_entrada >> pos) {
            Jardim* j = simulador.getJardim();
            if (!j) {
                cout << "[ERRO] O Jardim nao existe.\n";
                return;
            }

            if (!converterPosicao(pos, j->getLinhas(), j->getColunas(), l_coor, c_coor)) {
                cout << "[ERRO] Posicao invalida ou fora do jardim.\n";
                return;
            }

            simulador.getJardineiro().colhe(j->getPosicao(l_coor, c_coor));
            simulador.mostrar();
        } else {
            cout << " [ERRO] Sintaxe: colhe <lc>\n";
        }
    }
    else if (comando == "planta") {
        if (buffer_entrada >> pos >> tipo) {
            Jardim* j = simulador.getJardim();
            if (!j) {
                cout << "[ERRO] O Jardim nao existe.\n";
                return;
            }

            if (!simulador.getJardineiro().podePlantar()) {
                 cout << "[INFO] Limite de plantacoes (max " << Settings::Jardineiro::max_plantacoes << ") por instante esgotado.\n";
                 return;
            }

            if (!converterPosicao(pos, j->getLinhas(), j->getColunas(), l_coor, c_coor)) {
                cout << "[ERRO] Posicao invalida ou fora do jardim.\n";
                return;
            }

            Posicao& targetPos = j->getPosicao(l_coor, c_coor);

            if (targetPos.getPlanta() != nullptr) {
                cout << "[ERRO] A posicao " << pos << " ja esta ocupada por outra planta ('"
                     << targetPos.getPlanta()->getRepresentacao() << "').\n";
                return;
            }

            Planta* p = nullptr;
            tipo_char = toupper(tipo[0]);

            switch (tipo_char) {
                case 'C': p = new Cacto(); break;
                case 'E': p = new ErvaDaninha(); break;
                case 'R': p = new Roseira(); break;
                case 'X': p = new PlantaExotica(); break;
                default:
                    std::cout << "[ERRO] Tipo desconhecido: " << tipo << " (Usar C, E, R, X)\n";
                    return;
            }

            if (p) {
                simulador.getJardineiro().incrementaPlantacoes();
                targetPos.setPlanta(p);
                cout << "[INFO] Planta tipo '" << tipo_char << "' colocada em " << pos << ".\n";
                simulador.mostrar();
            }
        } else {
            std::cout << "[ERRO] Sintaxe: planta <lc> <tipo>\n";
        }
    }
    else if (comando == "larga") {
        if (!simulador.getJardim()) {
            std::cout << "[ERRO] O Jardim nao existe. Crie-o primeiro.\n";
            return;
        }
        simulador.getJardineiro().larga();
        simulador.mostrar();
    }
    else if (comando == "pega") {
        if (buffer_entrada >> indice) {
            simulador.getJardineiro().pega(indice);
            simulador.mostrar();
        } else {
            cout << " [ERRO] Sintaxe: pega <n>\n";
        }
    }
    else if (comando == "compra") {
        std::string tipo_str;

        if (buffer_entrada >> tipo_str) {
            tipo_char = tolower(tipo_str[0]);

            Ferramenta* f = nullptr;

            if (tipo_char == 'g') f = new Regador();
            else if (tipo_char == 'a') f = new PacoteAdubo();
            else if (tipo_char == 't') f = new TesouraPoda();
            else if (tipo_char == 'z') f = new FerramentaZ();

            if (f) {
                simulador.getJardineiro().adicionarFerramenta(f);
                cout << "[INFO] Ferramenta tipo '" << tipo_char << "' comprada (Serie " << f->getNumeroSerie() << ").\n";
                simulador.mostrar();
            } else {
                cout << "[ERRO] Tipo de ferramenta desconhecido: " << tipo_char << " (Usar g, a, t, z)\n";
            }
        } else {
            cout << " [ERRO] Sintaxe: compra <c>\n";
        }
    }
    else if (comando == "e" || comando == "d" || comando == "c" || comando == "b") {
        Jardim* j = simulador.getJardim();
        if (!j) {
            cout << "[ERRO] O Jardim nao existe.\n";
            return;
        }

        Jardineiro& jardineiro = simulador.getJardineiro();

        if (!jardineiro.estaDentro()) {
            cout << "[ERRO] Jardineiro nao esta no jardim.\n";
            return;
        }

        bool moveu = jardineiro.move(comando[0], j->getLinhas(), j->getColunas());

        if (moveu) {
            int l_depois = jardineiro.getLinha();
            int c_depois = jardineiro.getColuna();
            Posicao& novaPos = j->getPosicao(l_depois, c_depois);

            bool tinhaFerramenta = (novaPos.getFerramenta() != nullptr);
            simulador.getJardineiro().apanhaFerramenta(novaPos);

            if (tinhaFerramenta) {
                simulador.gerarFerramentaAleatoria(false);
            }

            simulador.mostrar();
        }
    }
    else if (comando == "entra") {
        if (buffer_entrada >> pos) {
            Jardim* j = simulador.getJardim();
            if (!j) {
                cout << "[ERRO] O Jardim nao existe.\n";
                return;
            }

            if (!simulador.getJardineiro().podeEntrarSair()) {
                cout << "[INFO] Limite de entradas/saidas por instante esgotado.\n";
                return;
            }

            if (!converterPosicao(pos, j->getLinhas(), j->getColunas(), l_coor, c_coor)) {
                cout << "[ERRO] Posicao invalida ou fora do jardim.\n";
                return;
            }

            simulador.getJardineiro().entra(l_coor, c_coor);

            if (simulador.getJardineiro().estaDentro()) {
                Posicao& targetPos = j->getPosicao(l_coor, c_coor);
                bool tinhaFerramenta = (targetPos.getFerramenta() != nullptr);

                simulador.getJardineiro().apanhaFerramenta(targetPos);

                if (tinhaFerramenta) {
                    simulador.gerarFerramentaAleatoria(false);
                }
            }

            simulador.mostrar();
        } else {
            cout << " [ERRO] Sintaxe: entra <lc>\n";
        }
    }
    else if (comando == "sai") {
        if (!simulador.getJardim()) {
            cout << "[ERRO] O Jardim nao existe.\n";
            return;
        }
        if (!simulador.getJardineiro().podeEntrarSair()) {
            cout << "[INFO] Limite de entradas/saidas por instante esgotado.\n";
            return;
        }
        if (!simulador.getJardineiro().estaDentro()) {
            cout << "[ERRO] Jardineiro ja esta fora do jardim.\n";
            return;
        }

        simulador.getJardineiro().sai();
        simulador.mostrar();
    }
    else if (comando == "grava") {
        if (buffer_entrada >> nome_copia) {
            simulador.gravar(nome_copia);
        } else {
            cout << "[ERRO] Sintaxe: grava <nome_da_copia>\n";
        }
    }
    else if (comando == "recupera") {
        if (buffer_entrada >> nome_copia) {
            simulador.recuperar(nome_copia);
        } else {
            cout << "[ERRO] Sintaxe: recupera <nome_da_copia>\n";
        }
    }
    else if (comando == "apaga") {
        if (buffer_entrada >> nome_copia) {
            simulador.apagar(nome_copia);
        } else {
            cout << "[ERRO] Sintaxe: apaga <nome_da_copia>\n";
        }
    }
    else if (comando != "fim") {
        cout << "[ERRO] Comando Desconhecido: " << comando << "\n";
    }
}