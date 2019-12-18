#include <math.h>

// color macros
#define C_R "\x1b[38;5;196m"
#define C_G "\x1b[38;5;120m"
#define C_RST "\x1b[0m"

//helper macros
#define CALCV(type, var, expr, unit, fmts) \
                        type var = expr; \
                        do {\
                            printf(#var" = " #expr" = "fmts " ["unit"]\n", var);\
                        } while(0)

// macro to calculate and create NEW double variable and print the computation with result
#define DCALCV(var, expr, unit) CALCV(double, var, expr, unit, "%lf")
#define _(var, expr, unit) DCALCV(var, expr, unit)

#define epscheck(var1, var2, eps) ((fabs(var1 - var2) < eps) ? 1 : 0)
// test whether two variables are "equal enough" in relation to allowed deviation @eps
#define check(var1, var2, eps) do {\
    int t = epscheck(var1, var2, eps);\
    const char* clr = (t) ? C_G: C_R;\
    printf("\t%sCHECK: " #var1 " ~= " #var2 " ? (eps = " #eps ") -> %s"C_RST"\n"\
           , clr, t ? "passed" : "failed");\
}while(0)
