#include <stdio.h>
#include "lista.h"


int main() {
    pnodr lista1=NULL, lista2=NULL, lista3=NULL;
    nodr dados1[]={{"XDR01",50, NULL},{"N3XUS",3, NULL},{"DRNX6",1, NULL},
            {"VYPR7",6, NULL},{"AERO9",7, NULL}, {"QBITX",40, NULL}};
    nodr dados2[]={{"XDR01",5, NULL}};
    nodr dados3[]={{"XDR01",50, NULL},{"N3XUS",300, NULL},{"DRNX6",1, NULL},
            {"VYPR7",6, NULL},{"AERO9",7, NULL}};

    // Exemplo 1
    // Lista Inicial: { XDR01-50, N3XUS-3, DRNX6-1,	VYPR7-6, AERO9-7, QBITX-40}
    // Lista Final: { XDR01-50, DRNX6-1, VYPR7-6, AERO9-7, QBITX-40 }

    lista1 = criaLista(dados1, 6);
    printf("\n\nLista 1 inicial: ");
    mostraLista(lista1);
    lista1 = desafio3(lista1);
    printf("\nLista 1 final:   ");
    mostraLista(lista1);

    // Exemplo 2
    // Lista Inicial: { XDR01-5 }
    // Lista Final: { XDR01-5 }
    lista2 = criaLista(dados2, 1);
    printf("\n\nLista 2 inicial: ");
    mostraLista(lista2);
    lista2 = desafio3(lista2);
    printf("\nLista 2 final:   ");
    mostraLista(lista2);

    // Exemplo 3
    // Lista Inicial: { XDR01-50, N3XUS-300, DRNX6-1,	VYPR7-6, AERO9-7 }
    // Lista Final: { XDR01-50, DRNX6-1, VYPR7-6, AERO9-7, N3XUS-300 }

    lista3 = criaLista(dados3, 5);
    printf("\n\nLista 3 inicial: ");
    mostraLista(lista3);
    lista3 = desafio3(lista3);
    printf("\nLista 3 final:   ");
    mostraLista(lista3);

    eliminaLista(lista1);
    eliminaLista(lista2);
    eliminaLista(lista3);

    return 0;
}
