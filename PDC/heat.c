#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

    int iter = 0;
    double diff = 0.0;
    do {
        iter++;
        if (iter % 100 == 0) {
            diff = 0.0;
        }
        for (int i = 1; i < H-1; ++i) {
            for (int j = 1; j < W-1; ++j) {
                x[i][j] = (x0[i][j+1] + x0[i][j-1] + x0[i+1][j] + x0[i-1][j]) / 4.0;
                if (iter % 100 == 0) {
                    diff += (x0[i][j] - x[i][j]) * (x0[i][j] - x[i][j]);
                }
            }
        }
        double** tmp; tmp = x0; x0 = x; x = tmp;
        if (iter % 100 == 0) {
            diff = sqrt(diff);
            if (diff <= epsilon) {
                break;
            }
        }
    } while (iter < max_iters);

    printf("Iter %d, diff %12.8f \n", iter, diff);

    if (argc == 10) {
        FILE* output_file = fopen(argv[9], "w");
        for (int i = 0; i < H; ++i) {
            for (int j = 0; j < W; ++j) {
                fprintf(output_file, "%12.6f", x0[i][j]);
            }
            fprintf(output_file, "\n");
        }
        fclose(output_file);
    }

    for (int i = 0; i < H; ++i) {
      free(x[i]);
      free(x0[i]);
    }
    free(x);
    free(x0);

    return 0;
}
