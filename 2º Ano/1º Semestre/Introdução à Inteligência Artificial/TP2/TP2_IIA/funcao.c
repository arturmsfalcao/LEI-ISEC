#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcao.h"

/* Lê a instância do ficheiro e devolve a matriz CxC */
double *init_dados(const char *filename, int *C_out, int *m_out)
{
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "[ERRO] nao foi possivel abrir %s\n", filename);
        exit(EXIT_FAILURE);
    }

    int C, m;
    if (fscanf(f, "%d %d", &C, &m) != 2) {
        fprintf(stderr, "[ERRO] primeira linha deve conter 'C m'\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }

    double *mat = malloc(sizeof(double) * C * C);
    if (!mat) {
        fprintf(stderr, "[ERRO] de memoria ao alocar matriz\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }

    /* inicializa a 0 */
    for (int i = 0; i < C * C; ++i) mat[i] = 0.0;

    char a[32], b[32];
    double val;
    while (fscanf(f, "%s %s %lf", a, b, &val) == 3) {
        /* converter "eX" -> índice X-1 */
        int ia = atoi(a + 1) - 1;
        int ib = atoi(b + 1) - 1;
        if (ia < 0 || ia >= C || ib < 0 || ib >= C) {
            fprintf(stderr, "[ERRO] indices fora do intervalo na linha (%s %s)\n", a, b);
            free(mat);
            fclose(f);
            exit(EXIT_FAILURE);
        }
        mat[ia * C + ib] = val;
        mat[ib * C + ia] = val;
    }

    fclose(f);
    *C_out = C;
    *m_out = m;
    return mat;
}

/* Conta 1s */
int conta_selecionados(int sol[], int C)
{
    int cnt = 0;
    for (int i = 0; i < C; ++i) if (sol[i]) ++cnt;
    return cnt;
}

/* Repara: se mais de m remove aleatoriamente; se menos, adiciona aleatoriamente.
 * Usa rand() externamente inicializado (init_rand in utils).
 */
void repara_solucao(int sol[], int C, int m)
{
    int cnt = conta_selecionados(sol, C);
    if (cnt == m) return;

    if (cnt > m) {
        while (cnt > m) {
            int idx = rand() % C;
            if (sol[idx]) { sol[idx] = 0; --cnt; }
        }
    } else {
        while (cnt < m) {
            int idx = rand() % C;
            if (!sol[idx]) { sol[idx] = 1; ++cnt; }
        }
    }
}

/* Avalia: média das distâncias entre pares dos seleccionados.
 * Se não tiver exactamente m selecionados devolve -1e30 (valor penalizado).
 */
double avalia_solucao(int sol[], const double *dist_matrix, int C, int m)
{
    int cnt = conta_selecionados(sol, C);
    if (cnt != m) return -1e30;
    if (m <= 1) return 0.0;

    double sum = 0.0;
    for (int i = 0; i < C; ++i) {
        if (!sol[i]) continue;
        for (int j = i+1; j < C; ++j) {
            if (!sol[j]) continue;
            sum += dist_matrix[i * C + j];
        }
    }
    return sum;
}
