#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <omp.h>

const double x0 = 1.0, xf = 2.0;
const double y0 = 0.0, yf = 2.0;
const double TOL = 1e-6;

// Inicializa las matrices de la grilla y las condiciones de frontera
void initialize_grid(int M, int N, std::vector<std::vector<double>>& T, std::vector<std::vector<double>>& source, double& h, double& k) {
    h = (xf - x0) / M;
    k = (yf - y0) / N;

    T.resize(M + 1, std::vector<double>(N + 1, 0.0));
    source.resize(M + 1, std::vector<double>(N + 1, 0.0));

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

// Calcula el término fuente como una distribución gaussiana
void poisson_source(int M, int N, std::vector<std::vector<double>>& source, double h, double k) {
    for (int i = 0; i <= M; ++i){
        for (int j = 0; j <= N; ++j){
            source[i][j] = 4.0;
        }
    }       
}

// Resuelve la ecuación de Poisson iterativamente usando tasks
void solve_poisson(std::vector<std::vector<double>>& T, const std::vector<std::vector<double>>& source, int M, int N, double h, double k) {
    double delta = 1.0;
    int block_size = 10; // Tamaño del bloque (puedes ajustarlo para experimentar)

    while (delta > TOL) {
        delta = 0.0;
        
        #pragma omp parallel shared(T, source, delta)
        {
            // Solo un hilo genera las tareas
            #pragma omp single
            {
                // Dividimos la grilla en bloques
                for (int bi = 1; bi < M; bi += block_size) {
                    for (int bj = 1; bj < N; bj += block_size) {
                        
                        // Creamos una tarea por cada bloque
                        #pragma omp task shared(T, source, delta) firstprivate(bi, bj)
                        {
                            double local_delta = 0.0;
                            
                            // Límites del bloque actual, asegurando no salirnos de M y N
                            int i_end = std::min(bi + block_size, M);
                            int j_end = std::min(bj + block_size, N);

                            for (int i = bi; i < i_end; ++i) {
                                for (int j = bj; j < j_end; ++j) {
                                    double T_new = (
                                        ((T[i + 1][j] + T[i - 1][j]) * k * k) +
                                        ((T[i][j + 1] + T[i][j - 1]) * h * h) -
                                        (source[i][j] * h * h * k * k)) /
                                        (2.0 * (h * h + k * k));

                                    local_delta = std::max(local_delta, std::abs(T_new - T[i][j]));
                                    T[i][j] = T_new;
                                }
                            }

                            // Actualizamos el delta global de forma segura
                            #pragma omp critical
                            {
                                delta = std::max(delta, local_delta);
                            }
                        } // Fin de la directiva task
                    }
                }
                
                // Esperamos a que todas las tareas de esta iteración terminen
                #pragma omp taskwait
            } // Fin de single (tiene una barrera implícita)
        } // Fin de parallel
    }
}

// Exporta los resultados de la matriz T a un archivo .dat
void export_to_file(const std::vector<std::vector<double>>& T, double h, double k, int M, int N, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo para escritura." << std::endl;
        return;
    }
    for (int i = 0; i <= M; ++i) {
        for (int j = 0; j <= N; ++j) {
            double x = x0 + i * h;
            double y = y0 + j * k;
            file << x << "\t" << y << "\t" << T[i][j] << "\n";
        }
    }
    file.close();
    std::cout << "Resultados exportados a " << filename << std::endl;
}

int main() {
    int M = 50, N = 50;
    double h, k;
    std::vector<std::vector<double>> T, source;

    initialize_grid(M, N, T, source, h, k);
    poisson_source(M, N, source, h, k);
    
    double start_time = omp_get_wtime();
    
    solve_poisson(T, source, M, N, h, k);
    
    double end_time = omp_get_wtime();
    std::cout << "Tiempo con task (bloques): " << end_time - start_time << " segundos.\n";

    export_to_file(T, h, k, M, N, "solucion_poisson_task.dat");

    std::cout << "Simulación completada." << std::endl;
    return 0;
}
