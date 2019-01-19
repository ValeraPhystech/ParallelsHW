#ifndef MATRIX_H
#define MATRIX_H
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int readMatrix(int **matrix, int *rows, int *columns, const char *path) {
    FILE *file = fopen(path, "r");

    fscanf(file, "%d", rows);
    fscanf(file, "%d", columns);

    (*matrix) = (int *)malloc((*rows) * (*columns) * (int)sizeof(int));
    if ((*matrix) == NULL) {
        fclose(file);
        return 1;
    }

    for (int i = 0; i < (*rows) * (*columns); ++i) {
        int x;
        fscanf(file, "%d", &x);
        (*matrix)[i] = x;
    }

    fclose(file);
    return 0;
}

int printMatrix(int *matrix, const int rows, const int columns) {
    if (matrix == NULL) {
        return 1;
    }
    printf("%d %d\n", rows, columns);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            printf("%d ", matrix[i * columns + j]);
        }
        printf("\n");
    }
    return 0;
}

int writeMatrix(int **matrix, const int rows, const int columns, const char *path) {
    FILE *file = fopen(path, "w");


    fprintf(file, "%d %d\n", rows, columns);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            fprintf(file, "%d ", matrix[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 1;
}

int getSegmentStart(int proc_id, int num_procs, int rows, int columns) {
    return rows * columns / num_procs * proc_id;
}

int getSegmentEnd(int proc_id, int num_procs, int rows, int columns) {
    if (proc_id == num_procs - 1) {
        return rows * columns;
    }
    return getSegmentStart(proc_id + 1, num_procs, rows, columns);
}

int getSegmentLength(int proc_id, int num_procs, int rows, int columns) {
    return getSegmentEnd(proc_id, num_procs, rows, columns) - getSegmentStart(proc_id, num_procs, rows, columns);
}

int prodMPI(
    int proc_id,
    int num_procs,
    int rows1, int columns1, const int *matrix1,
    int rows2, int columns2, const int *matrix2,
    int *rows_res, int *columns_res, int **res_matrix
) {
    if (columns1 != rows2) {
        return 1;
    }

    *rows_res = getSegmentLength(proc_id, num_procs, rows1, 1);
    *columns_res = columns2;

    (*res_matrix) = (int *)calloc((*rows_res) * (*columns_res), sizeof(int));
    if (res_matrix == NULL) {
        return 1;
    }

    int segment_start = getSegmentStart(proc_id, num_procs, rows1, 1);

    #pragma omp parallel for collapse(2) num_threads(4)
    for (int i = segment_start; i < segment_start + (*rows_res); ++i) {
        for (int j = 0; j < (*columns_res); ++j) {
            for (int k = 0; k < columns1; ++k) {
                (*res_matrix)[(i - segment_start) * (*columns_res) + j] += matrix1[i * columns1 + k] * matrix2[k * columns2 + j];
            }
        }
    }
    return 0;
}

#endif