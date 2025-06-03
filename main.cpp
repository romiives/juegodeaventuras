#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

class Jugador {
public:
    int vida;
    int ataque;
    float precision;
    int recuperacion;

    Jugador() {
        vida = 30;
        ataque = 5;
        precision = 0.6f;
        recuperacion = 3;
    }

    void mostrar() {
        cout << "Vida: " << vida << " | Ataque: " << ataque << " | Precisión: " << precision << " | Recuperación: " << recuperacion << endl;
    }

    void mejorar(int opcion) {
        if (opcion == 1) vida += 3;
        else if (opcion == 2) precision += 0.2f;
        else if (opcion == 3) ataque += 5;
        else if (opcion == 4) recuperacion += 4;
    }

    void curar() {
        vida += recuperacion;
        cout << "Recuperas " << recuperacion << " de vida. Vida actual: " << vida << endl;
    }
};

struct NodoEnemigo {
    string nombre;
    int vida, ataque;
    float precision;
    NodoEnemigo* sig;
};

class ColaEnemigos {
public:
    NodoEnemigo* frente;
    NodoEnemigo* fin;

    ColaEnemigos() {
        frente = fin = NULL;
    }

    void encolar(string nombre, int vida, int ataque, float precision) {
        NodoEnemigo* nuevo = new NodoEnemigo{nombre, vida, ataque, precision, NULL};
        if (!frente) frente = fin = nuevo;
        else {
            fin->sig = nuevo;
            fin = nuevo;
        }
    }

    void desencolar() {
        if (frente) {
            NodoEnemigo* aux = frente;
            frente = frente->sig;
            delete aux;
        }
    }

    NodoEnemigo* primero() { return frente; }

    bool vacia() { return frente == NULL; }

    void liberar() {
        while (!vacia()) desencolar();
    }
};

enum TipoHabitacion { INICIO, EVENTO, COMBATE, FIN };

struct Habitacion {
    int id;
    string nombre;
    string descripcion;
    TipoHabitacion tipo;
    Habitacion* hijos[3];

    Habitacion(int i, string n, string d, TipoHabitacion t) {
        id = i;
        nombre = n;
        descripcion = d;
        tipo = t;
        for (int j = 0; j < 3; j++) hijos[j] = NULL;
    }
};

bool ataqueExitoso(float p) {
    return (float(rand()) / RAND_MAX) < p;
}

void enemigosPorID(int id, ColaEnemigos& cola) {
    if (id == 2) {
        cola.encolar("Pantera", 12, 5, 0.8f);
    } else if (id == 4) {
        cola.encolar("Tribu Hostil", 15, 4, 0.6f);
        cola.encolar("Serpiente Gigante", 8, 7, 0.5f);
    } else if (id == 9) {
        cola.encolar("Bestia de la Cueva", 20, 10, 0.3f);
    }
}

bool ejecutarCombate(Jugador &jugador, int habitacionId) {
    ColaEnemigos enemigos;
    enemigosPorID(habitacionId, enemigos);
    cout << "\nComienza el combate." << endl;
    while (jugador.vida > 0 && !enemigos.vacia()) {
        NodoEnemigo* enemigo = enemigos.primero();

        if (ataqueExitoso(jugador.precision)) {
            enemigo->vida -= jugador.ataque;
            cout << "Golpeas a " << enemigo->nombre << " por " << jugador.ataque << endl;
        } else {
            cout << "Fallas tu ataque." << endl;
        }

        if (enemigo->vida <= 0) {
            cout << enemigo->nombre << " ha sido derrotado." << endl;
            enemigos.desencolar();
            continue;
        }

        if (ataqueExitoso(enemigo->precision)) {
            jugador.vida -= enemigo->ataque;
            cout << enemigo->nombre << " te golpea por " << enemigo->ataque << endl;
        } else {
            cout << enemigo->nombre << " falla su ataque." << endl;
        }

        jugador.mostrar();
    }

    enemigos.liberar();

    if (jugador.vida > 0) {
        cout << "Has sobrevivido al combate." << endl;
        jugador.curar();
        cout << "Elige una mejora:\n1. +3 Vida\n2. +0.2 Precisión\n3. +5 Ataque\n4. +4 Recuperación\n> ";
        int op;
        cin >> op;
        jugador.mejorar(op);
        return true;
    } else {
        cout << "Fuiste derrotado." << endl;
        return false;
    }
}

void ejecutarEvento(Jugador &jugador) {
    float r = (float(rand()) / RAND_MAX);
    cout << "\nEncuentras un evento extraño.\n";

    if (r < 0.33f) {
        cout << "Ves un cofre. ¿Qué haces?\n1. Abrirlo\n2. Ignorarlo\n> ";
        int op; cin >> op;
        if (op == 1) jugador.vida -= 5;
    } else if (r < 0.66f) {
        cout << "Escuchas un grito. ¿Qué haces?\n1. Acercarte\n2. Alejarte\n> ";
        int op; cin >> op;
        if (op == 1) jugador.vida += 10;
        else jugador.vida -= 2;
    } else {
        cout << "Sientes que te observan. ¿Qué haces?\n1. Acercarte\n2. Esconderte\n> ";
        int op; cin >> op;
        if (op == 1) jugador.vida -= 2;
        else jugador.vida += 1;
    }

    jugador.curar();
}

void recorrer(Habitacion* actual, Jugador& jugador) {
    if (!actual) return;

    cout << "\n" << actual->nombre << "\n" << actual->descripcion << endl;
    jugador.mostrar();

    if (actual->tipo == COMBATE) {
        if (!ejecutarCombate(jugador, actual->id)) return;
    } else if (actual->tipo == EVENTO) {
        ejecutarEvento(jugador);
    } else if (actual->tipo == FIN) {
        cout << "Fin del juego. " << actual->descripcion << endl;
        return;
    }

    int opciones = 0;
    for (int i = 0; i < 3; i++)
        if (actual->hijos[i]) opciones++;

    if (opciones == 0) {
        cout << "No hay más caminos. Fin del juego.\n";
        return;
    }

    cout << "¿Dónde deseas ir?\n";
    for (int i = 0; i < opciones; i++)
        cout << i + 1 << ". " << actual->hijos[i]->nombre << endl;

    int eleccion;
    cout << "> ";
    cin >> eleccion;

    if (eleccion >= 1 && eleccion <= opciones)
        recorrer(actual->hijos[eleccion - 1], jugador);
}

void liberarArbol(Habitacion* h) {
    if (!h) return;
    for (int i = 0; i < 3; i++) liberarArbol(h->hijos[i]);
    delete h;
}

int main() {
    srand(time(0));
    Jugador jugador;
    Habitacion* raiz = cargarMapa("data.map");
    recorrer(raiz, jugador);
    liberarArbol(raiz);

    return 0;
}
