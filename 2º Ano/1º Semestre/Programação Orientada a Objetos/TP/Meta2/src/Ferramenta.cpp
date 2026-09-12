#include "Ferramenta.h"

int Ferramenta::contadorSerie = 0;

Ferramenta::Ferramenta() {
    contadorSerie++;
    numeroSerie = contadorSerie;
}