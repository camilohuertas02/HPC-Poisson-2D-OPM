# =============================================================
# CONFIGURACIÓN DE RIGOR: plot_generico.gp
# =============================================================
set terminal pngcairo size 1000,1400 enhanced font 'Verdana,12'
set output FILE_OUT

# 1. FUNCIÓN ANALÍTICA DEDUCIDA DE TUS FRONTERAS (IMPRESCINDIBLE)
f_analitica(x,y) = (x - y - 1.0)**2

set palette rgbformulae 33,13,10
set multiplot layout 2,1 title "Validación de Rigor: ".FILE_IN noenhanced font ",16"

# PANEL 1: SABANA 3D (Comparación Directa)
# -------------------------------------------------------------
set tmargin at screen 0.90
set bmargin at screen 0.55
set lmargin at screen 0.15
set rmargin at screen 0.85

set title "Superficie de Potencial: Teoría (Malla) vs Cómputo (Rojo)"
set xlabel "Eje X"
set ylabel "Eje Y"
set zlabel "T(x,y)"
set dgrid3d 50,50 
set hidden3d
set view 60, 30, 1.0, 1.2

# Superposición: La malla es la verdad física, los puntos son tu código
splot f_analitica(x,y) with lines lc rgb "#505050" title "Física Teórica", \
      FILE_IN using 1:2:3 with points pt 7 ps 0.4 lc rgb "red" title "Cómputo HPC"

# PANEL 2: MAPA DE CALOR DEL ERROR (Verificación de Convergencia)
# -------------------------------------------------------------
set tmargin at screen 0.45
set bmargin at screen 0.10
set lmargin at screen 0.15
set rmargin at screen 0.85

set title "Error Absoluto Local: |T_{num} - T_{teorico}|"
set view map
unset dgrid3d
unset hidden3d

set xlabel "Eje X"
set ylabel "Eje Y"
set cblabel "Error"
set xrange [1:2]
set yrange [0:2]

# Si el código es correcto, este mapa debería ser casi uniforme y muy bajo (orden de 1e-6)
plot FILE_IN using 1:2:(abs($3 - f_analitica($1,$2))) with image title "Residuo"

unset multiplot