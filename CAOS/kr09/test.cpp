#include <iostream>
#include <atomic>

int main()
{
    std::atomic<int> i;
    i = 0;
    i += 1;
    std::cout << i;
}


