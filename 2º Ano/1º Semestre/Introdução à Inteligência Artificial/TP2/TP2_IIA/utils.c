#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

// Inicializa o gerador
void init_rand() {
    srand((unsigned) time(NULL));
}

// Inteiro aleatório [min, max]
int random_l_h(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Real [0,1]
float rand_01() {
    return ((float) rand()) / RAND_MAX;
}

// Copia solução b → a
void substitui(int a[], int b[], int n) {
    for (int i = 0; i < n; i++)
        a[i] = b[i];
}

// Escreve solução
void escreve_sol(int sol[], int n) {
    printf("Selecionados: ");
    for(int i=0;i<n;i++)
        if(sol[i] == 1)
            printf("%d ", i);
    printf("\n");
}
