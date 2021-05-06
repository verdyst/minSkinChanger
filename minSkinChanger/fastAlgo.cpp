#include "pch.h"
#include "fastAlgo.h"

void poly_root_solver()
{
    float M = 12.0;
    float b = M / 2;
    int m = 0;
    do
    {
        m++;
        b = b - ((b * b - M) / (2 * b));
    } while (fabs(b * b - M) > 1E-5 && m < 10);

    // the polynomial is x^2 - m
}