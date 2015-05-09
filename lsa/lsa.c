#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 

#include <math.h> 
#include "jacobi.h"

void generalized_print_matrix(double* m, int rows, int cols) {
    int i, j; 
    for (i = 0; i < rows; i++) {
        printf("\n| ");
        for (j = 0; j < cols; j++) {
            printf("%7.4f | ", m[(i * cols) + j]); 
        }
        //printf("\n"); 
    }

    printf("\n"); 
}

void gen_diag_matrix(double* vector, double*matrix, int dim_x, int dim_y) {
    
    
    int i, j; 
    for (i = 0; i < dim_y; i++) {
        for (j = 0; j < dim_x; j++) {
            if (i == j) { //diagonal element 
                matrix[i * dim_x + j] = vector[i]; 
            }

            else {
                matrix[i * dim_x + j] = 0.0; 
            }
        }
    }

}


int copy_rows(double* in, double* out, int start_row, int end_row, int dim) {
    if (!in || !out || start_row >= end_row) {
        return -1; 
    }

    int i, j; 
    for (i = start_row; i < end_row; i++) {
        for (j = 0; j < dim; j++) {
            out[(i - start_row) * dim + j ] =  in [i * dim + j]; 
        }
    }
    return 0; 
}

int copy_cols(double* in, double*out, int start_col, int end_col, int dim) { 
    if (!in || !out || start_col >= end_col) {
        return -1; 
    }
    
    int i, j; 
    for (i = 0; i < dim; i++){
        for (j = start_col; j < end_col; j++) {
            // printf("DEBUG i : %d j : %d \n", i, j); 
            out[i * (end_col - start_col)  + (j - start_col) ] =  in [i * dim + j]; 
        }
    }
    return 0; 
}

int trunc_rows(double** in, int num_rows, int old_x_dim) {
    /* Copy the first num_rows rows from input matrix
     * Making a new matrix of num_rows rows and old_x_dim cols
     * Maybe I should have used realloc() here?
     * fuckit
     */
    
    double* new_matrix = (double*) malloc(sizeof(double) * num_rows * old_x_dim); 
    if (!new_matrix) {
        return -1; 
    }
    copy_rows(*in, new_matrix, 0, num_rows, old_x_dim); 
    free(*in); 
    *in = new_matrix; 
    return 0; 
}

int trunc_cols(double** in, int num_cols, int old_y_dim) {
    /* Copy the first num_cols cols from input matrix
     * Making a new matrix of old_y_dim rows and num_cols cols
     */

    double* new_matrix = (double*) malloc(sizeof(double) * num_cols * old_y_dim); 
    if (!new_matrix) {
        return -1; 
    }
    copy_cols(*in, new_matrix, 0, num_cols, old_y_dim); 
    generalized_print_matrix(new_matrix, old_y_dim, num_cols); 
    free(*in); 
    *in = new_matrix; 
    return 0; 
    }


int lsa_transform(double* input_matrix, int input_dim, double* output_matrix, int output_dim) {
    if (output_dim >= input_dim) {
        return -1; 
    }

    //Anton's v returns Vt 
    double *s, *u, *v;  
    u = (double *) malloc (sizeof (double) * (input_dim * input_dim ));	
    s = (double *) malloc (sizeof (double) * input_dim);
    v = (double *) malloc (sizeof (double) * (input_dim * input_dim));
    printf("DEBUG: Starting jacobi now\n"); 
    jacobi (input_matrix, input_dim, s, u, v);
    double* full_diag_s = (double*) malloc(sizeof(double) * input_dim * input_dim); 
    gen_diag_matrix(s, full_diag_s, input_dim, input_dim); 
    printf("Singular values matrix (diagonal) \n"); 
    printMatrix(full_diag_s, input_dim); 

    printf("Matrix U\n"); 
    printMatrix(u, input_dim); 

    printf("Matrix Vt\n");  
    printMatrix(v, input_dim); 
    
    
    int i; 


    //Dimensionality reduction 
    for (i = output_dim; i< input_dim; i++) {
        s[i] = 0.0; 
    }

    //Reduce the size of the orthonormal vectors u v 
    //TODO not sure what to return out - the lsa just says remove 
    //lowest singular values - best output u' v' s' vs a' or vs' us' 
    //depends on exactly what you are doing with it 


    double * diag_s = (double *) malloc(sizeof(double) * (output_dim * output_dim)); 
    gen_diag_matrix(s, diag_s, output_dim, output_dim); 
    printf("Truncated Matrix S' \n"); 
    printMatrix(diag_s, output_dim); 

    trunc_rows(&u, output_dim, input_dim);

    printf("Truncated U'"); 

    generalized_print_matrix(u, output_dim, input_dim); 

     
    printf("Truncated Vt'"); 
    trunc_cols(&v, output_dim, input_dim);  
    
    free(s); 
    free(v); 
    free(u); 
    return 0; 
}

int read_from_file(char* filename,  double** arr) {

    printf("DEBUG: reading from file: %s\n", filename); 
    FILE* in = fopen(filename, "r"); 
    int dim = 0; 
    int i, j; 
    char buf[4096]; 
    char* ptr; 
    fgets(buf, sizeof(buf), in); 
    if (sscanf(buf, "%d", &dim) != 1) {
        return -1; 
    } 

    if(!dim) 
        return -1; 
    printf("DEBUG: matrix dimension: %d\n", dim); 
    *arr = (double*) calloc(dim * dim, sizeof(double));

    generalized_print_matrix(*arr, dim, dim); 
    for (i = 0; i<dim; i++) {
        if(!fgets(buf, sizeof(buf), in)) {
            free(*arr); 
            return -1; 
        }

        printf("DEBUG: %s", buf); 
        j = 0; 
        ptr = strtok(buf, " "); 

        while (ptr && j < dim) {
            //Hacky thing to make matrix real random and dense 
            //TODO get rid of this
            (*arr)[i * dim + j++] = atof(ptr); 
            ptr = strtok(NULL, " "); 
        }

    }

    printf("Finished reading matrix\n"); 

    generalized_print_matrix(*arr, dim, dim); 


    return 0;

}


int main(int argc, char** argv) {
    double* d = NULL; 
    int dim = 128; 
    int out_dim = 16; 
    read_from_file(argv[1], &d); 

    double* out = (double*) calloc(out_dim * out_dim, sizeof(double)); 

    lsa_transform(d, dim, out, out_dim); 

 
    return 0; 

}
