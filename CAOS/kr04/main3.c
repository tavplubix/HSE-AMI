#include <stdio.h>
#include <math.h>


extern void myexp(double x, double *r);


int main() {
    double x;
    double r;
    scanf("%lf", &x);
    myexp(x, &r);
    printf("%lf\n%lf\n", exp(x), r);
}




