#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <omp.h>
#include <string>

/**
 * @file poisson_atomic-act6.cpp
 * @brief Resolución de Poisson 2D con actualización in-place (Gauss-Seidel asíncrono).
 * Implementación impecable con reducción de error y contador atómico.
 */

const double x_start = 1.0, x_end = 2.0;
const double y_start = 0.0, y_end = 2.0;

int iterations = 0;

void initialize_grid(int M, int N, std::vector<std::vector<double>>& T,
                     std::vector<std::vector<double>>& source,
                     double& h, double& k) 
{
    h = (x_end - x_start) / M;
    k = (y_end - y_start) / N;

    T.assign(M + 1, std::vector<double>(N + 1, 0.0));
    source.assign(M + 1, std::vector<double>(N + 1, 0.0));

    for (int i = 0; i <= M; ++i){
        double x = x_start + i * h;
        T[i][0] = std::pow(x, 2);
        T[i][N] = std::pow(x - 2.0, 2);
    }

    for (int j = 0; j <= N; ++j){
        double y = y_start + j * k;
        T[0][j] = std::pow(1.0 - y, 2);
        T[M][j] = std::pow(2.0 - y, 2);
    }
}

void solve_poisson(std::vector<std::vector<double>>& T,
                   const std::vector<std::vector<double>>& source,
                   int M, int N, double h, double k, double TOL_param) 
{
    double delta = 1.0;

    while (delta > TOL_param) {
        delta = 0.0; // Reset indispensable antes de cada barrido

        #pragma omp parallel reduction(max:delta)
        {
            #pragma omp for collapse(2)
            for (int i = 1; i < M; ++i) {
                for (int j = 1; j < N; ++j) {
                    double T_old = T[i][j];
                    
                    // Esquema de diferencias finitas para Poisson
                    double T_new = (
                        ((T[i+1][j] + T[i-1][j]) * k * k) +
                        ((T[i][j+1] + T[i][j-1]) * h * h) -
                        (source[i][j] * h * h * k * k)
                    ) / (2.0 * (h*h + k*k));

                    double diff = std::abs(T_new - T_old);
                    T[i][j] = T_new; // Actualización in-place (Gauss-Seidel)

                    // Actualización de la variable de reducción
                    if (diff > delta) delta = diff;
                }
            }

            // Incremento atómico del contador global
            #pragma omp single
            {
                #pragma omp atomic
                iterations++;
            }
        }
    }
}

void export_to_file(const std::vector<std::vector<double>>& T,
                    double h, double k, int M, int N,
                    const std::string& filename) 
{
    std::ofstream file(filename, std::ios::out | std::ios::trunc);
    if (!file.is_open()) return;

    for (int i = 0; i <= M; ++i){
        for (int j = 0; j <= N; ++j){
            file << (x_start + i*h) << "\t" << (y_start + j*k) << "\t" << T[i][j] << "\n";
        }
    }
    file.close();
}

int main(int argc, char* argv[]) {
    int M = 50, N = 50;
    double TOL_val = 1e-6;

    if (argc >= 4) {
        M = std::stoi(argv[1]);
        N = std::stoi(argv[2]);
        TOL_val = std::stod(argv[3]);
    }

    double h, k;
    std::vector<std::vector<double>> T, source;

    initialize_grid(M, N, T, source, h, k);
    
    // Término fuente constante f(x,y) = 4
    for(auto &row : source) std::fill(row.begin(), row.end(), 4.0);

    double start_time = omp_get_wtime();
    solve_poisson(T, source, M, N, h, k, TOL_val);
    double end_time = omp_get_wtime();

    std::string filename = "data/solucion_poisson_atomic.dat";
    export_to_file(T, h, k, M, N, filename);

    std::cout << "Programa: poisson_atomic-act6 | Tiempo: " 
              << (end_time - start_time) << " s | Iteraciones: " 
              << iterations << " | Archivo: " << filename << std::endl;

    return 0;
}