#include <chrono>
#include <iostream>
#include <random>
#include <vector>

typedef std::chrono::milliseconds Milliseconds;
typedef std::chrono::steady_clock Clock;
typedef Clock::time_point Time;

const unsigned N = 8000;
const unsigned M = 8000;
// const unsigned N = 4;
// const unsigned M = 16000000;
const unsigned ITER = 20;

void init(std::vector<std::vector<double>>& A, std::vector<double>& x, std::vector<double>& y) {
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0, 1);

    for (unsigned i = 0; i < N; i++) {
        double a = dist(e2);
        for (unsigned j = 0; j < M; j++) {
            A[i][j] = a * i + j;
        }
        y[i] = 0;
    }
    for (unsigned j = 0; j < M; j++) {
        x[j] = dist(e2);
    }
}

void multiply(std::vector<std::vector<double>>& A, std::vector<double>& x, std::vector<double>& y) {
    for (unsigned i = 0; i < N; i++) {
        for (unsigned j = 0; j < M; j++) {
            y[i] += A[i][j] * x[j];
        }
    }
}

void multiply2(std::vector<std::vector<double>>& A, std::vector<double>& x, std::vector<double>& y) {
    for (unsigned j = 0; j < M; j++) {
        for (unsigned i = 0; i < N; i++) {
            y[i] += A[i][j]*x[j];
        }
    }
}

int main() {
    std::vector<std::vector<double>> A(N, std::vector<double>(M, 0));
    std::vector<double> x(M, 0), y(N, 0);

    init(A, x, y);

    Time start = Clock::now();

    for (unsigned i = 0; i < ITER; i++) {
        multiply(A, x, y);
    }

    unsigned long mult_time =
        std::chrono::duration_cast<Milliseconds>(Clock::now() - start).count();
    std::cout << "Time: " << mult_time << "\n";
}

