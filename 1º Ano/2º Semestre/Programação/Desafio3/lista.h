

#ifndef DESAFIO3_LISTA_H
#define DESAFIO3_LISTA_H

typedef struct drone nodr, *pnodr;

struct drone{
    char id[10];	// identificador do drone
    int bat;		// nivel da bateria do drone
    pnodr prox;		// proximo drone da lista
};

void eliminaLista(pnodr lista);

pnodr criaLista(nodr tab[], int tam);

void mostraLista(pnodr lista);

pnodr desafio3(pnodr lista);

#endif //DESAFIO3_LISTA_H
