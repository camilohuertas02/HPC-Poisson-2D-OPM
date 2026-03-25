#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <omp.h>
#include <string>
#include <iomanip>     // Obligatorio para el control de precisión decimal
#include <filesystem>  // Obligatorio para gestión robusta de directorios

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
                // Esquema de diferencias finitas centrado de segundo orden (5 puntos)
                T[i][j] = (((T[i+1][j] + T[i-1][j]) * k * k) + ((T[i][j+1] + T[i][j-1]) * h * h) - (source[i][j] * h * h * k * k)) / (2.0 * (h*h + k*k));
                
                double diff = std::abs(T[i][j] - T_old);
                if (diff > delta) delta = diff;
            }
        }
        iterations++;
    }
}

void export_to_file(const std::vector<std::vector<double>>& T, double h, double k, int M, int N, const std::string& filename) {
    // 1. Verificación e inicialización de la jerarquía del sistema de archivos
    fs::path p(filename);
    if (p.has_parent_path()) {
        fs::create_directories(p.parent_path());
    }

    // 2. Apertura del descriptor de archivo
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error crítico: No se pudo crear ni abrir el archivo de volcado " << filename << std::endl;
        return;
    }

    // 3. Formato de métricas científicas
    file << "# X\tY\tT(x,y)\n";
    file << std::scientific << std::setprecision(8); // Se evita truncamiento por debajo de 1e-6

    // 4. Volcado de la malla (Escaneo Lineal)
    for (int i = 0; i <= M; ++i) {
        double x = x_start + i * h;
        for (int j = 0; j <= N; ++j) {
            double y = y_start + j * k;
            file << x << "\t" << y << "\t" << T[i][j] << "\n";
        }
        // Crucial: Delimitador de bloque topológico (scanline break) para visualización en superficies
        file << "\n"; 
    }
    file.close();
}

int main(int argc, char* argv[]) {
    // Parámetros numéricos base
    int M = 50, N = 50, num_threads = 1;
    double TOL_val = 1e-6;

    // Control dinámico de ejecución
    if (argc >= 4) {
        M = std::stoi(argv[1]);
        N = std::stoi(argv[2]);
        TOL_val = std::stod(argv[3]);
    }
    if (argc >= 5) {
        num_threads = std::stoi(argv[4]); // Leído para consistencia de scripts, pero inerte en serial.
    }

    double h, k;
    std::vector<std::vector<double>> T, source;
    
    // Configuración del dominio y fuentes
    initialize_grid(M, N, T, source, h, k);
    poisson_source(M, N, source);

    // Medición rigurosa usando reloj de pared (wall-clock time)
    double start_time = omp_get_wtime();
    solve_poisson(T, source, M, N, h, k, TOL_val);
    double end_time = omp_get_wtime();

    // Exportación formal de resultados
    std::string filename = "data/solucion_poisson_serial.dat";
    export_to_file(T, h, k, M, N, filename);
    
    // Registro de telemetría final al canal estándar
    std::cout << M << "\t" << num_threads << "\t" << (end_time - start_time) 
              << "\t" << iterations << "\tserial" << std::endl;

    return 0;
}