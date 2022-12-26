


#include <stdio.h>
const double EPS = 1e-8;
double foo(double L, double R, double (*f)(double))
{
    while (R - L > EPS)
    {
        double m1 = (2 * L + R) / 3;
        double m2 = (L + 2 * R) / 3;
        if (f(m2) > f(m1))
            L = m1;
        else
            R = m2;
    }
    return L;
}
double func(double x)
{
    return -x * x + 2 * x + 1;
}
int main(void)
{
    double L, R;
    scanf("%lf%lf", &L, &R);
    printf("%0.3lf\n", foo(L, R, func));
    return 0;
}


