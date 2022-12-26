#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <atomic>


std::atomic_int id_to_print;

void
func(int id, char *filename)
{
    std::fstream in(filename);
    uint64_t a, b, n;
    in >> a >> b >> n;
    a %= n;
    b %= n;
    uint64_t res;
    b = n - b;
    if (a >= b) {
        res = a - b;
    } else {
        res = n - b + a;
    }
    
    while (id_to_print != id) {
        std::this_thread::yield();
    }
    std::cout << res << std::endl;
    id_to_print = id - 1;
}

int
main(int argc, char **argv)
{
    id_to_print = argc - 1;
    std::vector<std::thread> threads;
    for (int i = 1; i < argc; ++i) {
        threads.emplace_back(func, i, argv[i]);
    }
    for (auto &i: threads) {
        i.join();
    }
}

