#ifdef _OPENMP
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "validator.h"
#include "matrix.h"

int main(int argc, const char *argv[]) {
    if (argc != 5) {
        printf("Invalid number of parameters.\n");
        return 1;
    }
    const char *matrix1 = argv[1];
    const char *matrix2 = argv[2];
    const int threads_number = atoi(argv[3]);
    const char *res_file = argv[4];
    if (!validateFile(matrix1) || !validateFile(matrix2) || !validateFileWrite(res_file)) {
        printf("Some issues with files.\n");
        return 1;
    }
    if (threads_number <= 0) {
        printf("Invalid number of threads.\n");
        return 1;
    }

    int row_1 = 0, column_1 = 0, row_2 = 0, column_2 = 0;
    int **matrix_1 = NULL;
    int **matrix_2 = NULL;

    if (!readMatrix(&matrix_1, &row_1, &column_1, matrix1)) {
        printf("Error while reading matrix.\n");
        return 1;
    }
    if (!readMatrix(&matrix_2, &row_2, &column_2, matrix2)) {
        printf("Error while reading matrix.\n");
        for (int i = 0; i < row_1; ++i) {
            free(matrix_1[i]);
        }
        free(matrix_1);
        return 1;
    }
    
    if (column_1 != row_2) {
        printf("Matrixes cant be multiplied!\n");
        for (int i = 0; i < row_1; ++i) {
            free(matrix_1[i]);
        }
        for (int i = 0; i < row_2; ++i) {
            free(matrix_2[i]);
        }
        free(matrix_1);
        free(matrix_2);
        return 1;
    }

    int **res_matrix = (int **)malloc(row_1 * sizeof(int *));
    if (res_matrix == NULL) {
        printf("Allocation error.\n");
        for (int i = 0; i < row_1; ++i) {
            free(matrix_1[i]);
        }
        for (int i = 0; i < row_2; ++i) {
            free(matrix_2[i]);
        }
        free(matrix_1);
        free(matrix_2);
        return 1;
    }
    for (int i = 0; i < row_1; ++i) {
        res_matrix[i] = (int *)malloc(column_2 * sizeof(int));
        if (res_matrix[i] == NULL) {
            printf("Allocation error.\n");
            for (int j = 0; j < row_1; ++j) {
                free(matrix_1[j]);
            }
            for (int j = 0; j < row_2; ++j) {
                free(matrix_2[j]);
            }
            for (int j = 0; j < i; ++j) {
                free(res_matrix[j]);
            }
            free(res_matrix);
            free(matrix_1);
            free(matrix_2);
            return 1;
        }
        for (int j = 0; j < column_2; ++j) {
            res_matrix[i][j] = 0;
        }
    }
    int i = 0;
    int j = 0;
    int k = 0;
    double start_time = omp_get_wtime();
    printf("START:\t%lf\n", start_time);

    omp_set_nested(1);
    omp_set_num_threads(threads_number);
    #pragma omp parallel shared(matrix_1, matrix_2, res_matrix) private(i, j, k)
    {
        
        #pragma omp for
            for (i = 0; i < row_1; ++i) {
                for (j = 0; j < column_2; ++j) {
                            for (k = 0; k < row_2; ++k) {
                                res_matrix[i][j] += matrix_1[i][k] * matrix_2[k][j];
                            }
                    
                }
        
            }
    }
    double end_time = omp_get_wtime();
    printf("TIME:\t%lf\n", (end_time - start_time));
    writeMatrix(res_matrix, row_1, column_2, res_file);
    for (int i = 0; i < row_1; ++i) {
        free(matrix_1[i]);
    }
    for (int i = 0; i < row_2; ++i) {
        free(matrix_2[i]);
    }
    for (int i = 0; i < row_1; ++i) {
        free(res_matrix[i]);
    }
    free(res_matrix);
    free(matrix_1);
    free(matrix_2);
    return 0;
}

#endif