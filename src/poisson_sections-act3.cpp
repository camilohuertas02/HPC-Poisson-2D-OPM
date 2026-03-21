// poisson_sections.cpp
// Actividad 3: Uso de sections para inicialización paralela

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <omp.h>

const double x0 = 1.0, xf = 2.0;
const double y0 = 0.0, yf = 2.0;
const double TOL = 1e-6;

// Modificamos initialize_grid para quitar el cálculo de h, k y el resize
// (Esto ahora se hará en el main antes de las secciones)
void initialize_boundaries(int M, int N, std::vector<std::vector<double>>& T, double h, double k) {
    // Frontera inferior (y = 0) y superior (y = 2)
    for (int i = 0; i <= M; ++i){
        double x = x0 + i*h;
        T[i][0] = std::pow(x, 2);
        T[i][N] = std::pow(x - 2.0, 2);
    }
    // Frontera izquierda(x=1) y derecha (x=2)
    for (int j = 0; j <= N; ++j){
        double y = y0 + j*k;
        T[0][j] = std::pow(1.0 - y, 2);
        T[M][j] = std::pow(2.0 - y, 2);
    }
}

// poisson_source se mantiene igual
void poisson_source(int M, int N, std::vector<std::vector<double>>& source, double h, double k) {
    for (int i = 0; i <= M; ++i){
        for (int j = 0; j <= N; ++j){
            source[i][j] = 4.0;
        }
    }       
}

void solve_poisson(std::vector<std::vector<double>>& T, const std::vector<std::vector<double>>& source, int M, int N, double h, double k) {
    double delta = 1.0;
    while (delta > TOL) {
        delta = 0.0;
        #pragma omp parallel for collapse(2) reduction(max:delta)
        for (int i = 1; i < M; ++i) {
            for (int j = 1; j < N; ++j) {
                double T_new = (
                    ((T[i + 1][j] + T[i - 1][j]) * k * k) +
                    ((T[i][j + 1] + T[i][j - 1]) * h * h) -
                    (source[i][j] * h * h * k * k)) /
                    (2.0 * (h * h + k * k));

                delta = std::max(delta, std::abs(T_new - T[i][j]));
                T[i][j] = T_new;
            }
        }
    }
}

int main() {
    int M = 50, N = 50;
    double h = (xf - x0) / M;
    double k = (yf - y0) / N;
    std::vector<std::vector<double>> T(M + 1, std::vector<double>(N + 1, 0.0));
    std::vector<std::vector<double>> source(M + 1, std::vector<double>(N + 1, 0.0));

    double start_time = omp_get_wtime();

    // Actividad 3: Uso de sections
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            initialize_boundaries(M, N, T, h, k);
        }
        
        #pragma omp section
        {
            poisson_source(M, N, source, h, k);
        }
    }

    double end_time = omp_get_wtime();
    std::cout << "Tiempo de inicializacion (sections): " << end_time - start_time << " segundos.\n";

    solve_poisson(T, source, M, N, h, k);
    std::cout << "Simulación completada." << std::endl;
    return 0;
}
