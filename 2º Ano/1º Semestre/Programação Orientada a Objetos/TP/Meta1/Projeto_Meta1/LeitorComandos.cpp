#include "LeitorComandos.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

void LeitorComandos::iniciar() {
    string linha, comando;

    do {
        cout << "> ";
        getline(cin, linha);

        if (linha.empty())
            continue;

        istringstream buffer_entrada(linha);
        buffer_entrada >> comando;

        // === Comandos base ===
        if (comando == "sai");
        else if (comando == "jardim") {
            int l, c;
            if (buffer_entrada >> l >> c) {
                if (l >= 1 && l <= 26 && c >= 1 && c <= 26) {
                    simulador.criar(l, c);
                } else {
                    cout << "[ERRO] Sintaxe: Maximo de 26 linhas e 26 colunas\n";
                }
            } else {
                cout << "[ERRO] Sintaxe: jardim <linhas> <colunas>\n";
            }
        }
        else if (comando == "executa") {
            string nome;
            if (buffer_entrada >> nome) {
                ifstream ficheiro(nome);
                if (!ficheiro.is_open()) {
                    cout << "Erro: não foi possível abrir o ficheiro.\n";
                } else {
                    cout << "Correto\n";
                }
            } else {
                cout << " [ERRO] Sintaxe: executa <ficheiro>\n";
            }
        }

        // === Comando de tempo ===
        else if (comando == "avanca") {
            int n = 1;
            if (buffer_entrada >> n) {
                if (n > 0)
                    cout << "Comando Correto:" << comando << " (" << n << " instantes)\n";
                else
                    cout << "[ERRO]: Tem que ser positivo.\n";
            } else {
                cout << " [ERRO] Sintaxe: avanca [n]\n";
            }
        }

        // === Listagem de informação ===
        else if (comando == "lplantas") {
            cout << "Comando Correto:" << comando << "\n";
        }
        else if (comando == "lplanta") {
            string pos;
            if (buffer_entrada >> pos)
                cout << "Comando Correto:" << comando << " (" << pos << ")\n";
            else
                cout << " [ERRO] Sintaxe: lplanta <lc>\n";
        }
        else if (comando == "larea") {
            cout << "Comando Correto:" << comando << "\n";
        }
        else if (comando == "lsolo") {
            string pos;
            int n = -1;
            if (buffer_entrada >> pos) {
                if (buffer_entrada >> n)
                    cout << "Comando Correto:" << comando << " (" << pos << ", raio " << n << ")\n";
                else
                    cout << "Comando Correto:" << comando << " (" << pos << ")\n";
            } else {
                cout << " [ERRO] Sintaxe: lsolo <lc> [n]\n";
            }
        }
        else if (comando == "lferr") {
            cout << "Comando Correto:" << comando << "\n";
        }

        // === Ações ===
        else if (comando == "colhe") {
            string pos;
            if (buffer_entrada >> pos)
                cout << "Comando Correto:" << comando << " (" << pos << ")\n";
            else
                cout << " [ERRO] Sintaxe: colhe <lc>\n";
        }
        else if (comando == "planta") {
            string pos, tipo;
            if (buffer_entrada >> pos >> tipo)
                cout << "Comando Correto:" << comando << " (" << pos << ", tipo " << tipo << ")\n";
            else
                cout << " [ERRO] Sintaxe: planta <lc> <tipo>\n";
        }
        else if (comando == "larga") {
            cout << "Comando Correto:" << comando << "\n";
        }
        else if (comando == "pega") {
            int n;
            if (buffer_entrada >> n)
                cout << "Comando Correto:" << comando << " (" << n << ")\n";
            else
                cout << " [ERRO] Sintaxe: pega <n>\n";
        }
        else if (comando == "compra") {
            char tipo;
            if (buffer_entrada >> tipo)
                cout << "Comando Correto:" << comando << " (" << tipo << ")\n";
            else
                cout << " [ERRO] Sintaxe: compra <c>\n";
        }

        // === Movimento ===
        else if (comando == "e" || comando == "d" || comando == "c" || comando == "b") {
            cout << "Movimento '" << comando << "' Correto\n";
        }
        else if (comando == "entra") {
            string pos;
            if (buffer_entrada >> pos)
                cout << "Comando Correto:" << comando << " (" << pos << ")\n";
            else
                cout << " [ERRO] Sintaxe: entra <lc>\n";
        }

        // === Comando desconhecido ===
        else {
            cout << "[ERRO] Comando Desconhecido: " << comando << "\n";
        }

    } while (comando != "sai");
}
