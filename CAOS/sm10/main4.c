#include <stdio.h>

extern float dot_product(int n, const float* x, const float* y);

float foo(float a) {
    return a*a;
}


int main() {
    float x[64];
    float y[64];
    int n = 0;
    while (1) {
        float f;
        scanf("%f", &f);
        if (f == 42)
            break;
        x[n++] = f;
    }
    for (int i = 0; i < n; ++i)
        scanf("%f", y + i);

    printf("%f\n", dot_product(n, x, y));
    float dp = 0;
    for (int i = 0; i < n; ++i) 
        dp += x[i]*y[i];
    printf("%f\n", dp);

}




