#include <mpi.h>
#include <stdio.h>
#include <omp.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "matrix.h"

int isValidFile(const char *path) {
    struct stat fileStat = {0};
    
    stat(path, &fileStat);
    if (S_ISREG(fileStat.st_mode)) {
        return 1;
    }
    
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Wrong number of parameters.\n");
        return 1;
    }
    if (!isValidFile(argv[1]) || !isValidFile(argv[2])) {
        printf("File is invalid.\n");
        return 1;
    }

    int err_code;
    if ((err_code = MPI_Init(NULL, NULL))) {
        MPI_Abort(MPI_COMM_WORLD, err_code);
    }

    int proc_id = -1, num_procs = -1;
    int rows1, rows2, columns1, columns2;

    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

    int* matrix1 = NULL;
    int* matrix2 = NULL;

    if (!proc_id) {
        if (readMatrix(&matrix1, &rows1, &columns1, argv[1])) {
            printf("Problem whle reading matrix 1.\n");
            return 1;
        }
        if (readMatrix(&matrix2, &rows2, &columns2, argv[2])) {
            free(matrix1);
            printf("Problem while reading matrix 2.\n");
            return 1;
        }
        
    }

    double start_time = omp_get_wtime();

    int sizes[4] = {rows1, columns1, rows2, columns2};
    MPI_Bcast(sizes, 4, MPI_INT, 0, MPI_COMM_WORLD);
    rows1 = sizes[0];
    columns1 = sizes[1];
    rows2 = sizes[2];
    columns2 = sizes[3];

    if (proc_id) {
        matrix1 = (int *)malloc(rows1 * columns1 * sizeof(int));
        if (matrix1 == NULL) {
            printf("Allocation failure.\n");
            return 1;
        }
        matrix2 = (int *)malloc(rows2 * columns2 * sizeof(int));
        if (matrix2 == NULL) {
            printf("Allocation failure.\n");
            free(matrix1);
            return 1;
        }
    }

    MPI_Bcast(matrix1, rows1 * columns1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(matrix2, rows2 * columns2, MPI_INT, 0, MPI_COMM_WORLD);
    
    int* res_matrix = NULL;
    int rows_res = 0, columns_res = 0;

    if (prodMPI(proc_id, num_procs, rows1, columns1,
                matrix1, rows2, columns2, matrix2,
                &rows_res, &columns_res, &res_matrix)) {
        return 1;
    }

    if (proc_id) {
        if (MPI_Send(res_matrix, rows_res * columns_res, 
                            MPI_INT, 0, MPI_SUCCESS, MPI_COMM_WORLD)) {
            return 1;
        }
    } else {
        int total_recieved_res = rows_res * columns_res;
        int* total_buffer_res = NULL;
        total_buffer_res = (int *)malloc(rows1 * columns2 * sizeof(int));
        if (total_buffer_res == NULL) {
            if (matrix1) {
                free(matrix1);
            }
            if (matrix2) {
                free(matrix2);
            }
            if (res_matrix) {
                free(res_matrix);
            }
            return 1;
        }

        for (int i = 0; i < total_recieved_res; ++i) {
            total_buffer_res[i] = res_matrix[i];
        }

        for (int id = 1; id < num_procs; ++id) {
            if (MPI_Recv(total_buffer_res + total_recieved_res,
                            getSegmentLength(id, num_procs, rows1, 1) * columns2,
                            MPI_INT, id, MPI_SUCCESS, MPI_COMM_WORLD, MPI_STATUS_IGNORE)) {
                if (matrix1) {
                    free(matrix1);
                }
                if (matrix2) {
                    free(matrix2);
                }
                if (res_matrix) {
                    free(res_matrix);
                }
                if (total_buffer_res) {
                    free(total_buffer_res);
                }
                return 1;
            }
            total_recieved_res += 1;
        }
        double end_time = omp_get_wtime();
        printf("Time:\t%lf\n\n", end_time - start_time);
        //printMatrix(total_buffer_res, rows1, columns2);
        free(total_buffer_res);
    }
    printf("Proc id:\t%d\n", proc_id);
    free(matrix1);
    free(matrix2);
    free(res_matrix);
    MPI_Finalize();
    return 0;
}
