#ifndef SIMULADOR_H
#define SIMULADOR_H

#include "Jardim.h"
#include "Jardineiro.h"
#include <map>
#include <string>

struct EstadoJardim {
    Jardim* jardimCopia = nullptr;
    Jardineiro jardineiroCopia;

    ~EstadoJardim() {
        delete jardimCopia;
    }
};

class Simulador {
    Jardim* jardim = nullptr;
    Jardineiro jardineiro;

    std::map<std::string, EstadoJardim*> copias;

public:
    ~Simulador() {
        delete jardim;
        for (auto const& [nome, estado] : copias) {
            delete estado;
        }
    }
    Jardim* getJardim() { return jardim; }
    const Jardim* getJardim() const { return jardim; }

    Jardineiro& getJardineiro() { return jardineiro; }
    const Jardineiro& getJardineiro() const { return jardineiro; }

    void criar(int l, int c);

    void mostrar() const;

    void passarInstante();

    void gerarFerramentaAleatoria(bool inicial);

    void listarPlantas() const;
    void listarPropriedadesPlanta(int linha, int coluna) const;

    void listarArea() const;

    void gravar(const std::string& nome);
    void recuperar(const std::string& nome);
    void apagar(const std::string& nome);
    void listarCopias() const;
};

#endif