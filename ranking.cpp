// Módulo de historial y ranking de Buscaminas Duelo.
// Diseñado por itz.Natho03
//
// Lee historial.csv (una fila por partida), acumula los resultados de cada
// jugador en una lista enlazada, la ordena con Bubble Sort y escribe ranking.csv.
//
// Uso: ranking [historial.csv] [ranking.csv] [top]
// Compilación: g++ -std=c++17 -Wall -Wextra -O2 -o ranking ranking.cpp

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------
// Nodo de la lista enlazada: un jugador con sus resultados acumulados
// ---------------------------------------------------------------------------
struct Jugador {
    std::string nombre;
    int partidas;
    int victorias;
    int reinicios;        // reinicios acumulados en todas sus partidas
    Jugador* siguiente;

    explicit Jugador(const std::string& n)
        : nombre(n), partidas(0), victorias(0), reinicios(0), siguiente(nullptr) {}
};

// true si a debe ir DESPUÉS de b en el ranking.
// Criterio: más victorias primero; si empatan, menos reinicios acumulados.
// La comparación es estricta, así que los empates completos no se intercambian
// y quedan en el orden en que aparecieron en el historial (orden estable).
bool vaDespues(const Jugador& a, const Jugador& b) {
    if (a.victorias != b.victorias) return a.victorias < b.victorias;
    return a.reinicios > b.reinicios;
}

class ListaJugadores {
private:
    Jugador* cabeza;
    Jugador* cola;   // último nodo: insertar al final cuesta O(1)
    int tamanio;

public:
    ListaJugadores() : cabeza(nullptr), cola(nullptr), tamanio(0) {}
    ~ListaJugadores() { liberar(); }
    ListaJugadores(const ListaJugadores&) = delete;             // la lista es dueña de sus nodos
    ListaJugadores& operator=(const ListaJugadores&) = delete;

    Jugador* buscar(const std::string& nombre) const;
    Jugador* obtenerOCrear(const std::string& nombre);
    bool cargarDesdeCSV(const std::string& ruta, int& validas, int& ignoradas);
    void ordenarBurbuja();
    void mostrar(int topN) const;
    bool guardarRanking(const std::string& ruta, int topN) const;
    void liberar();
    int getTamanio() const { return tamanio; }
};

Jugador* ListaJugadores::buscar(const std::string& nombre) const {
    for (Jugador* p = cabeza; p != nullptr; p = p->siguiente)
        if (p->nombre == nombre) return p;
    return nullptr;
}

// Devuelve el jugador existente o crea un nodo nuevo al final de la lista.
Jugador* ListaJugadores::obtenerOCrear(const std::string& nombre) {
    Jugador* existente = buscar(nombre);
    if (existente != nullptr) return existente;

    Jugador* nuevo = new Jugador(nombre);
    if (cabeza == nullptr) cabeza = cola = nuevo;
    else { cola->siguiente = nuevo; cola = nuevo; }
    tamanio++;
    return nuevo;
}

void ListaJugadores::liberar() {
    while (cabeza != nullptr) {
        Jugador* aux = cabeza;
        cabeza = cabeza->siguiente;
        delete aux;
    }
    cola = nullptr;
    tamanio = 0;
}

// ---------------------------------------------------------------------------
// Lectura del CSV
// ---------------------------------------------------------------------------
static std::vector<std::string> separarPorComas(const std::string& linea) {
    std::vector<std::string> campos;
    std::stringstream ss(linea);
    std::string campo;
    while (std::getline(ss, campo, ',')) campos.push_back(campo);
    return campos;
}

// Convierte todo el texto a un entero >= 0; false si no es un número válido.
static bool aEnteroNoNegativo(const std::string& texto, int& valor) {
    if (texto.empty()) return false;
    try {
        std::size_t usados = 0;
        int v = std::stoi(texto, &usados);
        if (usados != texto.size() || v < 0) return false;
        valor = v;
        return true;
    } catch (...) {
        return false;
    }
}

// Formato: fecha,nombre_j1,reinicios_j1,nombre_j2,reinicios_j2,ganador,tiempo
bool ListaJugadores::cargarDesdeCSV(const std::string& ruta, int& validas, int& ignoradas) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) return false;
    validas = ignoradas = 0;

    std::string linea;
    std::getline(archivo, linea);                       // saltar encabezado
    while (std::getline(archivo, linea)) {
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();   // archivos de Windows
        if (linea.empty()) continue;

        std::vector<std::string> c = separarPorComas(linea);
        int r1 = 0, r2 = 0;
        bool ok = c.size() == 7
                  && !c[1].empty() && !c[3].empty() && c[1] != c[3]
                  && aEnteroNoNegativo(c[2], r1) && aEnteroNoNegativo(c[4], r2)
                  && !c[6].empty()
                  && (c[5] == c[1] || c[5] == c[3] || c[5] == "EMPATE");
        if (!ok) { ignoradas++; continue; }             // fila dañada: no detiene el programa

        // Solo se modifica la lista cuando la fila ya se comprobó completa
        Jugador* j1 = obtenerOCrear(c[1]);
        Jugador* j2 = obtenerOCrear(c[3]);
        j1->partidas++;  j1->reinicios += r1;
        j2->partidas++;  j2->reinicios += r2;
        if (c[5] == c[1]) j1->victorias++;
        else if (c[5] == c[3]) j2->victorias++;
        validas++;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Ordenamiento burbuja sobre la lista enlazada
// ---------------------------------------------------------------------------
// Compara nodos vecinos y, si están en el orden contrario, intercambia sus
// DATOS (los punteros no se tocan, así cabeza y cola siguen siendo válidos).
// En cada pasada el nodo "peor" se va al final, por eso 'fin' retrocede y
// la lista se corta antes. Si una pasada no intercambia nada, termina.
void ListaJugadores::ordenarBurbuja() {
    if (cabeza == nullptr) return;
    Jugador* fin = nullptr;           // desde aquí hacia el final ya está ordenado
    bool huboCambio;
    do {
        huboCambio = false;
        Jugador* p = cabeza;
        while (p->siguiente != fin) {
            if (vaDespues(*p, *p->siguiente)) {
                std::swap(p->nombre,    p->siguiente->nombre);
                std::swap(p->partidas,  p->siguiente->partidas);
                std::swap(p->victorias, p->siguiente->victorias);
                std::swap(p->reinicios, p->siguiente->reinicios);
                huboCambio = true;
            }
            p = p->siguiente;
        }
        fin = p;                      // p quedó en el último nodo comparado
    } while (huboCambio);
}

// ---------------------------------------------------------------------------
// Salida
// ---------------------------------------------------------------------------
void ListaJugadores::mostrar(int topN) const {
    int pos = 1;
    for (Jugador* p = cabeza; p != nullptr && pos <= topN; p = p->siguiente, pos++)
        std::cout << pos << ". " << p->nombre << " - " << p->victorias << " victorias, "
                  << p->reinicios << " reinicios (" << p->partidas << " partidas)\n";
}

bool ListaJugadores::guardarRanking(const std::string& ruta, int topN) const {
    std::ofstream salida(ruta);
    if (!salida.is_open()) return false;
    salida << "posicion,jugador,partidas,victorias,reinicios_acumulados\n";
    int pos = 1;
    for (Jugador* p = cabeza; p != nullptr && pos <= topN; p = p->siguiente, pos++)
        salida << pos << ',' << p->nombre << ',' << p->partidas << ','
               << p->victorias << ',' << p->reinicios << '\n';
    return true;
}

int main(int argc, char* argv[]) {
    std::string entrada = (argc > 1) ? argv[1] : "historial.csv";
    std::string salida  = (argc > 2) ? argv[2] : "ranking.csv";
    int top = 10;
    if (argc > 3 && !aEnteroNoNegativo(argv[3], top)) {
        std::cerr << "El tercer argumento (top) debe ser un entero >= 0\n";
        return 3;
    }

    ListaJugadores lista;
    int validas = 0, ignoradas = 0;
    if (!lista.cargarDesdeCSV(entrada, validas, ignoradas)) {
        std::cerr << "No se pudo abrir " << entrada << "\n";
        return 1;
    }
    if (ignoradas > 0)
        std::cerr << "Aviso: " << ignoradas << " fila(s) del historial se ignoraron por tener datos inválidos\n";

    lista.ordenarBurbuja();
    lista.mostrar(top);
    if (!lista.guardarRanking(salida, top)) {
        std::cerr << "No se pudo escribir " << salida << "\n";
        return 2;
    }
    return 0;      // el destructor libera todos los nodos
}
