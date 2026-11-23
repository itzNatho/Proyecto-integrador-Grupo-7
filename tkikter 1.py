
# La librería random nos ayuda a generar valores aleatorios
# Se utiliza para colocar las minas en diferentes posiciones
import random

# Librería para crear la ventana y los botones
import tkinter as tk

# Librería para mostrar mensajes en pantalla
from tkinter import messagebox


# -----------------------------------------
# DATOS PRINCIPALES DEL JUEGO
# -----------------------------------------

# Tamaño del tablero: 8 filas y 8 columnas
tamano = 8

# Cantidad de minas que tendrá el tablero
num_minas = 10

# Contador de reinicios del tablero
reinicios = 0

# Contador de casillas seguras descubiertas
descubiertas = 0

# Lista donde se guardarán los botones
botones = []


# -----------------------------------------
# CREAR LA VENTANA
# -----------------------------------------

ventana = tk.Tk()

ventana.title("Buscaminas")
ventana.geometry("450x550")

# Evita que se pueda cambiar el tamaño de la ventana
ventana.resizable(False, False)


# Título que aparecerá en la ventana
titulo = tk.Label(
    ventana,
    text="BUSCAMINAS",
    font=("Arial", 20, "bold")
)
titulo.pack(pady=10)


# Texto para mostrar la cantidad de reinicios
etiqueta_reinicios = tk.Label(
    ventana,
    text="Reinicios: 0",
    font=("Arial", 12)
)
etiqueta_reinicios.pack()


# Texto para mostrar los mensajes del juego
etiqueta_mensaje = tk.Label(
    ventana,
    text="Selecciona una casilla",
    font=("Arial", 12)
)
etiqueta_mensaje.pack(pady=10)
marco_tablero = tk.Frame(ventana)
marco_tablero.pack(pady=10)

# -----------------------------------------
# FUNCIÓN PARA CREAR LAS MATRICES
# -----------------------------------------

def tablero():

    # Matriz que guarda las posiciones de las minas
    minas = [[0 for j in range(tamano)] for i in range(tamano)]

    # Matriz que guarda los números de las casillas
    numeros = [[0 for j in range(tamano)] for i in range(tamano)]

    # Matriz que indica si una casilla está oculta o descubierta
    visibilidad = [[0 for j in range(tamano)] for i in range(tamano)]

    # Selecciona 10 posiciones diferentes para colocar las minas
    posiciones = random.sample(range(tamano * tamano), num_minas)

    # Colocar las minas en la matriz
    for posicion in posiciones:

        fila = posicion // tamano
        columna = posicion % tamano

        minas[fila][columna] = 1

    # Calcula cuántas minas hay alrededor de cada casilla
    for i in range(tamano):
        for j in range(tamano):

            contador = 0

            # Revisa las casillas vecinas
            for f in range(i - 1, i + 2):
                for c in range(j - 1, j + 2):

                    # Comprueba que la posición esté dentro del tablero
                    if f >= 0 and f < tamano and c >= 0 and c < tamano:

                        # No cuenta la misma casilla
                        if (f != i or c != j) and minas[f][c] == 1:
                            contador = contador + 1

            # Guarda el total de minas alrededor
            numeros[i][j] = contador

    return minas, numeros, visibilidad


# -----------------------------------------
# FUNCIÓN PARA CREAR LOS BOTONES
# -----------------------------------------

def crear_botones():

    # Recorre las filas del tablero
    for i in range(tamano):

        # Recorre las columnas del tablero
        for j in range(tamano):

            # Crea un botón para cada casilla
            boton = tk.Button(
                marco_tablero,
                text="?",
                width=4,
                height=2,
                font=("Arial", 12, "bold"),
                command=lambda f=i, c=j: seleccionar(f, c)
            )

            # Ubica el botón dentro del marco
            boton.grid(row=i, column=j, padx=2, pady=2)

            # Guarda el botón en la lista
            botones.append(boton)


# -----------------------------------------
# FUNCIÓN PARA SELECCIONAR UNA CASILLA
# -----------------------------------------

def seleccionar(fila, columna):

    # Permite modificar el contador de reinicios
    global reinicios

    # Permite modificar el contador de casillas descubiertas
    global descubiertas

    # Comprueba si la casilla ya fue descubierta
    if visibilidad[fila][columna] == 1:

        etiqueta_mensaje.config(text="Esta casilla ya fue descubierta")
        return

    # Comprueba si la casilla seleccionada tiene una mina
    if minas[fila][columna] == 1:

        etiqueta_mensaje.config(text="¡Encontraste una mina!")

        # Aumenta el contador de reinicios
        reinicios = reinicios + 1

        # Muestra la cantidad de reinicios
        etiqueta_reinicios.config(text="Reinicios: " + str(reinicios))

        # Informa que el tablero se reiniciará
        messagebox.showinfo(
            "Mina encontrada",
            "Encontraste una mina. El tablero se reiniciará."
        )

        # Genera un tablero completamente nuevo
        iniciar_juego()

    else:

        # Marca la casilla como descubierta
        visibilidad[fila][columna] = 1

        # Aumenta el contador de casillas descubiertas
        descubiertas = descubiertas + 1

        # Obtiene el número de minas alrededor
        numero = numeros[fila][columna]

        # Cambia el signo de interrogación por el número
        boton = botones[fila * tamano + columna]

        boton.config(
            text=str(numero),
            state="disabled",
            disabledforeground="black",
            bg="lightgray"
        )

        # Muestra cuántas minas hay alrededor
        etiqueta_mensaje.config(
            text="Minas alrededor: " + str(numero)
        )

        # Comprueba si ya se descubrieron todas las casillas seguras
        if descubiertas == tamano * tamano - num_minas:

            etiqueta_mensaje.config(text="¡Felicidades, ganaste!")

            messagebox.showinfo(
                "Victoria",
                "¡Felicidades! Descubriste todas las casillas sin minas."
            )


# -----------------------------------------
# FUNCIÓN PARA INICIAR O REINICIAR EL JUEGO
# -----------------------------------------

def iniciar_juego():

    # Permite modificar el contador de casillas descubiertas
    global descubiertas

    # Crea las tres matrices del tablero
    global minas, numeros, visibilidad

    # Reinicia el contador de casillas descubiertas
    descubiertas = 0

    # Genera un tablero nuevo
    minas, numeros, visibilidad = tablero()

    # Actualiza todos los botones
    for i in range(tamano):
        for j in range(tamano):

            boton = botones[i * tamano + j]

            boton.config(
                text="?",
                state="normal",
                bg="SystemButtonFace"
            )

    # Actualiza el mensaje de la ventana
    etiqueta_mensaje.config(text="Selecciona una casilla")


# -----------------------------------------
# INICIO DEL PROGRAMA
# -----------------------------------------

# Crea los botones del tablero
crear_botones()

# Genera el primer tablero
iniciar_juego()

# Mantiene abierta la ventana y permite
# que el usuario interactúe con los botones
ventana.mainloop()
