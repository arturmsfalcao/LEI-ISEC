#ifndef HIBRIDO_H
#define HIBRIDO_H

#include "evolutivo.h"

int *hybrid_memetic(int C, int m, const double *dist_matrix,
                    struct info_ea params,
                    int improve_k, int ls_iters, int ls_neigh);

int *hybrid_initial_refine(int C, int m, const double *dist_matrix,
                           struct info_ea params,
                           int ls_iters, int ls_neigh);

#endif
