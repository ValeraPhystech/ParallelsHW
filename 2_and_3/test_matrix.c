#include <stdio.h>

#include "matrix.h"

int main(int argc, const char *argv[]) {
    int **matrix1;
    int row1;
    int column1;
    readMatrix(&matrix1, &row1, &column1, argv[1]);
    int **matrix2;
    int row2;
    int column2;
    readMatrix(&matrix2, &row2, &column2, argv[2]);
    int **matrix = (int **)malloc(row1 * sizeof(int *));
    int row = row1;
    int column = column2;
    for (int i = 0; i < row; ++i) {
        matrix[i] = (int *)malloc(column * sizeof(int));
        for (int j = 0; j < column; ++j) {
            matrix[i][j] = 0;
        }
    }
    
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < column; ++j) {
            for (int k = 0; k < row2; ++k) {
                matrix[i][j] += matrix1[i][k]*matrix2[k][j];
            }
        }
    }

    writeMatrix(matrix, row, column, argv[3]);


}