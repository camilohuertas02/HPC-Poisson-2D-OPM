#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <omp.h>
#include <string>

/**
 * @file poisson_task-act7.cpp
 * @brief Resolución de Poisson 2D mediante el modelo de tareas (Tasks) de OpenMP.
 * Divide el dominio en bloques procesados de forma asíncrona para mejorar 
 * el balanceo de carga dinámico.
 */

const double x_start = 1.0, x_end = 2.0;
const double y_start = 0.0, y_end = 2.0;

// Contador de iteraciones global
int iterations = 0;

void initialize_grid(int M, int N,
                     std::vector<std::vector<double>>& T,
                     std::vector<std::vector<double>>& source,
                     double& h, double& k) 
{
    h = (x_end - x_start) / M;
    k = (y_end - y_start) / N;

    T.assign(M + 1, std::vector<double>(N + 1, 0.0));
    source.assign(M + 1, std::vector<double>(N + 1, 0.0));

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

void poisson_source(int M, int N, std::vector<std::vector<double>>& source) 
{
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
    const int block_size = 16; // Tamaño de bloque optimizado para caché L1/L2

    while (delta > TOL_param) {
        delta = 0.0;

        #pragma omp parallel shared(T, source, delta)
        {
            #pragma omp single
            {
                for (int bi = 1; bi < M; bi += block_size) {
                    for (int bj = 1; bj < N; bj += block_size) {
                        #pragma omp task firstprivate(bi, bj) shared(T, source, delta)
                        {
                            double local_delta = 0.0;
                            int i_end = std::min(bi + block_size, M);
                            int j_end = std::min(bj + block_size, N);

                            for (int i = bi; i < i_end; ++i) {
                                for (int j = bj; j < j_end; ++j) {
                                    double T_old = T[i][j];
                                    T[i][j] = (
                                        ((T[i + 1][j] + T[i - 1][j]) * k * k) +
                                        ((T[i][j + 1] + T[i][j - 1]) * h * h) -
                                        (source[i][j] * h * h * k * k)) /
                                        (2.0 * (h * h + k * k));

                                    double diff = std::abs(T[i][j] - T_old);
                                    if (diff > local_delta) local_delta = diff;
                                }
                            }

                            #pragma omp critical
                            {
                                if (local_delta > delta) delta = local_delta;
                            }
                        }
                    }
                }
                #pragma omp taskwait 
                iterations++; // Se incrementa tras sincronizar todas las tareas del paso
            }
        }
    }
}

void export_to_file(const std::vector<std::vector<double>>& T,
                    double h, double k, int M, int N,
                    const std::string& filename) 
{
    std::ofstream file(filename);
    if (!file.is_open()) return;

    for (int i = 0; i <= M; ++i) {
        for (int j = 0; j <= N; ++j) {
            double x = x_start + i*h;
            double y = y_start + j*k;
            file << x << "\t" << y << "\t" << T[i][j] << "\n";
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
    poisson_source(M, N, source);

    double start_time = omp_get_wtime();
    solve_poisson(T, source, M, N, h, k, TOL_val);
    double end_time = omp_get_wtime();

    std::string filename = "data/solucion_poisson_task.dat";
    export_to_file(T, h, k, M, N, filename);

    // Salida estandarizada e impecable
    std::cout << "Programa: poisson_task-act7 | Tiempo_total: " 
              << (end_time - start_time) << " s | Iteraciones: " 
              << iterations << " | Archivo: " << filename << std::endl;

    return 0;
}