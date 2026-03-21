# Compilador y banderas de optimización y paralelización
CXX = g++
CXXFLAGS = -Wall -O3 -fopenmp

# Directorios de la estructura del proyecto
SRC_DIR = src
BIN_DIR = bin
DATA_DIR = data

# Lista de todos los ejecutables que se van a generar en la carpeta bin/
TARGETS = \
    $(BIN_DIR)/poisson_serial \
    $(BIN_DIR)/poisson_parallel_for-act1 \
    $(BIN_DIR)/poisson_collapse-act2 \
    $(BIN_DIR)/poisson_sections-act3 \
    $(BIN_DIR)/poisson_schedule \
    $(BIN_DIR)/poisson_sync \
    $(BIN_DIR)/poisson_critical-act6 \
    $(BIN_DIR)/poisson_atomic-act6 \
    $(BIN_DIR)/poisson_task-act7

# Regla por defecto (lo que se ejecuta al escribir solo 'make')
all: directories $(TARGETS)

# Regla para crear las carpetas bin/ y data/ si no existen
directories:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(DATA_DIR)

# Regla genérica para compilar cada .cpp de src/ a un ejecutable en bin/
$(BIN_DIR)/%: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# Regla para limpiar los ejecutables y los archivos de datos generados
clean:
	rm -rf $(BIN_DIR)/*
	rm -rf $(DATA_DIR)/*.dat
	rm -f *.dat

# Declaración de reglas que no son archivos físicos
.PHONY: all clean directories
