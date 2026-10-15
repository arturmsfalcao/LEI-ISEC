#include "Ferramenta.h"

int Ferramenta::contadorSerie = 0;

// --- Ferramenta ---
Ferramenta::Ferramenta() {
    contadorSerie++;
    numeroSerie = contadorSerie;
}

// --- Regador ---
Regador::Regador() : Ferramenta(), aguaDisponivel(200) {}
char Regador::getSimbolo() const { return 'g'; }
void Regador::usar(Posicao &pos) {}

// --- Pacote de adubo ---
PacoteAdubo::PacoteAdubo() : Ferramenta(), quantidade(100) {}
char PacoteAdubo::getSimbolo() const { return 'a'; }
void PacoteAdubo::usar(Posicao &pos) {}

// --- Tesoura de poda ---
TesouraPoda::TesouraPoda() : Ferramenta() {}
char TesouraPoda::getSimbolo() const { return 't'; }
void TesouraPoda::usar(Posicao &pos) {}

// --- FerramentaZ ---
Pa::Pa() : Ferramenta() {}
char Pa::getSimbolo() const { return 'z'; }
void Pa::usar(Posicao &pos) {}
