#include "PlantaExotica.h"
#include "Posicao.h"
#include <algorithm>
#include <iostream>

PlantaExotica::PlantaExotica() {
    agua = Settings::PlantaExotica::inicial_agua;
    nutrientes = Settings::PlantaExotica::inicial_nutrientes;
}

PlantaExotica::PlantaExotica(const PlantaExotica& outra) : Planta(outra) {}

bool PlantaExotica::estaVivo() const {
    return nutrientes > 0;
}

void PlantaExotica::passarInstante(Posicao &pos) {
    double nutrientes_absorver = std::min((double)pos.getNutrientes(), (double)Settings::PlantaExotica::absorcao_nutrientes);
    pos.setNutrientes(pos.getNutrientes() - (int)nutrientes_absorver);
    nutrientes += (int)nutrientes_absorver;

    double agua_absorver = std::min((double)pos.getAgua(), (double)Settings::PlantaExotica::absorcao_agua);
    pos.setAgua(pos.getAgua() - (int)agua_absorver);
    agua += (int)agua_absorver;
}

Planta* PlantaExotica::clone() const {
    return new PlantaExotica(*this);
}

void PlantaExotica::listarPropriedades() const {
    std::cout << "\n--- PROPRIEDADES DA PLANTA EXOTICA ['x'] ---\n";
    std::cout << "Beleza: " << getBeleza() << std::endl;
    std::cout << "Reserva Agua: " << agua << "\n";
    std::cout << "Reserva Nutrientes: " << nutrientes << "\n";
    std::cout << "--- FIM PROPRIEDADES ---\n";
}