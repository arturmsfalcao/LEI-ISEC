#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hibrido.h"
#include "evolutivo.h"
#include "trepa_colinas.h"
#include "funcao.h"
#include "utils.h"

/* copia chrom.p para int array dest (length C) */
static void chrom_to_array(int dest[], const chrom *c, int C) {
    for (int i = 0; i < C; ++i) dest[i] = c->p[i];
}

/* copia int array src para chrom.p */
static void array_to_chrom(chrom *c, const int src[], int C) {
    for (int i = 0; i < C; ++i) c->p[i] = src[i];
}

static void sort_indices_by_fitness(int idx[], chrom pop[], int pop_size) {
    for (int i = 0; i < pop_size - 1; ++i) {
        int best = i;
        for (int j = i + 1; j < pop_size; ++j) {
            if (pop[j].fitness > pop[best].fitness) best = j;
        }
        if (best != i) {
            int tmp = idx[i];
            idx[i] = idx[best];
            idx[best] = tmp;
        }
    }
}

/* HYBRID 1: MEMETIC */
int *hybrid_memetic(int C, int m, const double *dist_matrix,
                    struct info_ea params,
                    int improve_k, int ls_iters, int ls_neigh)
{

    params.numGenes = C;
    params.m = m;

    pchrom pop = init_pop(params);
    evaluate(pop, params, dist_matrix);

    /* arrays temporarios */
    pchrom pais = malloc(sizeof(chrom) * params.popsize);
    pchrom filhos = malloc(sizeof(chrom) * params.popsize);
    if (!pais || !filhos) {
        fprintf(stderr, "[ERRO] alocar memoria em hybrid_memetic\n");
        exit(1);
    }

    /* EA loop */
    for (int gen = 0; gen < params.numGenerations; ++gen) {

        if (params.metodo_sel == 1)
            selecao_torneio(pop, params, pais);
        else
            selecao_roleta(pop, params, pais);

        operadores_geneticos(pais, params, filhos);

        for (int i = 0; i < params.popsize; ++i) {
            repara_solucao(filhos[i].p, C, m);
            filhos[i].fitness = avalia_solucao(filhos[i].p, dist_matrix, C, m);
        }

        int *idx = malloc(sizeof(int) * params.popsize);
        for (int i = 0; i < params.popsize; ++i) idx[i] = i;

        for (int a = 0; a < params.popsize - 1; ++a) {
            int best = a;
            for (int b = a+1; b < params.popsize; ++b) {
                if (filhos[b].fitness > filhos[best].fitness) best = b;
            }
            if (best != a) {
                int tmp = idx[a];
                idx[a] = idx[best];
                idx[best] = tmp;

                chrom tmpc = filhos[a];
                filhos[a] = filhos[best];
                filhos[best] = tmpc;
            }
        }

        int kmax = (improve_k < params.popsize) ? improve_k : params.popsize;
        for (int k = 0; k < kmax; ++k) {

            int *tmp = malloc(sizeof(int) * C);
            chrom_to_array(tmp, &filhos[k], C);
            /* aplica trepa-colinas */
            trepa_colinas(tmp, C, m, dist_matrix, ls_iters, ls_neigh);

            array_to_chrom(&filhos[k], tmp, C);
            filhos[k].fitness = avalia_solucao(filhos[k].p, dist_matrix, C, m);
            free(tmp);
        }

        free(idx);


        for (int i = 0; i < params.popsize; ++i) {
            pop[i] = filhos[i];
        }
    }

    chrom best = pop[0];
    for (int i = 1; i < params.popsize; ++i)
        if (pop[i].fitness > best.fitness) best = pop[i];

    int *result = malloc(sizeof(int) * C);
    if (!result) { fprintf(stderr, "[ERRO] malloc result hybrid_memetic\n"); exit(1); }
    for (int i = 0; i < C; ++i) result[i] = best.p[i];

    free(pop);
    free(pais);
    free(filhos);

    return result;
}

/* HYBRID 2: INITIAL REFINEMENT */
int *hybrid_initial_refine(int C, int m, const double *dist_matrix,
                           struct info_ea params,
                           int ls_iters, int ls_neigh)
{
    params.numGenes = C;
    params.m = m;

    pchrom pop = init_pop(params);

    /* aplica LS */
    for (int i = 0; i < params.popsize; ++i) {
        /* copy to temp array */
        int *tmp = malloc(sizeof(int) * C);
        if (!tmp) { fprintf(stderr, "[ERRO] malloc tmp hybrid_initial_refine\n"); exit(1); }
        chrom_to_array(tmp, &pop[i], C);
        trepa_colinas(tmp, C, m, dist_matrix, ls_iters, ls_neigh);
        array_to_chrom(&pop[i], tmp, C);
        free(tmp);
        /* evaluate */
        pop[i].fitness = avalia_solucao(pop[i].p, dist_matrix, C, m);
    }

    /* corre EA normalmente */
    pchrom pais = malloc(sizeof(chrom) * params.popsize);
    pchrom filhos = malloc(sizeof(chrom) * params.popsize);
    if (!pais || !filhos) { fprintf(stderr, "[ERRO] alocar pais/filhos\n"); exit(1); }

    for (int gen = 0; gen < params.numGenerations; ++gen) {

        if (params.metodo_sel == 1)
            selecao_torneio(pop, params, pais);
        else
            selecao_roleta(pop, params, pais);

        operadores_geneticos(pais, params, filhos);

        for (int i = 0; i < params.popsize; ++i) {
            repara_solucao(filhos[i].p, C, m);
            filhos[i].fitness = avalia_solucao(filhos[i].p, dist_matrix, C, m);
        }

        for (int i = 0; i < params.popsize; ++i) {
            pop[i] = filhos[i];
        }
    }

    chrom best = pop[0];
    for (int i = 1; i < params.popsize; ++i)
        if (pop[i].fitness > best.fitness) best = pop[i];

    int *result = malloc(sizeof(int) * C);
    if (!result) { fprintf(stderr, "[ERRO] malloc result hybrid_initial_refine\n"); exit(1); }
    for (int i = 0; i < C; ++i) result[i] = best.p[i];

    free(pop);
    free(pais);
    free(filhos);

    return result;
}
