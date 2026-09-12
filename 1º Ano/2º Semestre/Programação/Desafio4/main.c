#include <stdio.h>

#include "parque.h"

int main(){
    int v1 = 0, v2 = 0;
    pCliente l1 = criaExemploED((cliente []){{13, 2, NULL, NULL},{17,1,NULL,NULL},{22,3,NULL,NULL}},
                                (acesso[] ){{{10,10},{10,15},NULL}, {{14,30},{17,2},NULL}, {{10,50},{-1,-1},NULL},
                                 {{9,11},{9,12},NULL},{{10,5},{12,0},NULL},{{14,33},{15,30},NULL}},
                                3);


    pCliente l2 = criaExemploED((cliente []){{13, 1, NULL, NULL},{17,2,NULL,NULL},{22,2,NULL,NULL},{25,1,NULL,NULL}},
                                (acesso[] ){{{8,20},{8,55},NULL}, {{9,30},{9,40},NULL}, {{10,50},{14,1},NULL},
                                            {{9,11},{9,12},NULL},{{10,5},{10,50},NULL},{{14,33},{14,34},NULL}},
                                4);

    // O exemplo 1 deve escrever ID: 22
    // Deve eliminar o primeiro acesso do cliente 13
    if (l1 != NULL) {
        printf("Exemplo 1: \n");
        mostraTudo(l1);
        l1 = desafio4(l1, &v1);
        printf("ID: %d\nDepois da Funcao\n", v1);
        mostraTudo(l1);
        libertaTudo(l1);
    }

    // O exemplo 2 deve escrever ID: 22
    // Não elimina nenhum acesso do cliente 13
    if (l2 != NULL) {
        printf("\nExemplo 2: \n");
        mostraTudo(l2);
        l2 = desafio4(l2, &v2);
        printf("ID: %d\nDepois da Funcao\n", v2);
        mostraTudo(l2);
        libertaTudo(l2);
    }
    return 0;
}