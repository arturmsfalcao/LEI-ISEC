#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trepa_colinas.h"
#include "evolutivo.h"
#include "hibrido.h"
#include "funcao.h"
#include "utils.h"

#define DEFAULT_RUNS 10


double *read_instance(const char *fname, int *C, int *m)
{
    FILE *f = fopen(fname, "rt");
    if (!f) {
        printf("[ERRO] ao abrir '%s'\n", fname);
        exit(1);
    }

    fscanf(f, "%d %d", C, m);

    double *dist = malloc(sizeof(double) * (*C) * (*C));
    if (!dist) {
        printf("[ERRO] alocar matriz dist\n");
        exit(1);
    }

    for (int i = 0; i < *C; i++)
        for (int j = 0; j < *C; j++)
            fscanf(f, "%lf", &dist[i*(*C) + j]);

    fclose(f);
    return dist;
}

/* Função para escrever solução  */
void write_solution(const int sol[], int C)
{
    printf("Selecionados: ");
    for (int i = 0; i < C; i++)
        if (sol[i] == 1)
            printf("%d ", i);
    printf("\n");
}

/* MAIN */
int main()
{
    char fname[200];
    int C, m;
    int opcao, runs;
    double *dist;

    init_rand();

    printf("Nome da instancia: ");
    scanf("%s", fname);

    /* Lê o ficheiro */
    dist = init_dados(fname, &C, &m);

    printf("Numero de runs (default %d): ", DEFAULT_RUNS);
    if (scanf("%d", &runs) != 1)
        runs = DEFAULT_RUNS;

    if (runs <= 0) runs = DEFAULT_RUNS;

    /* Menu */
    printf("\n=== MENU ===\n");
    printf("1 - Pesquisa Local (Trepa-Colinas)\n");
    printf("2 - Algoritmo Evolutivo\n");
    printf("3 - Algoritmo Hibrido\n");
    printf("Opcao: ");
    scanf("%d", &opcao);

    double mbf = 0.0;
    double best_global_value = -1e18;
    int *best_global_sol = malloc(sizeof(int) * C);

    /* OPÇÃO 1 — PESQUISA LOCAL */
    if (opcao == 1)
    {
        int iter, neigh;

        printf("Numero de iteracoes: ");
        scanf("%d", &iter);

        printf("Vizinhanca (1 = swap simples, 2 = double swap): ");
        scanf("%d", &neigh);

        for (int r = 0; r < runs; r++)
        {
            printf("\nRun %d:\n", r+1);

            int *sol = malloc(sizeof(int) * C);

            /* Gerar solução inicial válida */
            for (int i = 0; i < C; i++)
                sol[i] = 0;
            for (int k = 0; k < m; k++) {
                int pos;
                do pos = random_l_h(0, C-1);
                while (sol[pos] == 1);
                sol[pos] = 1;
            }

            double fit = trepa_colinas(sol, C, m, dist, iter, neigh);

            printf("Fitness final: %.2f\n", fit/m);
            write_solution(sol, C);

            mbf += fit;

            if (fit > best_global_value) {
                best_global_value = fit;
                for (int i = 0; i < C; i++)
                    best_global_sol[i] = sol[i];
            }

            free(sol);
        }
    }

    /* OPÇÃO 2 — ALGORITMO EVOLUTIVO */
    else if (opcao == 2)
    {
        struct info_ea p;

        printf("Tamanho da populacao (>=2): ");
        scanf("%d", &p.popsize);

        if (p.popsize < 2) {
            printf("Populacao demasiado pequena. Ajustado para 2.\n");
            p.popsize = 2;
        }

        printf("Geracoes (>=1): ");
        scanf("%d", &p.numGenerations);

        if (p.numGenerations < 1) {
            printf("Numero de geracoes demasiado pequeno. Ajustado para 1.\n");
            p.numGenerations = 1;
        }

        printf("Prob. recombinacao (0-1): ");
        scanf("%lf", &p.pr);

        printf("Prob. mutacao (0-1): ");
        scanf("%lf", &p.pm);

        printf("Selecao (1=toneio, 2=roleta): ");
        scanf("%d", &p.metodo_sel);

        printf("Recombinacao (1=1 ponto, 2=uniforme): ");
        scanf("%d", &p.metodo_rec);

        printf("Mutacao (1=bitflip, 2=swap): ");
        scanf("%d", &p.metodo_mut);

        p.numGenes = C;
        p.m = m;

        for (int r = 0; r < runs; r++)
        {
            printf("\nRun %d:\n", r+1);

            /* cria população */
            pchrom pop = init_pop(p);

            /* avalia */
            evaluate(pop, p, dist);

            chrom best_run = pop[0];
            for (int i = 1; i < p.popsize; i++)
                if (pop[i].fitness > best_run.fitness)
                    best_run = pop[i];

            /* pais e filhos */
            pchrom pais = malloc(sizeof(chrom) * p.popsize);
            pchrom filhos = malloc(sizeof(chrom) * p.popsize);

            /* ciclo EA */
            for (int gen = 0; gen < p.numGenerations; gen++)
            {
                if (p.metodo_sel == 1)
                    selecao_torneio(pop, p, pais);
                else
                    selecao_roleta(pop, p, pais);

                operadores_geneticos(pais, p, filhos);

                for (int i = 0; i < p.popsize; i++) {
                    repara_solucao(filhos[i].p, C, m);
                    filhos[i].fitness = avalia_solucao(filhos[i].p, dist, C, m);
                }

                for (int i = 0; i < p.popsize; i++)
                    pop[i] = filhos[i];

                best_run = get_best(pop, p, best_run);
            }

            printf("Fitness final: %.2f\n", best_run.fitness/m);
            escreve_individuo(best_run, p);

            mbf += best_run.fitness;

            if (best_run.fitness > best_global_value)
            {
                best_global_value = best_run.fitness;
                for (int i = 0; i < C; i++)
                    best_global_sol[i] = best_run.p[i];
            }

            free(pop);
            free(pais);
            free(filhos);
        }
    }

    /* OPÇÃO 3 — MÉTODOS HÍBRIDOS */
    else if (opcao == 3)
    {
        int escolha;
        printf("\nMetodo Hibrido:\n");
        printf("1 - Memetico (EA + LS cada geracao)\n");
        printf("2 - Refinamento Inicial (LS -> EA normal)\n");
        printf("Opcao: ");
        scanf("%d", &escolha);

        struct info_ea p;

        printf("Tamanho da populacao (>=2): ");
        scanf("%d", &p.popsize);

        if (p.popsize < 2) {
            printf("Populacao demasiado pequena. Ajustado para 2.\n");
            p.popsize = 2;
        }

        printf("Geracoes (>=1): ");
        scanf("%d", &p.numGenerations);

        if (p.numGenerations < 1) {
            printf("Numero de geracoes demasiado pequeno. Ajustado para 1.\n");
            p.numGenerations = 1;
        }

        printf("Prob. recombinacao (0-1): ");
        scanf("%lf", &p.pr);

        printf("Prob. mutacao (0-1): ");
        scanf("%lf", &p.pm);

        printf("Selecao (1=toneio, 2=roleta): ");
        scanf("%d", &p.metodo_sel);

        printf("Recombinacao (1=1 ponto, 2=uniforme): ");
        scanf("%d", &p.metodo_rec);

        printf("Mutacao (1=bitflip, 2=swap): ");
        scanf("%d", &p.metodo_mut);

        p.numGenes = C;
        p.m = m;

        int ls_iters, ls_neigh, improve_k;
        printf("Iteracoes LS: ");
        scanf("%d", &ls_iters);

        printf("Vizinhanca LS (1 ou 2): ");
        scanf("%d", &ls_neigh);

        if (escolha == 1) {
            printf("Quantos individuos por geracao aplicar LS? ");
            scanf("%d", &improve_k);
        }

        for (int r = 0; r < runs; r++)
        {
            printf("\nRun %d:\n", r+1);

            int *sol;

            if (escolha == 1)
                sol = hybrid_memetic(C, m, dist, p, improve_k, ls_iters, ls_neigh);
            else
                sol = hybrid_initial_refine(C, m, dist, p, ls_iters, ls_neigh);

            double fit = avalia_solucao(sol, dist, C, m);

            printf("Fitness final: %.2f\n", fit/m);
            write_solution(sol, C);

            mbf += fit;

            if (fit > best_global_value) {
                best_global_value = fit;
                for (int i = 0; i < C; i++)
                    best_global_sol[i] = sol[i];
            }

            free(sol);
        }
    }

    /* RESULTADOS */
    printf("\n\n===== RESULTADOS =====\n");
    printf("AVG = %.2f\n", mbf / runs / m);
    printf("BEST = %.2f\n", best_global_value/m);
    write_solution(best_global_sol, C);

    free(best_global_sol);
    free(dist);
    return 0;
}

