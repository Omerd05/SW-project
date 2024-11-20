#define  _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

const int max_iter = 300;
const double eps = 1e-4;
/*int N,K,dim;*/

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

/*
 * Printing Matrix M of size M1xM2
 */
void print_matrix(Matrix matrix) {
    int i,j;
    if(matrix.table == NULL) {
        return;
    }
    for(i = 0; i < matrix.n_rows; i++) {
        for(j = 0; j < matrix.n_cols; j++) {
            printf("%.4f,",matrix.table[i][j]);
        }
        printf("\n");
    }
}

List_vectors read_points(char* filename){
    FILE *file = fopen(filename,"r");
    char* line = NULL;
    char* token = NULL;
    size_t line_len;
    int capacity = 10;
    List_vectors vectors;
    int nread=0;
    int i=0;
    int count = 0;
    vectors.v_list = calloc(capacity,sizeof(Vector));
    while((nread = getline(&line,&line_len,file)) != -1) {
        while(count >= capacity) {
            capacity *= 2;
            vectors.v_list = realloc(vectors.v_list, capacity*sizeof(Vector));
        }
        /*Every comma indicates a cord, except for the last one - therfore intialized to 1.*/
        vectors.v_list[count].len = 1;
        for(i = 0; i < nread; i++) {
            if(line[i] == ',') {
                vectors.v_list[count].len++;
            }
        }
        token = strtok(line,",\n");
        vectors.v_list[count].x = calloc(vectors.v_list[count].len,sizeof(double));
        for(i = 0; token != NULL; i++) {
            vectors.v_list[count].x[i] = atof(token);
            token = strtok(NULL,",\n");
        }

        count++;
    }
    free(line);
    free(token);
    fclose(file);

    vectors.size = count;
    /*N = count;
    dim = vectors[0].len;*/
    return vectors;
}

void free_matrix(Matrix matrix) {
    int i;
    for (i = 0; i < matrix.n_rows; i++) {
        free(matrix.table[i]);
    }
    free(matrix.table);
    /*free(matrix);*/
}

double L2Norm(Vector x1) {
    int i;
    double sum = 0;
    if(x1.x == NULL) {
        return 0;
    }
    for(i = 0; i < x1.len; i++) {
        sum += x1.x[i]*x1.x[i];
    }
    return sqrt(sum);
}

double EuclidDist(Vector x1, Vector x2) {
    int i,dim;
    double result;
    Vector y;
    dim = x1.len;
    y.len = dim;
    y.x = calloc(dim,sizeof(double));
    for(i = 0; i < dim; i++) {
        y.x[i] = x1.x[i] - x2.x[i];
    }
    result = pow(L2Norm(y),2);
    /*free(y);*/
    return result;
}

Matrix sym(List_vectors vectors) {
    int N = vectors.size;
    Matrix matrix;
    int i,j;
    matrix.n_rows = matrix.n_cols = N;
    matrix.table = calloc(N,sizeof(double*));
    for(i = 0; i < N; i++) {
        matrix.table[i] = calloc(N,sizeof(double));
    }
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            if(i!=j) {
                double target = EuclidDist(vectors.v_list[i],vectors.v_list[j]);
                matrix.table[i][j] = exp(-target/2);
            }
            else {
                matrix.table[i][j] = 0;
            }
        }
    }
    /*free_matrix(matrix.table,N);*/
    return matrix;
}

Matrix ddg(List_vectors vectors) {
    Matrix matrix;
    Matrix D;
    int i,j;
    double sum;
    int N = vectors.size;

    /*matrix.n_rows = matrix.n_cols = N;*/
    matrix = sym(vectors);

    D.n_rows = D.n_cols = N;
    D.table = calloc(N,sizeof(double*));

    if(matrix.table==NULL) {
        return matrix;
    }
    for(i = 0; i < N; i++) {
        D.table[i] = calloc(N,sizeof(double));
    }
    for(i = 0; i < N; i++) {
        sum = 0;
        for(j = 0; j < N; j++) {
            sum += matrix.table[i][j];
        }
        D.table[i][i] = sum;
    }
    /*free_matrix(matrix.table,N);*/
    /*free_matrix(D.table,N);*/
    return D;
}

Matrix norm(List_vectors vectors) {
    int N = vectors.size;
    int i,j;
    double tmp;
    Matrix A,D;
    Matrix result;
    A = sym(vectors);
    D = ddg(vectors);
    result.n_rows = result.n_cols = N;

    result.table = calloc(N,sizeof(double*));
    if(A.table==NULL || D.table==NULL) {
        return result;
    }
    for(i = 0; i < N; i++) {
        result.table[i] = calloc(N,sizeof(double));
    }
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            tmp = A.table[i][j] * (1/sqrt(D.table[i][i])) * (1/sqrt(D.table[j][j]));
            result.table[i][j] = tmp;
        }
    }
    free_matrix(A);
    free_matrix(D);
    return result;
}
void multiplication(Matrix A, Matrix B, Matrix* C){
    int i,j,l;
    C->n_rows = A.n_rows;
    C->n_cols = B.n_cols;
    C->table = calloc(C->n_rows,sizeof(double*));
    for(i = 0; i < C->n_rows; i++) {
        C->table[i] = calloc(C->n_cols,sizeof(double));
    }

    /*calculate result*/
    for(i=0;i<C->n_rows;i++){
        for(j=0;j<C->n_cols;j++){
            for(l=0;l<A.n_cols;l++){
                C->table[i][j]+=(A.table[i][l])*(B.table[l][j]);
            }
        }
    }
    return;
}
double calc_diff(Matrix A, Matrix B){
    double diff=0;
    int i,j;

    for(i=0;i<A.n_rows;i++){
        for(j=0;j<A.n_cols;j++)
            diff+=pow(A.table[i][j]-B.table[i][j],2);
    }
    diff=sqrt(diff);
    return diff;
}

Matrix transpose(Matrix A){
    Matrix A_T;
    int i,j;
    A_T.n_rows = A.n_rows;
    A_T.n_cols = A.n_cols;
    A_T.table = calloc(A_T.n_rows,sizeof(double*));
    for(i = 0; i < A_T.n_rows; i++){
        A_T.table[i] = calloc(A_T.n_cols,sizeof(double));
    }

    for(i = 0; i < A.n_rows; i++){
        for(j = 0; j < A.n_cols; j++){
            A_T.table[i][j] = A.table[j][i];
        }
    }
    return A_T;
}

Matrix symnmf(Matrix H, Matrix W){
    Matrix last_H;
    Matrix cur_H;
    Matrix WH;
    Matrix HT;
    Matrix HHTH;
    Matrix HHT;

    double diff = 1e3,beta = 0.5;
    int iter = 0,i,j;

    last_H = H;

    cur_H.n_rows = H.n_rows;
    cur_H.n_cols = H.n_cols;
    cur_H.table = calloc(cur_H.n_rows,sizeof(double*));
    for(i = 0; i < cur_H.n_rows; i++){
        cur_H.table[i] = calloc(cur_H.n_cols,sizeof(double));
    }

    for(i = 0; i < H.n_rows; i++){
        for(j = 0; j < H.n_cols; j++){
            cur_H.table[i][j] = H.table[i][j];
        }
    }
    
    while(diff >= eps && iter < max_iter){
        multiplication(W,H,&WH);
        HT = transpose(H);
        multiplication(H,HT,&HHT);
        multiplication(HT,H,&HHTH);

        for(i = 0; i < H.n_rows; i++){
            for(j = 0; j < H.n_cols; j++){
                double val = last_H.table[i][j]*(1-beta+beta*(WH.table[i][j])/(HHTH.table[i][j]));
                cur_H.table[i][j] = val;
            }
        }

        free_matrix(WH);
        free_matrix(HT);
        free_matrix(HHT);
        free_matrix(HHTH);

        diff = calc_diff(last_H,cur_H);

        if(iter > 0){
            free_matrix(last_H);
        }
        last_H = cur_H;
        iter++;
    }

    return last_H;
}

int main(int argc, char* argv[]) {
    List_vectors vectors;
    Matrix result;
    int i;
    if(argc != 3) {
        fprintf(stderr, "Invalid number of arguments\n");
        exit(1);
    }
    vectors = read_points(argv[2]);
    if(strcmp(argv[1],"sym")==0) {
        result = sym(vectors);
    }
    else if(strcmp(argv[1],"ddg")==0) {
        result = ddg(vectors);
    }
    else if(strcmp(argv[1],"norm")==0) {
        result = norm(vectors);
    }
    print_matrix(result);
    free_matrix(result);
    for (i = 0; i < vectors.size; i++) {
        free(vectors.v_list[i].x);
    }
    /*free(vectors.v_list[i]);
    free(vectors);*/
    return 0;
}
