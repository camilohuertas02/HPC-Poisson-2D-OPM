#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_FILES 6
#define MAX_LINE 512

int main() {

    const char *files[NUM_FILES] = {
        "solucion_poisson_atomic.dat",
        //"solucion_poisson_collapse.dat",
        "solucion_poisson_critical.dat",
        //"solucion_poisson_parallel_for.dat",
        "solucion_poisson_schedule.dat",
        "solucion_poisson_sections.dat",
        //"solucion_poisson_serial.dat",
        "solucion_poisson_sync.dat",
        "solucion_poisson_task.dat"
    };

    FILE *fps[NUM_FILES];

    // Abrir archivos
    for (int i = 0; i < NUM_FILES; i++) {
        fps[i] = fopen(files[i], "r");
        if (!fps[i]) {
            printf("Error abriendo: %s\n", files[i]);
            return 1;
        }
    }

    // Archivo de salida
    FILE *f_out = fopen("estadistica_final.stats", "w");
    if (!f_out) {
        perror("Error al crear archivo de salida");
        return 1;
    }

    // Encabezado
    fprintf(f_out, "# Validación Estadística Multimodelo\n");
    fprintf(f_out, "# N_muestras: %d\n", NUM_FILES);
    fprintf(f_out, "# X\tY\tMEAN\tUPPER_1S\tLOWER_1S\n");

    double x, y, t_val;
    double prev_x = -1.0;  // 🔥 para detectar cambio de fila
    char line[MAX_LINE];

    // Loop principal
    while (fgets(line, sizeof(line), fps[0])) {

        // Saltar comentarios / separadores
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {

            for (int i = 1; i < NUM_FILES; i++) {
                if (!fgets(line, sizeof(line), fps[i])) break;
            }
            continue;
        }

        double sum = 0.0, sum_sq = 0.0;

        // Archivo maestro
        if (sscanf(line, "%lf %lf %lf", &x, &y, &t_val) == 3) {

            // 🔥 INSERTAR SALTO DE LÍNEA SI CAMBIA X
            if (fabs(x - prev_x) > 1e-12) {
                if (prev_x != -1.0) {
                    fprintf(f_out, "\n");
                }
                prev_x = x;
            }

            sum += t_val;
            sum_sq += t_val * t_val;
        }

        // Resto de archivos
        for (int i = 1; i < NUM_FILES; i++) {
            if (fgets(line, sizeof(line), fps[i])) {
                double tx, ty, tv;
                if (sscanf(line, "%lf %lf %lf", &tx, &ty, &tv) == 3) {
                    sum += tv;
                    sum_sq += tv * tv;
                }
            }
        }

        // Estadística
        double mean = sum / NUM_FILES;
        double variance = (sum_sq / NUM_FILES) - (mean * mean);

        if (variance < 1e-18) variance = 0.0;

        double std_dev = sqrt(fabs(variance));

        fprintf(f_out, "%.6f\t%.6f\t%.10e\t%.10e\t%.10e\n",
                x, y, mean, mean + 1000*std_dev, mean - 1000*std_dev);
    }

    // Cerrar archivos
    for (int i = 0; i < NUM_FILES; i++) fclose(fps[i]);
    fclose(f_out);

    printf(">> Validación completada con %d métodos.\n", NUM_FILES);

    return 0;
}