#ifndef FUNCAO_H
#define FUNCAO_H

/* Lê a instância do ficheiro.
 * Formato esperado:
 *   primeira linha:  C m
 *   linhas seguintes: eA eB valor   (ex.: e1 e2 3.5)
 * Devolve um ponteiro para uma matriz (double) CxC alocada dinamicamente.
 * Os valores de C e m são escritos em *C_out e *m_out.
 */
double *init_dados(const char *filename, int *C_out, int *m_out);

/* Conta o número de 1s numa solução binária */
int conta_selecionados(int sol[], int C);

/* Repara a solução para ter exactamente m seleccionados (altera sol) */
void repara_solucao(int sol[], int C, int m);

/* Avalia a solução: devolve a distância média entre os m seleccionados.
 * Se a solução não tiver exactamente m seleccionados, devolve um valor penalizado (-1e30).
 */
double avalia_solucao(int sol[], const double *dist_matrix, int C, int m);

#endif
