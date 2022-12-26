#include <stdio.h>
enum {SIZE = 100000000};
double arr[SIZE];

int main() {
    FILE *f = fopen("test", "wb");
    
    for (int i = 0; i < SIZE; ++i)
        arr[i] = i;
    fwrite(arr, SIZE, sizeof(double), f);
    fclose(f);
}


