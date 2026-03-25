# ===========================================
# Makefile final para Poisson 2D con OpenMP
# ===========================================

# Compilador y banderas
CXX = g++
CXXFLAGS = -Wall -O3 -fopenmp

# Directorios
SRC_DIR = src
BIN_DIR = bin
DATA_DIR = data
IMAG_DIR = imag

# --- PARÁMETROS DE SIMULACIÓN (Valores por defecto) ---
M ?= 50
N ?= 50
TOL ?= 1e-6

# --- PARÁMETROS DE EXPERIMENTACIÓN (Nuevos) ---
THREADS_TEST = 1 2 4 8 16 32 64 80
GRID_SIZES = 50 

# Nombres base de los ejecutables
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

# ------------------------------------------------
# Reglas principales
all: directories $(TARGETS:%=$(BIN_DIR)/%)

directories:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(DATA_DIR)
	@mkdir -p $(IMAG_DIR)

$(BIN_DIR)/%: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(BIN_DIR)/post_stats: tools/post_stats.c
	$(CXX) -O3 $< -o $@

# ------------------------------------------------
# Regla de ejecución simple
run: directories all
	@echo "--- Iniciando ejecución simple ---"
	@echo "" > $(DATA_DIR)/tiempos.dat
	@for exe in $(TARGETS); do \
		echo "==== Ejecutando $$exe ====" | tee -a $(DATA_DIR)/tiempos.dat; \
		$(BIN_DIR)/$$exe $(M) $(N) $(TOL) 16 2>&1 | tee -a $(DATA_DIR)/tiempos.dat; \
	done

# ------------------------------------------------
# Regla: Barrido paramétrico para Análisis de Escalabilidad
run_experiment: directories all
	@echo "--- Iniciando barrido paramétrico de HPC ---"
	@echo "Este proceso tomará tiempo. Por favor, espera..."
	@echo "# Malla	Hilos	Tiempo(s)	Iteraciones	Programa" > $(DATA_DIR)/tiempos_experiment.dat
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
	@echo "--- Experimento Finalizado. Datos limpios en tiempos_experiment.dat ---"

# ------------------------------------------------
# Reglas de Visualización
# ------------------------------------------------

plot_fisica: directories
	@echo "[FISICA] Generando sábanas 3D y mapas de error..."
	@for file in $(DATA_DIR)/solucion_*.dat; do \
		if [ -f "$$file" ]; then \
			base=$$(basename $$file .dat); \
			echo " -> Procesando $$base..."; \
			gnuplot -e "FILE_IN='$$file'; FILE_OUT='$(IMAG_DIR)/$$base.png'" plot_generico.gp; \
		fi \
	done
	@echo "[SUCCESS] Imágenes físicas en /$(IMAG_DIR)"

plot_hpc: directories
	@echo "[HPC] Generando curvas de escalabilidad y rendimiento..."
	@gnuplot plot_escalabilidad.gp
	@echo "[SUCCESS] Gráficas de rendimiento en /$(IMAG_DIR)"

plot_all: plot_fisica plot_hpc


# ------------------------------------------------
# Regla: Post-procesamiento estadístico + visualización
# ------------------------------------------------

stats: directories $(BIN_DIR)/post_stats
	@echo "[STATS] Ejecutando análisis estadístico multimodelo..."

	@cd $(DATA_DIR) && ../$(BIN_DIR)/post_stats

	@echo "[STATS] Generando visualización con Gnuplot..."

	@gnuplot -e "FILE_IN='$(DATA_DIR)/estadistica_final.stats'; FILE_OUT='$(IMAG_DIR)/validacion_stats.png'" tools/plot_stats.gp

	@echo "[SUCCESS] Estadística y visualización completadas."


# ------------------------------------------------
clean:
	rm -rf $(BIN_DIR)/*
	rm -rf $(DATA_DIR)/*.dat
	rm -f *.dat

.PHONY: all clean directories run run_experiment plot_fisica plot_hpc plot_all
