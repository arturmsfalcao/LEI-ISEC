#include <iostream>
#include "LeitorComandos.h"

int main() {
    std::cout << "=== Simulador de Jardim ===" << std::endl;

    LeitorComandos leitor;
    leitor.iniciar();

    std::cout << "Programa terminado." << std::endl;
    return 0;
}
