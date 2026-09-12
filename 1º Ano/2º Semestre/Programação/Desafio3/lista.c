/* NOTAS IMPORTANTES

Devem efetuar as seguintes alteracoes:

a.	Alterar o nome do ficheiro, que deve passar a ser o do vosso numero de aluno e mantendo a extensao .c.
    Caso o vosso numero de aluno seja 20251234, devem submeter um ficheiro com nome 20251234.c

b.	Quando efetuarem a alteracao da alinea anterior, tenham cuidado para não alterar o nome dos
    ficheiros de cabeçalho (header files) que surgem no inicio do codigo.
    O refactoring do CLion pode alterar o nome dos ficheiros .h e isso nao pode acontecer no ficheiro que submeterem.
    Confiram a linha 35 do ficheiro que deve continuar a ser #include "lista.h".

c.	Após concluirem a submissao, confirmem que o ficheiro tem o nome correto.
    Se não cumprirem as regras das alineas a) e b) ficarao sem nota atribuida

d.	Completar a vossa identificaca nas linhas 28 e 29 do ficheiro.

e.	Escrever a funcao desafio3(), de acordo com o que é solicitado no enunciado.
    Esta funcao não deve escrever nada na consola, efetuando apenas as operacoes descritas no enunciado.
    Caso julgue relevante, pode criar funçoes auxiliares dentro deste ficheiro .c.

f.	Os restantes ficheiros do projeto nao podem ser alterados.
    Deve garantir que o ficheiro que entrega continua compativel com o resto do projeto.
*/



// Nome completo: Artur Meireles de Sousa Falcão
// Número de aluno: 2024130485

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lista.h"


void eliminaLista(pnodr lista){
    pnodr aux;

    while(lista != NULL){
        aux = lista;
        lista = lista->prox;
        free(aux);
    }
}

pnodr criaLista(nodr tab[], int tam){
    int i;
    pnodr lista=NULL, novo;

    for(i=tam-1; i>=0; i--){
        novo = malloc(sizeof(nodr));
        if(novo == NULL){
            eliminaLista(lista);
            return NULL;
        }
        *novo = tab[i];
        novo->prox = lista;
        lista = novo;
    }
    return lista;
}

void mostraLista(pnodr lista){
    printf("{ ");
    while(lista != NULL){
        printf("%s-%d", lista->id, lista->bat);
        lista = lista->prox;
        if(lista!=NULL)
            printf(", ");
    }
    printf(" }");
}



// Se achar necessario, pode escrever aqui funcoes auxiliares







// Final da seccao com funcoes auxiliares

// Escreva a funcao desafio3(), de acordo com o descrito no enunciado

pnodr desafio3(pnodr lista){
    //regra 1
    if (lista == NULL || lista->prox == NULL) return lista;

    int cont = 0;
    pnodr atual = lista, anterior = NULL, aux, ultimo = NULL, max = lista, anteriorMax = NULL;

    while (atual != NULL) {
        cont++;
        if (atual->prox == NULL)
            ultimo = atual;
        atual = atual->prox;
    }

    //regra 2
    if (cont % 2 == 0) {
        aux = lista->prox;
        lista->prox = aux->prox;
        free(aux);
    }

    //regra 3
    else {
        atual = lista;
        anterior = NULL;

        while (atual != NULL) {
            if (atual->bat > max->bat) {
                max = atual;
                anteriorMax = anterior;
            }
            anterior = atual;
            atual = atual->prox;
        }

        if (max != ultimo) {
            if (anteriorMax == NULL)
                lista = max->prox;
            else
                anteriorMax->prox = max->prox;
            ultimo->prox = max;
            max->prox = NULL;
        }
    }

    return lista;
}

