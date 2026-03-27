# ==============================================================================
# Makefile MASTER para Proyecto HPC: Ecuación de Poisson 2D con OpenMP
# ==============================================================================

# ------------------------------------------------------------------------------
# 1. CONFIGURACIÓN DEL COMPILADOR Y BANDERAS
# ------------------------------------------------------------------------------
CXX = g++
# -Wall: Muestra todas las advertencias. 
# -O3: Nivel máximo de optimización matemática del compilador.
# -fopenmp: Activa el soporte nativo multihilo de OpenMP.
CXXFLAGS = -Wall -O3 -fopenmp

# ------------------------------------------------------------------------------
# 2. ESTRUCTURA DE DIRECTORIOS
# ------------------------------------------------------------------------------
SRC_DIR = src
BIN_DIR = bin
DATA_DIR = data
IMAG_DIR = imag

# ------------------------------------------------------------------------------
# 3. PARÁMETROS MATEMÁTICOS Y EXPERIMENTALES
# ------------------------------------------------------------------------------
M ?= 50
N ?= 50
TOL ?= 1e-6

# Dominios del barrido de HPC
THREADS_TEST = 4 8  #16 32 64 
GRID_SIZES = 64 #128 256 512 1024

# ------------------------------------------------------------------------------
# 4. DEFINICIÓN DE OBJETIVOS (Binarios a compilar)
# ------------------------------------------------------------------------------
TARGETS = \
    poisson_serial \
    poisson_parallel_for-act1 \
    poisson_collapse-act2 \
    poisson_sections-act3 \
    poisson_schedule \
    poisson_sync \
    poisson_critical-act6 \
    poisson_atomic-act6 \
    poisson_task-act7

# ==============================================================================
# 5. COMANDO MAESTRO (EL PIPELINE HPC COMPLETO)
# ==============================================================================
# Ejecuta secuencialmente: limpieza, creación de carpetas, compilación, 
# simulación numérica pesada, análisis estadístico y graficación final.
pipeline: clean directories all run_experiment stats plot_all
	@echo "========================================================"
	@echo " [SUCCESS] PIPELINE HPC COMPLETADO EXITOSAMENTE"
	@echo " -> Binarios generados en      : /$(BIN_DIR)"
	@echo " -> Tiempos y datos crudos en  : /$(DATA_DIR)"
	@echo " -> Gráficas científicas en    : /$(IMAG_DIR)"
	@echo "========================================================"

# ------------------------------------------------------------------------------
# 6. REGLAS DE COMPILACIÓN BASE
# ------------------------------------------------------------------------------
# Compila todos los ejecutables
all: directories $(TARGETS:%=$(BIN_DIR)/%)

# Garantiza que el árbol de directorios exista antes de compilar o guardar datos
directories:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(DATA_DIR)
	@mkdir -p $(IMAG_DIR)

# Regla genérica para compilar cualquier .cpp dentro de /src hacia /bin
$(BIN_DIR)/%: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# Regla específica para compilar la herramienta estadística en C
$(BIN_DIR)/post_stats: tools/post_stats.c
	$(CXX) -O3 $< -o $@

# ------------------------------------------------------------------------------
# 7. REGLAS DE EJECUCIÓN Y BARRIDO MASIVO
# ------------------------------------------------------------------------------
# Ejecuta una prueba rápida de validación de todos los códigos en la malla por defecto
run: directories all
	@echo "--- Iniciando ejecución simple de validación ---"
	@echo "" > $(DATA_DIR)/tiempos.dat
	@for exe in $(TARGETS); do \
		echo "==== Ejecutando $$exe ====" | tee -a $(DATA_DIR)/tiempos.dat; \
		$(BIN_DIR)/$$exe $(M) $(N) $(TOL) 16 2>&1 | tee -a $(DATA_DIR)/tiempos.dat; \
	done

# Realiza el barrido anidado (Mallas vs Hilos) para evaluar la escalabilidad fuerte
run_experiment: directories all
	@echo "--- Iniciando barrido paramétrico de HPC ---"
	@echo "Este proceso someterá la CPU a estrés. Por favor, espera..."
	@echo "# Malla  Hilos   Tiempo(s)   Iteraciones Programa" > $(DATA_DIR)/tiempos_experiment.dat
	@for m in $(GRID_SIZES); do \
		$(BIN_DIR)/poisson_serial $$m $$m $(TOL) 1 >> $(DATA_DIR)/tiempos_experiment.dat; \
		for t in $(THREADS_TEST); do \
			$(BIN_DIR)/poisson_parallel_for-act1 $$m $$m $(TOL) $$t >> $(DATA_DIR)/tiempos_experiment.dat; \
			$(BIN_DIR)/poisson_collapse-act2 $$m $$m $(TOL) $$t >> $(DATA_DIR)/tiempos_experiment.dat; \
			$(BIN_DIR)/poisson_sections-act3 $$m $$m $(TOL) $$t >> $(DATA_DIR)/tiempos_experiment.dat; \
			$(BIN_DIR)/poisson_schedule $$m $$m $(TOL) $$t >> $(DATA_DIR)/tiempos_experiment.dat; \
			$(BIN_DIR)/poisson_sync $$m $$m $(TOL) $$t >> $(DATA_DIR)/tiempos_experiment.dat; \
			$(BIN_DIR)/poisson_critical-act6 $$m $$m $(TOL) $$t >> $(DATA_DIR)/tiempos_experiment.dat; \
			$(BIN_DIR)/poisson_atomic-act6 $$m $$m $(TOL) $$t >> $(DATA_DIR)/tiempos_experiment.dat; \
			$(BIN_DIR)/poisson_task-act7 $$m $$m $(TOL) $$t >> $(DATA_DIR)/tiempos_experiment.dat; \
		done \
	done
	@echo "--- Experimento Finalizado. Datos consolidados en tiempos_experiment.dat ---"

# ------------------------------------------------------------------------------
# 8. REGLAS DE ANÁLISIS Y VISUALIZACIÓN
# ------------------------------------------------------------------------------
# Renderiza la topología de la ecuación de Poisson usando Gnuplot
plot_fisica: directories
	@echo "[FISICA] Generando sábanas 3D y mapas de error..."
	@for file in $(DATA_DIR)/solucion_*.dat; do \
		if [ -f "$$file" ]; then \
			base=$$(basename $$file .dat); \
			echo " -> Procesando $$base..."; \
			gnuplot -e "FILE_IN='$$file'; FILE_OUT='$(IMAG_DIR)/$$base.png'" plot_generico.gp; \
		fi \
	done
	@echo "[SUCCESS] Imágenes físicas almacenadas en /$(IMAG_DIR)"

# Genera las gráficas de rendimiento (Tiempo vs Hilos)
plot_hpc: directories
	@echo "[HPC] Generando curvas de escalabilidad y rendimiento..."
	@gnuplot plot_escalabilidad.gp
	@echo "[SUCCESS] Gráficas de rendimiento almacenadas en /$(IMAG_DIR)"

# Agrupa ambas reglas de graficación
plot_all: plot_fisica plot_hpc

# ------------------------------------------------------------------------------
# 9. POST-PROCESAMIENTO ESTADÍSTICO
# ------------------------------------------------------------------------------
stats: directories $(BIN_DIR)/post_stats
	@echo "[STATS] Ejecutando análisis estadístico multimodelo..."
	@cd $(DATA_DIR) && ../$(BIN_DIR)/post_stats
	@echo "[STATS] Generando visualización con Gnuplot..."
	@gnuplot -e "FILE_IN='$(DATA_DIR)/estadistica_final.stats'; FILE_OUT='$(IMAG_DIR)/validacion_stats.png'" tools/plot_stats.gp
	@echo "[SUCCESS] Estadística y visualización completadas."

# ------------------------------------------------------------------------------
# 10. REGLA DE LIMPIEZA (PURGA DE ENTORNO)
# ------------------------------------------------------------------------------
clean:
	@echo "[CLEAN] Purgando binarios compilados..."
	@rm -rf $(BIN_DIR)/*
	@echo "[CLEAN] Purgando datos crudos en data/..."
	@rm -rf $(DATA_DIR)/*.dat
	@rm -f *.dat
	@echo "[CLEAN] Eliminando gráficas antiguas en imag/ (Protegiendo subcarpetas 512 y 1024)..."
	@find $(IMAG_DIR) -maxdepth 1 -type f -name "*.png" -delete
	@echo "[CLEAN] Entorno limpio y listo para una nueva ejecución."

.PHONY: all clean directories run run_experiment plot_fisica plot_hpc plot_all pipeline stats