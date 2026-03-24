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

# --- PARÁMETROS DE SIMULACIÓN (Valores por defecto) ---
M ?= 50
N ?= 50
TOL ?= 1e-6

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
# Regla por defecto
all: directories $(TARGETS:%=$(BIN_DIR)/%)

directories:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(DATA_DIR)

$(BIN_DIR)/%: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# ------------------------------------------------
# Regla de ejecución parametrizada
# Uso: make run M=100 N=100 TOL=1e-8 TARGET=poisson_sync
run: directories
	@echo "--- Iniciando ejecución con M=$(M), N=$(N), TOL=$(TOL) ---"
	@echo "" > $(DATA_DIR)/tiempos.dat
	@if [ -n "$(TARGET)" ]; then \
		echo "==== Ejecutando $(TARGET) ====" | tee -a $(DATA_DIR)/tiempos.dat; \
		$(BIN_DIR)/$(TARGET) $(M) $(N) $(TOL) 2>&1 | tee -a $(DATA_DIR)/tiempos.dat; \
	else \
		for exe in $(TARGETS); do \
			echo "==== Ejecutando $$exe ====" | tee -a $(DATA_DIR)/tiempos.dat; \
			$(BIN_DIR)/$$exe $(M) $(N) $(TOL) 2>&1 | tee -a $(DATA_DIR)/tiempos.dat; \
		done \
	fi

clean:
	rm -rf $(BIN_DIR)/*
	rm -rf $(DATA_DIR)/*.dat
	rm -f *.dat

.PHONY: all clean directories run