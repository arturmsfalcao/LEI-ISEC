#include <stdio.h>
#include <stdlib.h>
#include "evolutivo.h"
#include "funcao.h"
#include "utils.h"

/* INICIALIZA POPULAÇÃO */
pchrom init_pop(struct info_ea d)
{
    pchrom pop = malloc(sizeof(chrom) * d.popsize);
    if (!pop) {
        printf("[ERRO] alocar população.\n");
        exit(1);
    }

    for (int i = 0; i < d.popsize; i++)
    {
        /* começa tudo a 0 */
        for (int j = 0; j < d.numGenes; j++)
            pop[i].p[j] = 0;

        /* coloca exatamente m em 1 */
        for (int k = 0; k < d.m; k++) {
            int pos;
            do pos = random_l_h(0, d.numGenes - 1);
            while (pop[i].p[pos] == 1);
            pop[i].p[pos] = 1;
        }

        pop[i].valido = 1;
    }

    return pop;
}

/* AVALIA POPULAÇÃO */
void evaluate(pchrom pop, struct info_ea d, const double *dist)
{
    for (int i = 0; i < d.popsize; i++) {
        /* reparação garante sempre m seleccionados */
        repara_solucao(pop[i].p, d.numGenes, d.m);
        pop[i].fitness = avalia_solucao(pop[i].p, dist, d.numGenes, d.m);
    }
}

/* SELEÇÃO POR TORNEIO */
void selecao_torneio(pchrom pop, struct info_ea d, pchrom pais)
{
    for (int i = 0; i < d.popsize; i++)
    {
        int a = random_l_h(0, d.popsize - 1);
        int b;
        do b = random_l_h(0, d.popsize - 1); while (b == a);

        if (pop[a].fitness > pop[b].fitness)
            pais[i] = pop[a];
        else
            pais[i] = pop[b];
    }
}

/* SELEÇÃO POR ROLETA */
void selecao_roleta(pchrom pop, struct info_ea d, pchrom pais)
{
    double soma = 0.0;

    for (int i = 0; i < d.popsize; i++)
        soma += pop[i].fitness;

    for (int p = 0; p < d.popsize; p++)
    {
        double alvo = rand_01() * soma;
        double parcial = 0.0;

        for (int i = 0; i < d.popsize; i++) {
            parcial += pop[i].fitness;
            if (parcial >= alvo) {
                pais[p] = pop[i];
                break;
            }
        }
    }
}

/* CROSSOVER - 1 PONTO */
void crossover_1ponto(pchrom pais, struct info_ea d, pchrom filhos)
{
    for (int i = 0; i < d.popsize; i += 2)
    {
        if (rand_01() < d.pr)
        {
            int p = random_l_h(1, d.numGenes - 1);
            for (int j = 0; j < p; j++) {
                filhos[i].p[j]     = pais[i].p[j];
                filhos[i+1].p[j]   = pais[i+1].p[j];
            }
            for (int j = p; j < d.numGenes; j++) {
                filhos[i].p[j]     = pais[i+1].p[j];
                filhos[i+1].p[j]   = pais[i].p[j];
            }
        }
        else {
            filhos[i]   = pais[i];
            filhos[i+1] = pais[i+1];
        }
    }
}

/* CROSSOVER UNIFORME */
void crossover_uniforme(pchrom pais, struct info_ea d, pchrom filhos)
{
    for (int i = 0; i < d.popsize; i += 2)
    {
        for (int j = 0; j < d.numGenes; j++)
        {
            if (rand_01() < d.pr) {
                filhos[i].p[j]   = pais[i].p[j];
                filhos[i+1].p[j] = pais[i+1].p[j];
            }
            else {
                filhos[i].p[j]   = pais[i+1].p[j];
                filhos[i+1].p[j] = pais[i].p[j];
            }
        }
    }
}

/* MUTAÇÃO BITFLIP */
void mutacao_bitflip(pchrom filhos, struct info_ea d)
{
    for (int i = 0; i < d.popsize; i++)
        for (int g = 0; g < d.numGenes; g++)
            if (rand_01() < d.pm)
                filhos[i].p[g] = !filhos[i].p[g];
}

/* MUTAÇÃO SWAP */
void mutacao_swap(pchrom filhos, struct info_ea d)
{
    for (int i = 0; i < d.popsize; i++)
    {
        if (rand_01() < d.pm)
        {
            int s1, s2;

            /* troca entre posições com valores diferentes */
            do s1 = random_l_h(0, d.numGenes - 1); while(filhos[i].p[s1] != 1);
            do s2 = random_l_h(0, d.numGenes - 1); while(filhos[i].p[s2] != 0);

            filhos[i].p[s1] = 0;
            filhos[i].p[s2] = 1;
        }
    }
}

/* OPERADORES GENÉTICOS (wrapper) */
void operadores_geneticos(pchrom pais, struct info_ea d, pchrom filhos)
{
    /* RECOMBINAÇÃO */
    if (d.metodo_rec == 1)
        crossover_1ponto(pais, d, filhos);
    else
        crossover_uniforme(pais, d, filhos);

    /* MUTAÇÃO */
    if (d.metodo_mut == 1)
        mutacao_bitflip(filhos, d);
    else
        mutacao_swap(filhos, d);
}

/* MELHOR INDIVÍDUO */
chrom get_best(pchrom pop, struct info_ea d, chrom best)
{
    for (int i = 0; i < d.popsize; i++)
        if (pop[i].fitness > best.fitness)
            best = pop[i];

    return best;
}

/* ESCREVER INDIVÍDUO */
void escreve_individuo(chrom x, struct info_ea d)
{
    printf("Fitness: %.2f\n", x.fitness);
    printf("Solucao: ");

    for (int i = 0; i < d.numGenes; i++)
        printf("%d", x.p[i]);

    printf("\n");
}
