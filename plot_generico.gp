# =============================================================
# CONFIGURACIÓN DE RIGOR (FINAL)
# =============================================================
set terminal pngcairo size 1200,1400 enhanced font 'Verdana,12'
set output FILE_OUT

f_analitica(x,y) = (x - y)**2

set palette rgbformulae 33,13,10
set multiplot layout 2,1 title "Validación de Rigor: ".FILE_IN font ",16"

# =============================================================
# PANEL 1: SUPERFICIE 3D
# =============================================================

unset key
set key top right

set lmargin 10
set rmargin 10
set tmargin 2
set bmargin 5

set title "Superficie: Teoría vs Cómputo"

set xlabel "Eje X"
set ylabel "Eje Y"
set zlabel "T(x,y)"

set autoscale z
set xrange [1:2]
set yrange [0:2]

unset dgrid3d
set hidden3d
set view 60, 35, 1, 1

set ticslevel 0
set ztics auto

splot f_analitica(x,y) with lines lw 1 lc rgb "#404040" title "Teoría", \
      FILE_IN using 1:2:3 with points pt 7 ps 0.3 lc rgb "red" title "Numérico"


# =============================================================
# PANEL 2: MAPA DE ERROR (ESTILO ORIGINAL RESTAURADO)
# =============================================================

set title "Error Absoluto Local: |T_{num} - T_{teorico}|"

set view map
unset hidden3d

set xlabel "Eje X"
set ylabel "Eje Y"
set cblabel "Error"

set xrange [1:2]
set yrange [0:2]

# 🔥 CLAVE: mejorar calidad de image (evita franjas)
set samples 200
set isosamples 200

# 🔥 mantener tu estilo original
plot FILE_IN using 1:2:(abs($3 - f_analitica($1,$2))) with image notitle

unset multiplot