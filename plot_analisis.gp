# =============================================================
# CONFIGURACIÓN DINÁMICA DE ENTRADA/SALIDA
# =============================================================
# FILE_IN y FILE_OUT se definen desde la línea de comandos
set terminal pngcairo size 1000,1400 enhanced font 'Verdana,12'
set output FILE_OUT

# Definición de la solución analítica (Física Teórica)
f_analitica(x,y) = (x-1.0)**2 + (y-1.0)**2

set palette rgbformulae 33,13,10
set multiplot layout 2,1 title "Análisis de Rigor: ".FILE_IN font ",16"

# PANEL 1: SABANA 3D
set tmargin at screen 0.90
set bmargin at screen 0.55
set lmargin at screen 0.15
set rmargin at screen 0.85
set dgrid3d 50,50
set hidden3d
set view 60, 30, 1.0, 1.2
splot f_analitica(x,y) with lines lc rgb "#505050" title "Física Analítica", \
      FILE_IN using 1:2:3 with points pt 7 ps 0.4 lc rgb "red" title "Cómputo HPC"

# PANEL 2: MAPA DE CALOR (ERROR)
set tmargin at screen 0.45
set bmargin at screen 0.10
set lmargin at screen 0.15
set rmargin at screen 0.85
set view map
unset dgrid3d
unset hidden3d
set xrange [1:2]
set yrange [0:2]
plot FILE_IN using 1:2:(abs($3 - f_analitica($1,$2))) with image title "Error Absoluto"

unset multiplot