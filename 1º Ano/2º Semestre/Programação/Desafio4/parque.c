
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parque.h"

// Nome: Artur Meireles de Sousa Falcão
// Número: 2024130485


void libertaTudo(pCliente p){
    pCliente auxC;
    pAcesso auxA;

    while(p != NULL){
        while(p->lista != NULL){
            auxA = p->lista;
            p->lista = p->lista->prox;
            free(auxA);
        }
        auxC = p;
        p = p->prox;
        free(auxC);
    }
}

pCliente criaExemploED(cliente a[], acesso b[], int totC){
    int i, j, k=-1;

    pCliente lista = NULL, novoC;
    pAcesso novoA;

    for(i=0; i<totC; i++){
        k+=a[i].contador;
    }
    for(i=totC-1; i>=0; i--){
        novoC = malloc(sizeof(cliente));
        if(novoC == NULL){
            libertaTudo(lista);
            return NULL;
        }
        *novoC = a[i];
        novoC->prox = lista;
        lista = novoC;
        for(j=0; j<novoC->contador; j++){
            novoA = malloc(sizeof(acesso));
            if(novoA == NULL){
                libertaTudo(lista);
                return NULL;
            }
            *novoA = b[k--];
            novoA->prox = novoC->lista;
            novoC->lista = novoA;
        }
    }
    return lista;
}

void mostraTudo(pCliente p){
    pAcesso auxA;

    while(p != NULL){
        printf("\nUtilizador com id %d efetuou %d acessos\n", p->id, p->contador);
        auxA = p->lista;
        while(auxA != NULL){
            printf("Entrou as %2.2d:%2.2d. ", auxA->in.h, auxA->in.m);
            if(auxA->out.h == -1)
                printf("Ainda nao saiu do parque\n");
            else
                printf("Saiu as %2.2d:%2.2d\n", auxA->out.h, auxA->out.m);
            auxA = auxA->prox;
        }
        p = p->prox;
    }
}


// Criar funcoes auxiliares aqui



// Final das funcoes auxiliares

pCliente desafio4(pCliente p, int *id){
    if (p==NULL) {
        *id = -1;
        return p;
    }

    pCliente atualC = p;
    int maxCompletas = 0;
    int idMaior = -1;

    while (atualC != NULL) {
        pAcesso aux = atualC->lista;
        int completasCliente = 0;

        while (aux != NULL) {
            if (aux->out.h != -1) {
                completasCliente++;
            }
            aux = aux->prox;
        }
        if (completasCliente > 0) {
            if (completasCliente > maxCompletas || (completasCliente == maxCompletas && atualC->id > idMaior)) {
                maxCompletas = completasCliente;
                idMaior = atualC->id;
            }
        }

        atualC = atualC->prox;
    }

    *id = idMaior;

    pAcesso atualA = p->lista;
    pAcesso anteriorA = NULL;

    while (atualA != NULL) {
        int flagApagar = 0;

        if (atualA->out.h != -1) {
            int duracao = (atualA->out.h * 60 + atualA->out.m) - (atualA->in.h * 60 + atualA->in.m);

            if (duracao < 30)
                flagApagar = 1;
        }
        if (flagApagar == 1){
            pAcesso apagarA = atualA;

            if (anteriorA == NULL) {
                p->lista = atualA->prox;
                atualA = p->lista;
            }
            else {
                anteriorA->prox = atualA->prox;
                atualA = atualA->prox;
            }

            free(apagarA);
            p->contador--;
        }
        else {
            anteriorA = atualA;
            atualA = atualA->prox;
        }
    }
    if (p->lista == NULL) {
        pCliente apagarC = p;
        p = p->prox;
        free(apagarC);
    }

    return p;
}