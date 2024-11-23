#ifndef SYMNMF_H_
#define SYMNMF_H_

#include <stdio.h>
#include <stdlib.h>

/* Struct Definitions */
typedef struct {
    double* x;
    int len;
} Vector;

typedef struct {
    Vector* v_list;
    int size;
} List_vectors;

typedef struct {
    double** table;
    int n_rows;
    int n_cols;
} Matrix;

/* Function Declarations */
Matrix* sym(List_vectors* vectors);
Matrix* ddg(List_vectors* vectors);
Matrix* norm(List_vectors* vectors);
Matrix* symnmf(Matrix* H, Matrix* W);

void free_matrix(Matrix* matrix);
#endif /* SYMNMF_H_ */
