#ifndef EVOLUTIVO_H
#define EVOLUTIVO_H

#define MAX_VERT 2000   // ajusta conforme necessário

typedef struct individuo {
    int p[MAX_VERT];        // solução (0/1)
    double fitness;         // valor da solução
    int valido;             // sempre 1 (usamos reparação)
} chrom, *pchrom;

struct info_ea {
    int popsize;        // tamanho da população
    int numGenes;       // C vértices
    int m;              // nº de selecionados obrigatórios
    double pm;          // prob. mutação
    double pr;          // prob. recombinação
    int numGenerations; // número de gerações

    int metodo_sel;     // 1=torneio, 2=roleta
    int metodo_rec;     // 1=1 ponto, 2=uniforme
    int metodo_mut;     // 1=bitflip, 2=swap
};

/* Inicialização */
pchrom init_pop(struct info_ea d);

/* Avaliação */
void evaluate(pchrom pop, struct info_ea d, const double *dist);

/* Seleção */
void selecao_torneio(pchrom pop, struct info_ea d, pchrom pais);
void selecao_roleta(pchrom pop, struct info_ea d, pchrom pais);

/* Recombinação */
void crossover_1ponto(pchrom pais, struct info_ea d, pchrom filhos);
void crossover_uniforme(pchrom pais, struct info_ea d, pchrom filhos);

/* Mutação */
void mutacao_bitflip(pchrom filhos, struct info_ea d);
void mutacao_swap(pchrom filhos, struct info_ea d);

/* wrapper */
void operadores_geneticos(pchrom pais, struct info_ea d, pchrom filhos);

/* Melhor indivíduo */
chrom get_best(pchrom pop, struct info_ea d, chrom best);

/* Escrita */
void escreve_individuo(chrom x, struct info_ea d);

#endif
