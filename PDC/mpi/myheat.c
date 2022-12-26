#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void bufToX(double** _x, double* buf, int x, int fromY, int toY)
{
    for (int i = fromY; i < toY; ++i)
        _x[i][x] = buf[i];
}

void xToBuf(double** _x, double* buf, int x, int fromY, int toY)
{
    for (int i = fromY; i < toY; ++i)
        buf[i] = _x[i][x];
}

int dbg = -1;

void partToBuf(double** x, double* buf, int xIdx, int yIdx, int p, int _h, int rank)
{
    for (int i = 0; i < _h; ++i) {
        for (int j = 0; j < _h; ++j) {
            if (rank == dbg)
                printf("rank %d    x %d %d to parts %d\n", rank, i + p * yIdx, j + p * xIdx, i * _h + j);
            buf[i * _h + j] = x[i + p * yIdx][j + p * xIdx];
        }
    }
    if (rank == dbg) fflush(stdout);
}

void partsToX(double** _x, double* parts, int p, int _h, int rank)
{
    if (rank == dbg)
        printf("toX %d %d\n", p, _h);
    for (int yIdx = 0; yIdx < p; ++yIdx) {
        for (int xIdx = 0; xIdx < p; ++xIdx) {
            if (rank == dbg)
                printf("Iter %d %d\n", yIdx, xIdx);
            for (int i = 0; i < _h; ++i) {
                for (int j = 0; j < _h; ++j) {
                    if (rank == dbg)
                        printf("parts %d to x %d %d\n", (yIdx * p + xIdx)*_h*_h + (_h * i + j), i + p * yIdx, j + p * xIdx);
                    _x[i + p * yIdx][j + p * xIdx] = parts[(yIdx * p + xIdx)*_h*_h + (_h * i + j)];
                }
            }
        }
    }
    if (rank == dbg) fflush(stdout);
}

int main(int argc, char* argv[]) {
    if (argc < 9 || argc > 10) {
        fprintf(stderr, "Usage: %s W H T1 T2 T3 T4 epsilon max_iters [output_file]\n", argv[0]);
        return 1;
    }

    int W = atoi(argv[1]); // grid width
    int H = atoi(argv[2]); // grid height

    double T1 = atof(argv[3]); // temperature at the top
    double T2 = atof(argv[4]); // temperature at the bottom
    double T3 = atof(argv[5]); // temperature at the left
    double T4 = atof(argv[6]); // temperature at the right

    double epsilon = atof(argv[7]); // convergence threshold
    int max_iters = atoi(argv[8]); // maximum number of iterations

    int rank, size, len;
    char host[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int p = sqrt(size);
    int _h = H / p;
    int xIdx = rank % p;
    int yIdx = rank / p;

    double** x = malloc(H * sizeof(double*));
    double** x0 = malloc(H * sizeof(double*));
    for (int i = 0; i < H; ++i) {
        x[i] = malloc(W * sizeof(double));
        x0[i] = malloc(W * sizeof(double));
    }

    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            x[i][j] = 0.0;
            x0[i][j] = 0.0;
        }
    }
    x[0][W/2] = T1;
    x0[0][W/2] = T1;
    x[H-1][W/2] = T2;
    x0[H-1][W/2] = T2;
    x[H/2][0] = T3;
    x0[H/2][0] = T3;
    x[H/2][W-1] = T4;
    x0[H/2][W-1] = T4;


    int fromX = _h * xIdx;
    if (xIdx == 0) ++fromX;
    int toX = _h * (xIdx + 1);
    if (xIdx == p - 1) --toX;
    int fromY = _h * yIdx;
    if (yIdx == 0) ++fromY;
    int toY = _h * (yIdx + 1);
    if (yIdx == p - 1) --toY;

    double* rBuf = malloc(_h * sizeof(double));
    double* lBuf = malloc(_h * sizeof(double));
    double* tBuf = malloc(_h * sizeof(double));
    double* bBuf = malloc(_h * sizeof(double));

    int iter = 0;
    double diff = 0.0;
    do {
        iter++;
        if (iter % 100 == 0) {
            diff = 0.0;
        }

        for (int i = fromY; i < toY; ++i) {
            for (int j = fromX; j < toX; ++j) {
                x[i][j] = (x0[i][j+1] + x0[i][j-1] + x0[i+1][j] + x0[i-1][j]) / 4.0;
                if (iter % 100 == 0) {
                    diff += (x0[i][j] - x[i][j]) * (x0[i][j] - x[i][j]);
                }
            }
        }
        double** tmp; tmp = x0; x0 = x; x = tmp;

        for (int i = fromY; i < toY; ++i) {
            for (int j = fromX; j < toX; ++j) {
                x[i][j] = (x0[i][j+1] + x0[i][j-1] + x0[i+1][j] + x0[i-1][j]) / 4.0;
                if (iter % 100 == 0) {
                    diff += (x0[i][j] - x[i][j]) * (x0[i][j] - x[i][j]);
                }
            }
        }
        tmp = x0; x0 = x; x = tmp;

        if (yIdx != 0)
            MPI_Send(x[fromY] + fromX, toX - fromX, MPI_DOUBLE, (yIdx - 1) * p + xIdx, 0, MPI_COMM_WORLD);
        if (yIdx != p - 1)
            MPI_Send(x[toY - 1] + fromX, toX - fromX, MPI_DOUBLE, (yIdx + 1) * p + xIdx, 0, MPI_COMM_WORLD);
        if (xIdx != 0) {
            xToBuf(x, lBuf, fromX, fromY, toY);
            MPI_Send(lBuf, toY - fromY, MPI_DOUBLE, (yIdx) * p + xIdx - 1, 0, MPI_COMM_WORLD);
        }
        if (xIdx != p - 1) {
            xToBuf(x, rBuf, toX - 1, fromY, toY);
            MPI_Send(rBuf, toY - fromY, MPI_DOUBLE, (yIdx) * p + xIdx + 1, 0, MPI_COMM_WORLD);
        }

        if (yIdx != 0)
            MPI_Recv(x[fromY - 1] + fromX, toX - fromX, MPI_DOUBLE, (yIdx - 1) * p + xIdx, MPI_ANY_TAG, MPI_COMM_WORLD, 0);
        if (yIdx != p - 1)
            MPI_Recv(x[toY] + fromX, toX - fromX, MPI_DOUBLE, (yIdx + 1) * p + xIdx, MPI_ANY_TAG, MPI_COMM_WORLD, 0);
        if (xIdx != 0) {
            MPI_Recv(lBuf, toY - fromY, MPI_DOUBLE, (yIdx) * p + xIdx - 1, MPI_ANY_TAG, MPI_COMM_WORLD, 0);
            bufToX(x, lBuf, fromX - 1, fromY, toY);
        }
        if (xIdx != p - 1) {
            MPI_Recv(rBuf, toY - fromY, MPI_DOUBLE, (yIdx) * p + xIdx + 1, MPI_ANY_TAG, MPI_COMM_WORLD, 0);
            bufToX(x, rBuf, fromX + 1, fromY, toY);
        }

        if (iter % 100 == 0) {
            diff = sqrt(diff);
            if (diff <= epsilon) {
                //break;
            }
        }
    } while (iter < max_iters);

    MPI_Barrier(MPI_COMM_WORLD);

    double* buf1 = malloc(_h * _h * sizeof(double));
    partToBuf(x0, buf1, xIdx, yIdx, p, _h, rank);
    double* buf2 = malloc(H * W * sizeof(double));
    MPI_Gather(buf1, _h * _h, MPI_DOUBLE,
               buf2, H * W, MPI_DOUBLE,
               0, MPI_COMM_WORLD);
    partsToX(x0, buf2, p, _h, rank);


    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == dbg) {
        for (int i = 0; i < H; ++i) {
            for (int j = 0; j < W; ++j) {
                printf(" %12.6f", buf2[i * H + j]);
            }
            printf("\n");
        }

        printf("\n");
        printf("\n");
        printf("\n");

        for (int i = 0; i < H; ++i) {
            for (int j = 0; j < W; ++j) {
                printf(" %12.6f", x0[i][j]);
            }
            printf("\n");
        }
        fflush(stdout);
    }


    MPI_Barrier(MPI_COMM_WORLD);
    //printf("%d PASS\n", rank); fflush(stdout);

    partToBuf(x0, buf1, xIdx, yIdx, p, _h, rank);
    MPI_Gather(buf1, _h * _h, MPI_DOUBLE,
               buf2, H * W, MPI_DOUBLE,
               0, MPI_COMM_WORLD);
    partsToX(x0, buf2, p, _h, rank);

    diff = 0;
    for (int i = 1; i < H - 1; ++i) {
        for (int j = 1; j < W - 1; ++j) {
            diff += (x0[i][j] - x[i][j]) * (x0[i][j] - x[i][j]);
        }
    }

    printf("Iter %d, diff %12.8f \n", iter, diff);

    //printf("%d PASS\n", rank); fflush(stdout);
    if (rank == 0) {
        if (argc == 10) {
            FILE* output_file = fopen(argv[9], "w");
            for (int i = 0; i < H; ++i) {
                for (int j = 0; j < W; ++j) {
                    fprintf(output_file, " %16.6f", x0[i][j]);
                }
                fprintf(output_file, "\n");
            }
            fclose(output_file);
        }
    }
    //printf("%d PASS\n", rank); fflush(stdout);
    for (int i = 0; i < H; ++i) {
      free(x[i]);
      free(x0[i]);
    }
    free(x);
    free(x0);
    printf("%d PASS\n", rank); fflush(stdout);

    MPI_Finalize();
    printf("%d PASS\n", rank); fflush(stdout);

    return 0;
}
