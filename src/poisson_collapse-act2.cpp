#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <omp.h>
#include <string>
#include <iomanip>     // Control de precisión científica
#include <filesystem>  // Gestión de directorios (C++17)

namespace fs = std::filesystem;

const double x_start = 1.0, x_end = 2.0;
const double y_start = 0.0, y_end = 2.0;
int iterations = 0;

void initialize_grid(int M, int N, std::vector<std::vector<double>>& T, std::vector<std::vector<double>>& source, double& h, double& k) {
    h = (x_end - x_start) / M;
    k = (y_end - y_start) / N;
    
    T.assign(M + 1, std::vector<double>(N + 1, 0.0));
    source.assign(M + 1, std::vector<double>(N + 1, 0.0));
    
    // Condiciones de frontera analíticas
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

void poisson_source(int M, int N, std::vector<std::vector<double>>& source) {
    for (int i = 0; i <= M; ++i){
        for (int j = 0; j <= N; ++j){
            source[i][j] = 4.0; // f(x,y) constante
        }
    }       
}

void solve_poisson(std::vector<std::vector<double>>& T, const std::vector<std::vector<double>>& source, int M, int N, double h, double k, double TOL_param) {
    double delta = 1.0;
    const double h2 = h * h;
    const double k2 = k * k;
    const double denom = 2.0 * (h2 + k2);

    while (delta > TOL_param) {
        delta = 0.0;
        
        // El collapse(2) expande el espacio de iteración a (M-1)*(N-1)
        #pragma omp parallel for collapse(2) reduction(max:delta)
        for (int i = 1; i < M; ++i) {
            for (int j = 1; j < N; ++j) {
                double T_old = T[i][j];
                
                // Esquema de 5 puntos
                T[i][j] = (((T[i+1][j] + T[i-1][j]) * k2) + 
                           ((T[i][j+1] + T[i][j-1]) * h2) - 
                           (source[i][j] * h2 * k2)) / denom;

                double diff = std::abs(T[i][j] - T_old);
                if (diff > delta) delta = diff;
            }
        }
        iterations++;
    }
}

void export_to_file(const std::vector<std::vector<double>>& T, double h, double k, int M, int N, const std::string& filename) {
    // 1. Asegurar la existencia del directorio
    fs::path p(filename);
    if (p.has_parent_path()) {
        fs::create_directories(p.parent_path());
    }

    // 2. Apertura del archivo con formato científico de alta precisión
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de salida " << filename << std::endl;
        return;
    }

    file << "# X\tY\tT(x,y)\n";
    file << std::scientific << std::setprecision(8);

    for (int i = 0; i <= M; ++i) {
        double x = x_start + i * h;
        for (int j = 0; j <= N; ++j) {
            double y = y_start + j * k;
            file << x << "\t" << y << "\t" << T[i][j] << "\n";
        }
        // Salto de línea para definir bloques de malla (formato splot de Gnuplot)
        file << "\n";
    }
    file.close();
}

int main(int argc, char* argv[]) {
    int M = 50, N = 50, num_threads = omp_get_max_threads();
    double TOL_val = 1e-6;

    if (argc >= 4) {
        M = std::stoi(argv[1]);
        N = std::stoi(argv[2]);
        TOL_val = std::stod(argv[3]);
    }
    if (argc >= 5) {
        num_threads = std::stoi(argv[4]);
        omp_set_num_threads(num_threads);
    }

    double h, k;
    std::vector<std::vector<double>> T, source;

    initialize_grid(M, N, T, source, h, k);
    poisson_source(M, N, source);

    double start_time = omp_get_wtime();
    solve_poisson(T, source, M, N, h, k, TOL_val);
    double end_time = omp_get_wtime();
    
    std::string filename = "data/solucion_poisson_collapse.dat";
    export_to_file(T, h, k, M, N, filename);

    std::cout << M << "\t" << num_threads << "\t" << (end_time - start_time) 
              << "\t" << iterations << "\tcollapse" << std::endl;

    return 0;
}