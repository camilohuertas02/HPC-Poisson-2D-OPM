# Proyecto de Paralelización con OpenMP: Ecuación de Poisson en 2D

Este repositorio contiene la implementación y el análisis de rendimiento de la solución numérica de la ecuación de Poisson 2D mediante diferencias finitas (método iterativo). El código base secuencial ha sido paralelizado utilizando múltiples directivas de OpenMP para evaluar su impacto en el tiempo de ejecución y la escalabilidad.

## Autores
* Camilo Huertas
* Sebastián Rodríguez
* Universidad Distrital Francisco José de Caldas - Programa académico de Física

## Requisitos del Sistema
Para compilar, ejecutar y visualizar los resultados de este proyecto, necesitas tener instalados:
* Compilador C++ (ej. g++) con soporte para OpenMP.
* Make (para la orquestación de tareas).
* Gnuplot (para la generación automática de gráficas).

## Estructura del Proyecto
* src/: Códigos fuente en C++ (poisson_serial.cpp, poisson_parallel_for.cpp, etc.).
* bin/: Carpeta donde se guardan los binarios compilados automáticamente.
* data/: Archivos .dat con las matrices de resultados y registros de tiempos generados por las simulaciones.
* imag/: Gráficas .png generadas automáticamente por Gnuplot.
* Makefile: Archivo de automatización para compilar, ejecutar y graficar.
* plot_generico.gp: Script de Gnuplot para validación física (sábanas 3D y error absoluto).
* plot_escalabilidad.gp: Script de Gnuplot para el análisis de rendimiento (tiempo vs. número de hilos).

## Instrucciones de Uso

Todo el flujo de trabajo está automatizado a través del Makefile. Abre tu terminal en la raíz del proyecto y utiliza los siguientes comandos:

### 1. Compilación
Ejecuta el siguiente comando para compilar todos los archivos fuente dentro de la carpeta src/ y generar los ejecutables en bin/:

    make all

### 2. Ejecución Simple (Prueba)
Ejecuta una prueba rápida de todos los programas con una malla de 50x50 usando 16 hilos:

    make run

### 3. Ejecución del Experimento de Escalabilidad (HPC)
Realiza un barrido paramétrico riguroso evaluando distintas directivas de OpenMP. Varía el tamaño de la malla (50, 100, 200) y el número de hilos lógicos (1 a 80). Guarda los resultados limpios en data/tiempos_experiment.dat:

    make run_experiment

### 4. Generación de Gráficas
Una vez finalizadas las ejecuciones, puedes generar las gráficas con:

    make plot_fisica
    make plot_hpc
    make plot_all

* make plot_fisica: Genera las sábanas 3D teóricas vs. numéricas.
* make plot_hpc: Genera las curvas de escalabilidad y rendimiento.
* make plot_all: Ejecuta ambas visualizaciones al tiempo. Todas las imágenes resultantes se guardarán en la carpeta imag/.

### 5. Limpieza del Entorno
Elimina los binarios compilados y los datos generados para dejar el directorio limpio:

    make clean
