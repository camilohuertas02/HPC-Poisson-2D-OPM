# =============================================================
# SCRIPT HPC: COMPARATIVA DE DIRECTIVAS POR TAMAÑO DE MALLA
# =============================================================

set terminal pngcairo size 800,600 enhanced font 'Verdana,12'

# Estilo general
set grid
set xlabel "Número de Hilos (Escala Log2)" font ",12"
set ylabel "Tiempo de Ejecución (segundos, Log10)" font ",12"
set logscale x 2

# Ajustado estrictamente a tus valores reales + el caso serial
set xtics (1, 4, 8, 16, 32, 64)

set logscale y 10
set format y "10^{%L}"

# Posición de la leyenda ajustada a superior izquierda
set key top left box opaque

# Mallas reales del experimento
mallas = "64 128 256 512 1024"

# Bucle principal: Genera una gráfica por cada tamaño de malla
do for [m in mallas] {
    
    set output sprintf("imag/comparativa_malla_%s.png", m)
    set title sprintf("Rendimiento OpenMP - Malla %sx%s", m, m) font ",14"
    
    plot \
        sprintf("< awk '$1==%s && $5==\"serial\"' data/tiempos_experiment.dat", m) \
            u 2:3 w points pt 13 ps 2 lc "black" title "Serial (Baseline)", \
        \
        sprintf("< awk '$1==%s && $5==\"parallel_for\"' data/tiempos_experiment.dat", m) \
            u 2:3 w linespoints lw 2 pt 7 lc rgb "#0072bd" title "Parallel For", \
        \
        sprintf("< awk '$1==%s && $5==\"collapse\"' data/tiempos_experiment.dat", m) \
            u 2:3 w linespoints lw 2 pt 5 lc rgb "#d95319" title "Collapse", \
        \
        sprintf("< awk '$1==%s && $5==\"schedule\"' data/tiempos_experiment.dat", m) \
            u 2:3 w linespoints lw 2 pt 9 lc rgb "#edb120" title "Schedule", \
        \
        sprintf("< awk '$1==%s && $5==\"sections\"' data/tiempos_experiment.dat", m) \
            u 2:3 w linespoints lw 2 pt 8 lc rgb "#7e2f8e" title "Sections", \
        \
        sprintf("< awk '$1==%s && $5==\"sync\"' data/tiempos_experiment.dat", m) \
            u 2:3 w linespoints lw 2 pt 11 lc rgb "#77ac30" title "Sync", \
        \
        sprintf("< awk '$1==%s && $5==\"critical\"' data/tiempos_experiment.dat", m) \
            u 2:3 w linespoints lw 2 pt 15 lc rgb "#4dbeee" title "Critical", \
        \
        sprintf("< awk '$1==%s && $5==\"atomic\"' data/tiempos_experiment.dat", m) \
            u 2:3 w linespoints lw 2 pt 17 lc rgb "#a2142f" title "Atomic", \
        \
        sprintf("< awk '$1==%s && $5==\"task\"' data/tiempos_experiment.dat", m) \
            u 2:3 w linespoints lw 2 pt 19 lc rgb "#000000" title "Tasks"
}