#include <thread>
#include <atomic>
#include <iostream>
#include <mutex>
#include <vector>
#include <fstream>

std::mutex can_block_row;

void func(std::vector<std::vector<int> >& matrix, std::vector<std::mutex>& r_mutex, std::vector<std::mutex>& c_mutex,
                int iter, int row, int col, int incr) {
    if (row == -1) {
        c_mutex[col].lock();
        for (int k = 0; k < iter; ++k) {
            for (size_t i = 0; i < matrix.size(); ++i) {
                can_block_row.lock();
                r_mutex[i].lock();
                matrix[i][col] += incr;
                r_mutex[i].unlock();
                can_block_row.unlock();
            }
        }
        c_mutex[col].unlock();
    } else {
        can_block_row.lock();
        r_mutex[row].lock();
        for (int k = 0; k < iter; ++k) {
            for (size_t i = 0; i < matrix[0].size(); ++i) {
                c_mutex[i].lock();
                can_block_row.unlock();
                matrix[row][i] += incr;
                can_block_row.lock();
                c_mutex[i].unlock();
            }
        }
        r_mutex[row].unlock();
        can_block_row.unlock();
    }
}

int main() {
    std::ifstream cin("input");
    int rows, cols, thrs;
    cin >> rows >> cols >> thrs;
    std::vector<std::vector<int> > matrix;
    std::vector<std::mutex> r_mutex(rows);
    std::vector<std::mutex> c_mutex(cols);
    for (int i = 0; i < rows; ++i) {
        std::vector<int> row;
        for (int j = 0; j < cols; ++j) {
            row.push_back(0);
        }
        matrix.push_back(row);
    }
    std::vector<std::thread> threads;
    for (int i = 0; i < thrs; ++i) {
        int iter, row, col, incr;
        cin >> iter >> row >> col >> incr;
        threads.emplace_back(func, std::ref(matrix), std::ref(r_mutex), std::ref(c_mutex), iter, row, col, incr);
    }
    for (int i = 0; i < thrs; ++i) {
        threads[i].join();
    }
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            std::cout << matrix[i][j] << ' ';
        }
        std::cout << std::endl;
    }
}
