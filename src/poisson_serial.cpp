#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <omp.h>
#include <string>

const double x_start = 1.0, x_end = 2.0;
const double y_start = 0.0, y_end = 2.0;
int iterations = 0;

void initialize_grid(int M, int N, std::vector<std::vector<double>>& T, std::vector<std::vector<double>>& source, double& h, double& k) {
    h = (x_end - x_start) / M;
    k = (y_end - y_start) / N;
    T.assign(M + 1, std::vector<double>(N + 1, 0.0));
    source.assign(M + 1, std::vector<double>(N + 1, 0.0));
    for (int i = 0; i <= M; ++i){
        T[i][0] = std::pow(x_start + i*h, 2);
        T[i][N] = std::pow((x_start + i*h) - 2.0, 2);
    }
    for (int j = 0; j <= N; ++j){
        T[0][j] = std::pow(1.0 - (y_start + j*k), 2);
        T[M][j] = std::pow(2.0 - (y_start + j*k), 2);
    }
}

void poisson_source(int M, int N, std::vector<std::vector<double>>& source) {
    for (int i = 0; i <= M; ++i){
        for (int j = 0; j <= N; ++j){
            source[i][j] = 4.0;
        }
    }       
}

void solve_poisson(std::vector<std::vector<double>>& T, const std::vector<std::vector<double>>& source, int M, int N, double h, double k, double TOL_param) {
    double delta = 1.0;
    while (delta > TOL_param) {
        delta = 0.0;
        for (int i = 1; i < M; ++i) {
            for (int j = 1; j < N; ++j) {
                double T_old = T[i][j];
                T[i][j] = (((T[i+1][j] + T[i-1][j]) * k * k) + ((T[i][j+1] + T[i][j-1]) * h * h) - (source[i][j] * h * h * k * k)) / (2.0 * (h*h + k*k));
                double diff = std::abs(T[i][j] - T_old);
                if (diff > delta) delta = diff;
            }
        }
        iterations++;
    }
}

int main(int argc, char* argv[]) {
    int M = 50, N = 50, num_threads = 1;
    double TOL_val = 1e-6;

    if (argc >= 4) {
        M = std::stoi(argv[1]);
        N = std::stoi(argv[2]);
        TOL_val = std::stod(argv[3]);
    }
    if (argc >= 5) {
        num_threads = std::stoi(argv[4]); // Leído pero no aplicado porque es serial
    }

    double h, k;
    std::vector<std::vector<double>> T, source;
    initialize_grid(M, N, T, source, h, k);
    poisson_source(M, N, source);

    double start_time = omp_get_wtime();
    solve_poisson(T, source, M, N, h, k, TOL_val);
    double end_time = omp_get_wtime();

    std::cout << M << "\t" << num_threads << "\t" << (end_time - start_time) 
              << "\t" << iterations << "\tserial" << std::endl;

    return 0;
}
