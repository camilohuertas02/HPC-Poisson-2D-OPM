# =============================================================
# SCRIPT HPC: COMPARATIVA DE DIRECTIVAS POR TAMAÑO DE MALLA
# =============================================================
# Este script genera 3 gráficas (una por cada malla).
# Eje X: Hilos (Log2) | Eje Y: Tiempo (Log10) | Color: Directiva

set terminal pngcairo size 800,600 enhanced font 'Verdana,12'

# Estilo general
set grid
set xlabel "Número de Hilos" font ",12"
set ylabel "Tiempo de Ejecución (segundos)" font ",12"
set logscale x 2
set xtics (1, 2, 4, 8, 16, 32, 64, 80)
set logscale y 10
set format y "10^{%L}" # Formato científico para el eje Y

# Posición de la leyenda
set key top right box opaque

# Lista de mallas a iterar
mallas = "50 100 200"

# Bucle principal: Genera una gráfica por cada tamaño de malla
do for [m in mallas] {
    
    # Nombre del archivo de salida para esta malla
    set output sprintf("imag/comparativa_malla_%s.png", m)
    
    set title sprintf("Rendimiento OpenMP - Malla %sx%s", m, m) font ",14"
    
    # Truco de Gnuplot: Filtramos nativamente la columna 1 ($1) que tenga la malla actual (m)
    # y la columna 5 ($5) que tenga el nombre del programa.
    
plot \
    sprintf("< awk '$1==%s && $5==\"serial\"' data/tiempos_experiment.dat", m) \
        u 2:3 w linespoints lw 2 pt 13 lc "black" title "Serial (Baseline)", \
    \
    sprintf("< awk '$1==%s && $5==\"parallel_for\"' data/tiempos_experiment.dat", m) \
        u 2:3 w linespoints lw 2 pt 7 lc rgb "#0072bd" title "Parallel For", \
    \
    sprintf("< awk '$1==%s && $5==\"collapse\"' data/tiempos_experiment.dat", m) \
        u 2:3 w linespoints lw 2 pt 5 lc rgb "#d95319" title "Collapse(2)", \
    \
    sprintf("< awk '$1==%s && $5==\"schedule\"' data/tiempos_experiment.dat", m) \
        u 2:3 w linespoints lw 2 pt 9 lc rgb "#edb120" title "Schedule", \
    \
    sprintf("< awk '$1==%s && $5==\"sections\"' data/tiempos_experiment.dat", m) \
        u 2:3 w linespoints lw 2 pt 8 lc rgb "#d95319" title "Sections", \
    \
    sprintf("< awk '$1==%s && $5==\"sync\"' data/tiempos_experiment.dat", m) \
        u 2:3 w linespoints lw 2 pt 11 lc rgb "#7e2f8e" title "Sync", \
    \
    sprintf("< awk '$1==%s && $5==\"critical\"' data/tiempos_experiment.dat", m) \
        u 2:3 w linespoints lw 2 pt 15 lc rgb "#77ac30" title "Critical", \
    \
    sprintf("< awk '$1==%s && $5==\"atomic\"' data/tiempos_experiment.dat", m) \
        u 2:3 w linespoints lw 2 pt 17 lc rgb "#4dbeee" title "Atomic", \
    \
    sprintf("< awk '$1==%s && $5==\"task\"' data/tiempos_experiment.dat", m) \
        u 2:3 w linespoints lw 2 pt 19 lc rgb "#a2142f" title "Tasks"
}
