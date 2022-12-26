#include <stdio.h>
#include <math.h>


extern void myhypot(double x, double y, double* r);


int main() {
    double x, y, r = 42.0;
    scanf("%lf%lf", &x, &y);
    myhypot(x, y, &r);
    printf("%g %g\n", r, sqrt(x*x + y*y));
}






