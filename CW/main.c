#include <stdio.h>
#include <math.h>
#include <string.h>

// Определение функций f1, f2 и f3
double f1(double x) {
    return 0.6 * x + 3;
}

double f2(double x) {
    return pow((x - 2), 3) - 1;
}

double f3(double x) {
    return 3 / x;
}

// Функции для нахождения производных
double df1(double x) {
    return 0.6;
}

double df2(double x) {
    return 3 * pow((x - 2), 2);
}

double df3(double x) {
    return -3 / (x * x);
}

// Функция для нахождения корня
double root(double (*f)(double), double (*g)(double), double a, double b, double eps) {
    double mid;
    while (b - a > eps) {
        mid = (a + b) / 2.0;
        if ((f(a) - g(a)) * (f(mid) - g(mid)) <= 0) {
            b = mid;
        } else {
            a = mid;
        }
    }
    return mid;
}

// Функция для вычисления определенного интеграла
double integral(double (*f)(double), double a, double b, double eps) {
    int n = 1;
    double h = b - a;
    double integral = (f(a) + f(b)) * h / 2.0;

    double prev_integral;
    do {
        prev_integral = integral;
        n *= 2;
        h /= 2.0;
        integral = 0.0;
        for (int i = 0; i < n; i++) {
            integral += f(a + i * h) + f(a + (i + 1) * h);
        }
        integral *= h / 2.0;
    } while (fabs(integral - prev_integral) > eps);

    return integral;
}

// Функция для вывода справки
void print_help() {
    printf("Usage:\n");
    printf("  --help            Display this information\n");
    printf("  --root            Find the intersection points of the functions\n");
    printf("  --integral        Compute the area under the curves\n");
    printf("  --test            Run tests for root and integral functions\n");
}

// Основная программа
int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    if (strcmp(argv[1], "--help") == 0) {
        print_help();
    } else if (strcmp(argv[1], "--root") == 0) {
        // Поиск точек пересечения функций
        double x1 = root(f1, f2, 0, 4, 1e-6);
        double x2 = root(f2, f3, 0.1, 10, 1e-6);
        double x3 = root(f1, f3, -10, 10, 1e-6);

        printf("Roots:\n");
        if (!isnan(x1)) {
            printf("Root 1 (f1 = f2): x = %f\n", x1);
        }
        if (!isnan(x2)) {
            printf("Root 2 (f2 = f3): x = %f\n", x2);
        }
        else {
            printf("No root found for f2 and f3 in the given interval.\n");
        }
        if (!isnan(x3)) {
            printf("Root 3 (f1 = f3): x = %f\n", x3);
        }
    } else if (strcmp(argv[1], "--integral") == 0) {
        // Вычисление площадей под кривыми
        double area1 = integral(f1, 1, 4, 1e-6);
        double area2 = integral(f2, 1, 4, 1e-6);
        double area3 = integral(f3, 1, 4, 1e-6);
        printf("Areas:\n");
        printf("Integral of f1: %f\n", area1);
        printf("Integral of f2: %f\n", area2);
        printf("Integral of f3: %f\n", area3);
    } else if (strcmp(argv[1], "--test") == 0) {
        // Тестирование функций root и integral
        printf("test...\n");
        double testRoot = root(f1, f2, 0, 4, 1e-6);
        printf("Test root: %f\n", testRoot);
        double testIntegral = integral(f1, 0, 1, 1e-6);
        printf("Test integral: %f\n", testIntegral);
    } else {
        print_help();
        return 1;
    }

    return 0;
}
