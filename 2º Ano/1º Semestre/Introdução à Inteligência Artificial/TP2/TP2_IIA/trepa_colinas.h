#ifndef TREPA_COLINAS_H
#define TREPA_COLINAS_H

/* Gera vizinho: swap simples (1 sai, 1 entra) */
void gera_vizinho(int sol[], int viz[], int C);

/* Gera vizinho: double swap (2 saem, 2 entram) */
void gera_vizinho2(int sol[], int viz[], int C);

/* Trepa-colinas:
 * sol     -> solução inicial (modificada in-place)
 * C, m    -> número de vértices e nº de seleccionados obrigatórios
 * dist    -> matriz de distâncias CxC
 * iter    -> número de iterações
 * neigh   -> 1 = vizinhança simples, 2 = double swap
 *
 * devolve a qualidade (distância média)
 */
double trepa_colinas(int sol[], int C, int m, const double *dist, int iter, int neigh);

#endif
