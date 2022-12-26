#include <thread>
#include <string>
#include <iomanip>
#include <future>
#include <iostream>
#include <cstdlib>

using namespace std;

long long func(unsigned int thr_num, long long it_num) {
    long long circ_res = 0;
    for (int i = 0; i < it_num; ++i) {
        double x = (rand_r(&thr_num) % 10001) / 10000.0;
        double y = (rand_r(&thr_num) % 10001) / 10000.0;
        if (x*x + y*y <= 1) {
            circ_res += 1;
        }
    }
    return circ_res;
}

int main(int argc, char** argv) {
    unsigned int num_thr = atoi(argv[1]);
    long long num_it = atoi(argv[2]);
    double res = 0;
    for (unsigned int i = 0; i < num_thr; ++i) {
        auto handle = async(launch::async, func, i, num_it);
        long long cur_res = handle.get();
        res += cur_res;
    }
    res = res * 1.0 / (num_it);
    res /= num_thr; 
    std::cout << setprecision(5) << fixed << res << endl;
}
