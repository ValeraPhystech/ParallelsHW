#ifndef MATRIX_H
#define MATRIX_H
#include <stdlib.h>

int readMatrix(int ***matrix, int *rows, int *columns, const char *path) {
    FILE *file = fopen(path, "r");

    fscanf(file, "%d", rows);
    fscanf(file, "%d", columns);
    
    *(matrix) = (int **)malloc((*rows) * (int)sizeof(int *));
    if ((*matrix) == NULL) {
        fclose(file);
        return 0;
    }
    
    for (int i = 0; i < (*rows); ++i) {
        (*matrix)[i] = (int *)malloc((*columns) * (int)sizeof(int));
        if (((*matrix)[i]) == NULL) {
            for (int j = 0; j < i; ++j) {
                free((*matrix)[j]);
            }
            fclose(file);
            return 0;
        }
        for (int j = 0; j < (*columns); ++j) {
            int x;
            fscanf(file, "%d", &x);
            (*matrix)[i][j] = x;
        }
    }

    fclose(file);
    return 1;
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

#endif