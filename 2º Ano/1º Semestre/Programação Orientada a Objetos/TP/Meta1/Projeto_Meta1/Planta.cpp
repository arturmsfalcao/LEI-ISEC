#include "Planta.h"

// --- Planta ---
Planta::Planta() : agua(0), nutrientes(0), tempoVivo(0) {}
Planta::~Planta() {}

// --- Cacto ---
Cacto::Cacto() : Planta(), instantesAguaAlta(0), instantesSemNutrientes(0) {}
char Cacto::getSimbolo() const { return 'c'; }
void Cacto::passarInstante(Posicao &pos) {}
bool Cacto::morreu() const { return false; }

// --- Roseira ---
Roseira::Roseira() : Planta() {}
char Roseira::getSimbolo() const { return 'r'; }
void Roseira::passarInstante(Posicao &pos) {}
bool Roseira::morreu() const { return false; }

// --- ErvaDaninha ---
ErvaDaninha::ErvaDaninha() : Planta(), contadorInstantes(0), tempoUltimaMultiplicacao(0) {}
char ErvaDaninha::getSimbolo() const { return 'e'; }
void ErvaDaninha::passarInstante(Posicao &pos) {}
bool ErvaDaninha::morreu() const { return false; }

// --- PlantaExotica ---
AgataAzul::AgataAzul() : Planta() {}
char AgataAzul::getSimbolo() const { return 'x'; }
void AgataAzul::passarInstante(Posicao &pos) {}
bool AgataAzul::morreu() const { return false; }
