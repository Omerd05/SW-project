#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include "symnmf.h"

/*
* auxiliary function used to free object of List_vectors type
*/
void free_list_vectors(List_vectors* vectors){
	int i;
	for(i = 0; i < vectors->size; i++){
		free(vectors->v_list[i].x);
	}
}

/*
* auxiliary function used to convert python list into List_vectors
*/

List_vectors* create_c_points(PyObject *args){
	PyObject *lst;
	PyObject *item;
	PyObject *item1;
    List_vectors* result = malloc(sizeof(List_vectors));;
	double er;
    int n,i,j,d;

	if (!PyArg_ParseTuple(args, "O", &lst)) {
	    printf("An Error Has Occurred\n");
        return result;
    }

    /*Initializing the list_vectors result*/
    n = (result->size = PyObject_Length(lst));
    result->v_list = calloc(result->size,sizeof(Vector));

    if(result->v_list==NULL){
        printf("An Error Has Occurred\n");
        return result;
    }

    /*Actual conversion*/
    for(i = 0; i < n; i++){
    	item = PyList_GetItem(lst, i);
        d = PyObject_Length(item);
        result->v_list[i].len=d;
        result->v_list[i].x=calloc(d,sizeof(double));

        if (result->v_list[i].x == NULL) {
            printf("An Error Has Occurred\n");
            return result;
        }

        for(j=0;j<d;j++){
            item1 = PyList_GetItem(item,j);
            er = PyFloat_AsDouble(item1);
            (result->v_list[i].x)[j] = er; 
        }
    }

    return result;
}

/*
* auxiliary function used to convert our matrix in c to a list of lists in python
*/

PyObject* create_py_matrix(Matrix ans){
	int n = ans.n_rows, k = ans.n_cols;
    PyObject *mat = PyList_New(n);
    PyObject *vec;
    PyObject *num;
    int i,j;

    for(i=0;i<n;i++){
        vec=PyList_New(k);
        for(j=0;j<k;j++){
            num=Py_BuildValue("d",ans.table[i][j]);
            PyList_SetItem(vec, j, num);
        }
        PyList_SetItem(mat, i, vec);
    }
    free_matrix(ans);
    return mat;
}

/*
* auxiliary function used to convert python list into Matrix
*/

Matrix create_c_matrix(PyObject *lst){
    /*Initializing necessary variables*/
	PyObject* item;
	PyObject* item1;
	Matrix mat;
	double er;
	int n,i,j,d;
	n = PyObject_Length(lst);
	mat.n_rows = n;
	mat.table = calloc(mat.n_rows,sizeof(double*));

    /*Actual conversion*/
	for(i = 0; i < n; i++){
		item = PyList_GetItem(lst, i);
        d = PyObject_Length(item);
        mat.n_cols = d;

        mat.table[i] = calloc(d,sizeof(double));
        for(j = 0; j < d; j++){
        	item1 = PyList_GetItem(item,j);
        	er = PyFloat_AsDouble(item1);
        	mat.table[i][j] = er;
        }
	}

	return mat;
}

/*Wrapper function for sym method*/
static PyObject *Symnmf_Sym(PyObject *self, PyObject *args) {
    Matrix ans;
    List_vectors* vectors = create_c_points(args);
    ans = sym(vectors);
    free_list_vectors(vectors);
    return create_py_matrix(ans);
}

/*Wrapper function for ddg method*/
static PyObject *Symnmf_Ddg(PyObject *self, PyObject *args) {
    Matrix ans;
    List_vectors* vectors = create_c_points(args);
    ans = ddg(vectors);
    free_list_vectors(vectors);
    return create_py_matrix(ans);
}

/*Wrapper function for norm method*/
static PyObject *Symnmf_Norm(PyObject *self, PyObject *args) {
    Matrix ans;
    List_vectors* vectors = create_c_points(args);
    ans = norm(vectors);
    free_list_vectors(vectors);
    return create_py_matrix(ans);
}

/*Wrapper function for symnmf method*/
static PyObject *Symnmf_Symnmf(PyObject *self, PyObject *args) {
    Matrix ans;
    Matrix H;
    Matrix W;
    PyObject *lst1, *lst2;
    if (!PyArg_ParseTuple(args, "OO", &lst1,&lst2)) {
        return NULL;
    }
    H = create_c_matrix(lst1);
    W = create_c_matrix(lst2);

    ans = symnmf(H,W);
    free_matrix(W);

    return create_py_matrix(ans);
}

static PyMethodDef Symnmf_FunctionsTable[] = {
    {
        "sym",
        Symnmf_Sym,
        METH_VARARGS,
        "Calculates the Similarity Matrix of the given points"
    }, {
        "ddg",
        Symnmf_Ddg,
        METH_VARARGS,
        "Calculates the Diagonal Degree Matrix of the given points"
    }, {
        "norm",
        Symnmf_Norm,
        METH_VARARGS,
        "Calculates the Normalized Similarity Matrix of the given points"
    }, {
        "symnmf",
        Symnmf_Symnmf,
        METH_VARARGS,
        "Calculates the Optimizing H Matrix of the W and H matrixes"
    },{
        NULL, NULL, 0, NULL
    }
};

static struct PyModuleDef Symnmf_Module = {
    PyModuleDef_HEAD_INIT,
    "symnmf",
    "Symnmf Python wrapper for custom C extension library.", // module documentation
    -1,
    Symnmf_FunctionsTable
};

PyMODINIT_FUNC PyInit_symnmf(void) {
    return PyModule_Create(&Symnmf_Module);
}
