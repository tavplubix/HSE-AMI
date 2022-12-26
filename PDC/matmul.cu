#include <chrono>
#include <iostream>

#include <cuda.h>

using namespace std::chrono;
using namespace std;

void gen_matrix(double *matrix, int rows, int cols) {
    for (int i = 0; i < rows * cols; ++i) {
        matrix[i] = double(rand()) / RAND_MAX;
    }
}

// === DO NOT REMOVE THIS LINE ===

// Insert your matrix multiplication kernel below

#define IDX(row, col, len) ((row)*(len)+(col))

// A - KxM, B - MxN, C - KxN
__host__ void matmul_cpu(double *A, double *B, double *C, int K, int M, int N) {
    for (int i = 0; i < K; ++i) {
        for (int j = 0; j < N; ++j) {
            int c_idx = IDX(i, j, N);
            C[c_idx] = 0.0;
            for (int k = 0; k < M; ++k) {
                int a_idx = IDX(i, k, M);
                int b_idx = IDX(k, j, N);
                C[c_idx] += A[a_idx] * B[b_idx];
            }
        }
    }
}

// === DO NOT REMOVE THIS LINE ===

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " K M N" << endl;
        return 1;
    }
    unsigned K = atoi(argv[1]);
    unsigned M = atoi(argv[2]);
    unsigned N = atoi(argv[3]);
    srand(K + M + N);

    cout << "A: " << K << "x" << M << endl;
    cout << "B: " << M << "x" << N << endl;
    cout << "C: " << K << "x" << N << endl;
    cout << endl;

    double *A = new double[K * M];
    double *B = new double[M * N];

    gen_matrix(A, K, M);
    gen_matrix(B, M, N);

    double *C = new double[K * N];
    double *cpuC = new double[K * N];

    high_resolution_clock::time_point total_start = high_resolution_clock::now();

    // === DO NOT REMOVE THIS LINE ===

    // Insert your code that runs C=A*B on GPU below

    matmul_cpu(A, B, cpuC, K, M, N);

    // === DO NOT REMOVE THIS LINE ===

    high_resolution_clock::time_point total_end = high_resolution_clock::now();
    double total_time = duration_cast<duration<double>>(total_end - total_start).count();

    cout << "Total (kernel+copy) time: " << total_time << endl;

    double maxdiff = 0;
    for (unsigned i = 0; i < K*N; ++i)
            maxdiff = std::max(std::abs(cpuC[i] - C[i]), maxdiff);

    cout << "Max diff " << maxdiff << endl;

    delete A;
    delete B;
    delete C;
}
