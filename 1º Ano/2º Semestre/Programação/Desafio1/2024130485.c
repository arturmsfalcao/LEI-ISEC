// NOTAS IMPORTANTES

// 1. Altere o nome deste ficheiro. Deve passar a ter o seu numero de aluno como nome, mantendo a extensao .c
// Por exemplo, se o numero de aluno for 1234567, o ficheiro deve passar a chamar-se: 1234567.c

// 2. Complete a sua identificacao nas linhas 11 e 12

// 3. So pode escrever codigo dentro da funcao desafio1(). Nao pode alterar mais nada neste ficheiro.
// Esta funcao nao deve escrever nada na consola

// Nome completo: Artur Meireles de Sousa Falcão
// Numero de aluno: 2024130485


#include <stdio.h>

// Recebe: Endereco inicial de uma tabela de inteiros (v), dimensao da tabela (tam) e endereco de uma variavel inteira (dif)

// Calcula a maior diferenca absoluta entre elementos do vetor e coloca na variavel referenciada por dif
// Devolve a soma maxima de tres elementos consecutivos

int desafio1(int v[], int tam, int *dif) {
    int maior = v[0], menor = v[0], somaMax = v[0]+v[1]+v[2];

    for (int i=1; i<tam; i++){
        if (v[i] > maior)
            maior = v[i];
        else if (v[i] < menor)
            menor = v[i];
    }
    
    *dif = maior - menor;

    for (int j=1; j<tam-2; j++){
        if (v[j] + v[j+1] + v[j+2] > somaMax)
            somaMax = v[j] + v[j+1] + v[j+2];
    }
    
    return somaMax;
}


int main() {
    int tab1[9] = {5, 4, 8, 4, 4, 4, 12, 5, 3};

    int tab2[3] = {1, 2, 3};

    int tab3[10] = {10, 1, 4, 3, 4, 4, 3, 2, 2, 9};

    int tab4[5] = {1, 1, 1, 1, 1};

    int c1=0, c2=0, c3=0, c4 = 0, d1, d2, d3, d4;

    d1 = desafio1(tab1, 9, &c1);
    d2 = desafio1(tab2, 3, &c2);
    d3 = desafio1(tab3, 10, &c3);
    d4 = desafio1(tab4, 5, &c4);

    printf("Tab1: %d %d Tab2: %d %d Tab3: %d %d Tab4: %d %d\n", d1, c1, d2, c2, d3, c3, d4, c4);
    return 0;
}