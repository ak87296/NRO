#include <iostream>
#include <cmath>

// Definicija konstante M_PI, ker še ni definirana
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Funkcija za izračun arctan(x) z uporabo Taylorjeve vrste
double calcAtan(double* x, int* N_steps) {
    double sum = 0.0;
    double term = *x; // Prvi člen v vrsti
    int sign = 1;

    for (int n = 0; n < *N_steps; ++n) {
        sum += sign * term / (2 * n + 1);
        term *= (*x) * (*x); // Naslednji člen
        sign = -sign;
    }

    return sum;
}

// Funkcija e^(3x) * arctan(x/2)
double function(double x) {
    double half_x = x / 2; 
    int N_steps = 20; // Večje število členov za natančnost
    return std::exp(3 * x) * calcAtan(&half_x, &N_steps);
}

// Trapezna metoda za integracijo
double trapezoidalRule(double a, double b, int n) {
    double h = (b - a) / n; // Korak
    double sum = function(a) + function(b);

    for (int i = 1; i < n; ++i) {
        double x = a + i * h;
        sum += 2 * function(x); // Vmesne točke štejemo dvojno
    }

    return (h / 2) * sum; // Končni rezultat
}

int main() {
    // Meje integracije in število intervalov
    double a = 0; // Spodnja meja
    double b = M_PI / 4; // Zgornja meja
    int n = 1000; // Število intervalov za boljšo natančnost

    // Izračun integrala z uporabo trapezne metode
    double result = trapezoidalRule(a, b, n);

    // Izpis rezultata
    std::cout << "Vrednost integrala je: " << result << std::endl;

    return 0;
}
