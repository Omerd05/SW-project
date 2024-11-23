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
Matrix sym(List_vectors* vectors);
Matrix ddg(List_vectors* vectors);
Matrix norm(List_vectors* vectors);
Matrix symnmf(Matrix H, Matrix W);

List_vectors read_points(char* filename);

void print_matrix(Matrix matrix);
void free_matrix(Matrix matrix);
void multiplication(Matrix A, Matrix B, Matrix* C);
double calc_diff(Matrix A, Matrix B);
Matrix transpose(Matrix A);
double EuclidDist(Vector x1, Vector x2);
double L2Norm(Vector x);

#endif /* SYMNMF_H_ */
