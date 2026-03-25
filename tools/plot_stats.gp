set terminal pngcairo size 1200,1200 enhanced font 'Verdana,12'
set output FILE_OUT

set multiplot layout 2,1 title "Superficie Promedio y Banda de Incertidumbre {/Symbol s}"

# =========================
# CONFIGURACIÓN GLOBAL
# =========================
set ticslevel 0
set grid
set hidden3d
set pm3d at s
set palette rgbformulae 33,13,10

# =========================
# PANEL 1: Vista general
# =========================
set title "Vista general (60°,35°)"
set view 60,35

splot \
    FILE_IN using 1:2:3 with pm3d title "{/Symbol m}(x,y)", \
    FILE_IN using 1:2:4 with lines lc rgb "#000000" lw 1 dt 2 title "+{/Symbol s}", \
    FILE_IN using 1:2:5 with lines lc rgb "#000000" lw 1 dt 2 title "-{/Symbol s}"

# =========================
# PANEL 2: Vista lateral
# =========================
set title "Vista lateral (25°,60°)"
set view 25,60

splot \
    FILE_IN using 1:2:3 with pm3d title "{/Symbol m}(x,y)", \
    FILE_IN using 1:2:4 with lines lc rgb "#000000" lw 1 dt 2 title "+{/Symbol s}", \
    FILE_IN using 1:2:5 with lines lc rgb "#000000" lw 1 dt 2 title "-{/Symbol s}"

unset multiplot