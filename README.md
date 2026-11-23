# Buscaminas Duelo

Juego de lógica para dos jugadores locales, hecho como Proyecto Integrador de primer nivel de **Ingeniería en Inteligencia Artificial** (UIDE, periodo septiembre 2026 – enero 2027).

Cada jugador tiene su propio tablero de Buscaminas de 8 × 8 con 10 minas y ambos juegan a la vez. Gana quien revele primero sus 54 casillas seguras. Pisar una mina no elimina al jugador: su tablero se reemplaza por uno nuevo y se suma un reinicio.

**Autores:** André Maldonado, Belén Doicela y Donato Rodríguez.

## Componentes

| Módulo | Lenguaje | Qué hace |
|---|---|---|
| Juego | Python 3 + Tkinter | Lógica del tablero (tres matrices), pila para deshacer, apertura en cascada sin recursión, temporizador, interfaz gráfica y registro de resultados |
| Ranking | C++17 | Lee el historial, construye una lista enlazada de jugadores, la ordena con Bubble Sort y genera el ranking |

Los dos módulos solo se comunican por archivos CSV:

```
Python  ──escribe──▶  datos/historial.csv  ──lee──▶  C++  ──escribe──▶  datos/ranking.csv  ──lee──▶  Python
```

## Reglas del juego

- Cada jugador revela casillas con clic izquierdo y pone o quita banderas con clic derecho.
- Una casilla vacía (0 minas vecinas) abre en cascada las casillas conectadas. La cascada no se puede deshacer.
- Se puede deshacer la última jugada (revelar una casilla con número o poner/quitar una bandera) usando una pila propia de cada jugador.
- Al pisar una mina se genera un tablero nuevo, se suma un reinicio y la pila se vacía. El tablero del rival no cambia.
- La partida dura 10 minutos. Si nadie terminó, se concede una sola vez un tiempo extra de 3 minutos.
- Si al terminar el tiempo extra nadie completó su tablero, gana quien tenga más casillas reveladas; si igualan, quien tenga menos reinicios; si igualan, es empate.

## Estructura del repositorio

```
buscaminas-duelo/
├── python/
│   ├── main.py              # punto de entrada
│   ├── juego/
│   │   ├── tablero.py       # matrices, pila, cascada, jugadas
│   │   ├── partida.py       # dos jugadores y temporizador
│   │   ├── historial.py     # escritura del CSV y llamada al módulo C++
│   │   └── gui.py           # interfaz Tkinter
│   └── tests/               # pruebas unitarias
├── cpp/
│   ├── ranking.cpp          # lista enlazada, Bubble Sort y ranking
│   ├── Makefile
│   └── tests/               # prueba con historial de ejemplo
├── datos/
│   └── ejemplo_historial.csv
├── docs/
│   ├── Fase2_diseno.pdf     # documento de diseño de la Fase 2
│   └── diagramas/           # diagramas funcionales
├── LICENSE
└── README.md
```

## Cómo ejecutarlo

Requisitos: Python 3.8 o superior con Tkinter (en Ubuntu/Debian: `sudo apt install python3-tk`) y un compilador de C++17 (`g++`).

```bash
# 1. Compilar el módulo de ranking
make -C cpp

# 2. Jugar
python python/main.py
```

El botón **Ver ranking** ejecuta el programa compilado, y **Ver historial** muestra las partidas guardadas en `datos/historial.csv`.

El módulo C++ también se puede usar solo:

```bash
./cpp/ranking datos/ejemplo_historial.csv datos/ranking.csv 10
```

## Formato de los archivos

`historial.csv` (una fila por partida):

```
fecha,nombre_j1,reinicios_j1,nombre_j2,reinicios_j2,ganador,tiempo
01/11/2026,Andres,2,Juan,1,Juan,05:32
```

`ranking.csv` (una fila por jugador, ordenado por más victorias y, en empate, por menos reinicios acumulados):

```
posicion,jugador,partidas,victorias,reinicios_acumulados
1,Juan,2,2,4
```

## Pruebas

```bash
# Python (desde la carpeta python/)
cd python && python -m unittest discover -s tests -t . -v

# C++ (desde la raíz del repositorio)
make -C cpp test
```

La prueba de integración de Python se omite si el módulo C++ no está compilado.

## Licencia

Este proyecto se distribuye bajo la **licencia MIT**. El texto completo está en el archivo [LICENSE](LICENSE). Esto permite usar, copiar, modificar y distribuir el código siempre que se conserve el aviso de copyright.

La licencia MIT cubre el código y la documentación escritos por el equipo. **No cubre** el logotipo de la UIDE ni el de Arizona State University que aparecen en la portada de `docs/Fase2_diseno.pdf`, que pertenecen a sus respectivas instituciones y se incluyen solo con fines académicos.

## Créditos y declaración de autoría

### Autoría del proyecto

El diseño del juego (reglas del duelo, tablero, pila, formato del historial, criterios del ranking) y el documento de la Fase 2 son del equipo. El juego original **Buscaminas** (Minesweeper) es un juego clásico de dominio común como idea; este proyecto no copia código de ninguna implementación existente.

### Código de terceros

No se incluye código copiado de otros repositorios, tutoriales ni foros. Los algoritmos usados (Bubble Sort, lista enlazada, pila, recorrido de vecinos con vectores de direcciones) son técnicas estándar vistas en clase y se programaron para este proyecto.

### Librerías y herramientas

El proyecto **no usa librerías de terceros**: solo las bibliotecas estándar de cada lenguaje.

| Recurso | Uso | Licencia |
|---|---|---|
| Python 3 (`random`, `csv`, `subprocess`, `pathlib`, `unittest`, `datetime`) | Lógica, registro y pruebas | Python Software Foundation License |
| Tkinter / ttk | Interfaz gráfica | Incluido con Python (PSF License) |
| Biblioteca estándar de C++ (`fstream`, `sstream`, `string`, `vector`) | Módulo de ranking | Parte del compilador usado (GCC: GPL con excepción de biblioteca en tiempo de ejecución) |
| GNU Make y g++ | Compilación del módulo C++ | GPL (herramientas; no se distribuyen con el proyecto) |

### Uso de inteligencia artificial

Se usó **Claude** (asistente de IA de Anthropic) como apoyo en este proyecto para:

- Revisar el documento de diseño de la Fase 2 y señalar inconsistencias.
- Generar el código base del módulo de ranking en C++ y del módulo del juego en Python.
- Escribir las pruebas unitarias.
- Redactar este README.

El código generado se probó con las pruebas incluidas en el repositorio. El equipo es responsable del contenido final y de entenderlo.

### Recursos visuales

- Los **diagramas funcionales** de `docs/diagramas/` fueron elaborados por el equipo en el documento de diseño de la Fase 2; las imágenes son capturas de sus páginas.
- La interfaz del juego usa solo elementos gráficos nativos de Tkinter y colores definidos en el código. No incluye imágenes, íconos, sonidos ni fuentes de terceros.
- Los logotipos institucionales de la portada del PDF son de la UIDE y de Arizona State University (ver sección de licencia).
