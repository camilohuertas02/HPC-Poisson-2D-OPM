# ⚡ Paralelización con OpenMP: Ecuación de Poisson en 2D

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=for-the-badge&logo=c%2B%2B)
![OpenMP](https://img.shields.io/badge/OpenMP-Enabled-red.svg?style=for-the-badge)
![Gnuplot](https://img.shields.io/badge/Gnuplot-Data_Viz-orange.svg?style=for-the-badge)
![Make](https://img.shields.io/badge/Make-Automation-green.svg?style=for-the-badge)

Este repositorio contiene la implementación en C++ y el análisis riguroso de rendimiento de la solución numérica de la **Ecuación de Poisson 2D** mediante el método iterativo de diferencias finitas. 

El código base secuencial ha sido paralelizado utilizando múltiples directivas de **OpenMP** (`parallel for`, `collapse`, `sections`, `schedule`, `atomic`, `critical`, `task`) para evaluar empíricamente su impacto en el tiempo de ejecución, la contención de memoria y los límites de escalabilidad fuerte regidos por la Ley de Amdahl.

---

## 🖥️ Contexto de Hardware (HPC)
El rendimiento en la computación paralela es relativo a la arquitectura subyacente. Las pruebas de escalabilidad y los perfiles de rendimiento (*Benchmarking*) de este proyecto están diseñados y evaluados para la siguiente topología:
* **Procesador:** AMD Ryzen Threadripper 3990X.
* **Núcleos Físicos / Lógicos:** 64 / 128 Threads.
* **Dominio Paramétrico:** Mallas espaciales desde $64 \times 64$ hasta $1024 \times 1024$.
* **Escalabilidad:** Pruebas de concurrencia con 4, 8, 16, 32 y 64 hilos.

---

## ⚙️ Requisitos del Sistema
Para compilar, ejecutar y renderizar los resultados numéricos de este proyecto, el entorno debe contar con:
1. **Compilador C++:** `g++` (Con soporte para el estándar C++17 y la bandera `-fopenmp`).
2. **Orquestación:** `make` (GNU Make para el pipeline de integración).
3. **Visualización Científica:** `gnuplot` (Para renderizado de superficies 3D y curvas de rendimiento).

---

## 📂 Estructura del Proyecto

```text
Taller_OpenMP_Poisson/
├── Makefile                # Pipeline automatizado de Integración Continua
├── README.md               # Documentación principal
├── actividades/            # Reporte formal en formato LaTeX (.tex y .pdf)
├── bin/                    # [Autogenerado] Ejecutables binarios
├── data/                   # [Autogenerado] Matrices de resultados y logs (.dat)
├── imag/                   # [Autogenerado] Gráficas de rendimiento y sábanas 3D (.png)
├── tools/                  # Herramientas de post-procesamiento estadístico
└── src/                    # Códigos fuente C++
    ├── poisson_serial.cpp
    ├── poisson_parallel_for-act1.cpp
    ├── poisson_collapse-act2.cpp
    ├── poisson_sections-act3.cpp
    ├── poisson_schedule.cpp
    ├── poisson_sync.cpp
    ├── poisson_critical-act6.cpp
    ├── poisson_atomic-act6.cpp
    └── poisson_task-act7.cpp
