#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <omp.h>
#include <string>

/**
 * @file poisson_sections-act3.cpp
 * @brief Resolución de Poisson 2D usando parallel sections para la inicialización.
 * Divide las tareas de configuración (fronteras y fuente) en secciones paralelas
 * para reducir el tiempo de setup inicial.
 */

const double x_start = 1.0, x_end = 2.0;
const double y_start = 0.0, y_end = 2.0;

// Contador de iteraciones global
int iterations = 0;

void initialize_boundaries(int M, int N, std::vector<std::vector<double>>& T, double h, double k) {
    for (int i = 0; i <= M; ++i){
        double x = x_start + i*h;
        T[i][0] = std::pow(x, 2);
        T[i][N] = std::pow(x - 2.0, 2);
    }
    for (int j = 0; j <= N; ++j){
        double y = y_start + j*k;
        T[0][j] = std::pow(1.0 - y, 2);
        T[M][j] = std::pow(2.0 - y, 2);
    }
}

void poisson_source(int M, int N, std::vector<std::vector<double>>& source) {
    for (int i = 0; i <= M; ++i){
        for (int j = 0; j <= N; ++j){
            source[i][j] = 4.0;
        }
    }       
}

void solve_poisson(std::vector<std::vector<double>>& T,
                   const std::vector<std::vector<double>>& source,
                   int M, int N, double h, double k, double TOL_param) 
{
    double delta = 1.0;
    while (delta > TOL_param) {
        delta = 0.0;
        #pragma omp parallel for collapse(2) reduction(max:delta)
        for (int i = 1; i < M; ++i) {
            for (int j = 1; j < N; ++j) {
                double T_old = T[i][j];
                T[i][j] = (
                    ((T[i + 1][j] + T[i - 1][j]) * k * k) +
                    ((T[i][j + 1] + T[i][j - 1]) * h * h) -
                    (source[i][j] * h * h * k * k)) /
                    (2.0 * (h * h + k * k));

                double diff = std::abs(T[i][j] - T_old);
                if (diff > delta) delta = diff;
            }
        }
        iterations++;
    }
}

void export_to_file(const std::vector<std::vector<double>>& T,
                    double h, double k, int M, int N,
                    const std::string& filename) 
{
    std::ofstream file(filename);
    if (!file.is_open()) return;

    for (int i = 0; i <= M; ++i){
        for (int j = 0; j <= N; ++j){
            double x = x_start + i*h;
            double y = y_start + j*k;
            file << x << "\t" << y << "\t" << T[i][j] << "\n";
        }
    }
    file.close();
}

int main(int argc, char* argv[]) {
    // Parámetros por defecto
    int M = 50, N = 50;
    double TOL_val = 1e-6;

    // Captura de argumentos desde el Makefile
    if (argc >= 4) {
        M = std::stoi(argv[1]);
        N = std::stoi(argv[2]);
        TOL_val = std::stod(argv[3]);
    }

    double h = (x_end - x_start) / M;
    double k = (y_end - y_start) / N;
    
    std::vector<std::vector<double>> T(M + 1, std::vector<double>(N + 1, 0.0));
    std::vector<std::vector<double>> source(M + 1, std::vector<double>(N + 1, 0.0));

    double start_time = omp_get_wtime();

    // Actividad 3: División de tareas independientes
    #pragma omp parallel sections
    {
        #pragma omp section
        initialize_boundaries(M, N, T, h, k);

        #pragma omp section
        poisson_source(M, N, source);
    }

    solve_poisson(T, source, M, N, h, k, TOL_val);

    double end_time = omp_get_wtime();

    std::string filename = "data/solucion_poisson_sections.dat";
    export_to_file(T, h, k, M, N, filename);

    // Salida estandarizada impecable
    std::cout << "Programa: poisson_sections-act3 | Tiempo_total: " 
              << (end_time - start_time) << " s | Iteraciones: " 
              << iterations << " | Archivo: " << filename << std::endl;

    return 0;
}