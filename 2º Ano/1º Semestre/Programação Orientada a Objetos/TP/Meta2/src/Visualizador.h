#ifndef VISUALIZADOR_H
#define VISUALIZADOR_H

class Jardim;
class Jardineiro;

class Visualizador {
public:
    static void mostrar(const Jardim& jardim, const Jardineiro& jardineiro);
};

#endif