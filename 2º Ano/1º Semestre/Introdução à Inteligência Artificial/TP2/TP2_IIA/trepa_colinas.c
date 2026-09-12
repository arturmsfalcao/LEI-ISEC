#include <stdio.h>
#include <stdlib.h>
#include "trepa_colinas.h"
#include "utils.h"
#include "funcao.h"

/* VIZINHANÇA 1: swap simples */
void gera_vizinho(int sol[], int viz[], int C)
{
    for(int i=0; i<C; i++)
        viz[i] = sol[i];

    int p1, p2;

    /* escolhe posição 1 → desliga 1 */
    do p1 = random_l_h(0, C-1);
    while(viz[p1] != 1);

    /* escolhe posição 0 → liga 1 */
    do p2 = random_l_h(0, C-1);
    while(viz[p2] != 0);

    viz[p1] = 0;
    viz[p2] = 1;
}

/* VIZINHANÇA 2: double swap (duas trocas) */
void gera_vizinho2(int sol[], int viz[], int C)
{
    for(int i=0;i<C;i++)
        viz[i] = sol[i];

    int s1, s2, u1, u2;

    /* 2 seleccionados para remover */
    do s1 = random_l_h(0, C-1); while(viz[s1] != 1);
    do s2 = random_l_h(0, C-1); while(viz[s2] != 1 || s2 == s1);

    /* 2 não seleccionados para adicionar */
    do u1 = random_l_h(0, C-1); while(viz[u1] != 0);
    do u2 = random_l_h(0, C-1); while(viz[u2] != 0 || u2 == u1);

    viz[s1] = 0;
    viz[s2] = 0;
    viz[u1] = 1;
    viz[u2] = 1;
}

/* TREPA-COLINAS (best improvement) */
double trepa_colinas(int sol[], int C, int m, const double *dist, int iter, int neigh)
{
    int *viz = malloc(sizeof(int)*C);
    int *best_viz = malloc(sizeof(int)*C);
    if (!viz || !best_viz) { printf("[ERRO] de memória.\n"); exit(1); }

    repara_solucao(sol, C, m);
    double custo = avalia_solucao(sol, dist, C, m);

    for(int k=0; k<iter; k++)
    {
        double melhor = custo;
        int found = 0;

        for(int t=0; t<C; t++)
        {
            if (neigh == 1) gera_vizinho(sol, viz, C);
            else            gera_vizinho2(sol, viz, C);

            repara_solucao(viz, C, m);
            double fit = avalia_solucao(viz, dist, C, m);

            if (fit > melhor)
            {
                melhor = fit;
                /* guarda o vizinho */
                for(int i=0;i<C;i++) best_viz[i] = viz[i];
                found = 1;
                /* best-improvement: continua a procurar melhores */
            }
        }

        if (!found) break;

        /* Accept best neighbour */
        substitui(sol, best_viz, C);
        custo = melhor;
    }

    free(viz);
    free(best_viz);
    return custo;
}

