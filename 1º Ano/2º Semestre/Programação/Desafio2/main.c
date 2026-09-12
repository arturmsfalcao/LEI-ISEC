#include <stdio.h>
#include "voo.h"

int main(void) {

    voo tab1[5] = {
        {"A1","Lisboa", "Porto", {3,4}, 23},
        {"A2","Porto", "Lisboa", {3,4}, 3},
        {"ABC","Lisboa", "Madrid", {2,4}, 13},
        {"XY","Madrid", "Porto", {13,4}, 1},
        {"AB23","Lisboa", "Porto", {12,4}, 100}};

    voo tab2[3] = {
        {"123","Lisboa", "Madrid", {12,9}, 1},
        {"456","Lisboa", "Madrid", {12,9}, 3},
        {"789","Lisboa", "Madrid", {12,10}, 1}};

    voo tab3[6] = {
        {"C1","Lisboa", "Paris", {1,1}, 12},
        {"C2","Lisboa", "Paris", {1,2}, 30},
        {"B1","Porto", "Munique", {2,1}, 35},
        {"Z2","Lisboa", "Paris", {2,2}, 1},
        {"XY","Munique", "Porto", {3,1}, 1},
        {"B2","Lisboa", "Porto", {12,1}, 1}};


    // Devolve 1 e troca as posicoes dos voos XY e A1
    printf("%d\n", desafio2(tab1, 5, "Lisboa", "Porto", (data){3, 3}, (data){11, 4}));
    printTab(tab1, 5);

    // Devolve 2 e nao troca posicoes de voos
    printf("%d\n", desafio2(tab2, 3, "Lisboa", "Madrid", (data){12, 3}, (data){11, 10}));
    printTab(tab2, 3);

    // Devolve 2 e trcoa as posicoes dos voos B2 e C1
    printf("%d\n", desafio2(tab3, 6, "Lisboa", "Paris", (data){1, 1}, (data){1, 2}));
    printTab(tab3, 6);

    return 0;
}