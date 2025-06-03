// tarea2.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

// ---------------------- ESTRUCTURAS ----------------------
struct Enemigo {
    string nombre;
    int vida;
    int ataque;
    float precision;
    float probabilidad;
    Enemigo* sig;
};

struct NodoCola {
    Enemigo* enemigo;
    NodoCola* sig;
};

struct ColaCombate {
    NodoCola* frente;
    NodoCola* final;

    ColaCombate() {
        frente = final = NULL;
    }

    void encolar(Enemigo* enemigo) {
        NodoCola* nuevo = new NodoCola{enemigo, NULL};
        if (final == NULL) frente = final = nuevo;
        else {
            final->sig = nuevo;
            final = nuevo;
        }
    }

    void desencolar() {
        if (frente == NULL) return;
        NodoCola* temp = frente;
        frente = frente->sig;
        if (frente == NULL) final = NULL;
        delete temp;
    }

    bool vacia() {
        return frente == NULL;
    }
};

struct EventoOpcion {
    string texto;
    string consecuencia;
    int cambioVida = 0;
    float cambioPrecision = 0.0;
};

struct Evento {
    string nombre;
    float probabilidad;
    EventoOpcion opcionA;
    EventoOpcion opcionB;
};

struct Jugador {
    int vida = 30;
    int ataque = 7;
    float precision = 0.7;
    int recuperacion = 3;
};

struct Habitacion {
    int id;
    string nombre;
    string tipo;
    string descripcion;
};

struct NodoHabitacion {
    Habitacion* habitacion;
    NodoHabitacion* hijo1;
    NodoHabitacion* hijo2;
    NodoHabitacion* hijo3;
};

const int MAX_HABITACIONES = 100;
Habitacion* habitaciones[MAX_HABITACIONES];
int adyacencia[MAX_HABITACIONES][3];
int totalHabitaciones = 0;

Enemigo enemigos[10];
int totalEnemigos = 0;
Evento eventos[10];
int totalEventos = 0;

float randomFloat() {
    return (float)rand() / RAND_MAX;
}

// ---------------------- LECTURA DEL ARCHIVO ----------------------
void leerArchivo(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo.c_str());
    string linea;
    if (!archivo.is_open()) {
        cout << "Error al abrir archivo." << endl;
        exit(1);
    }

    getline(archivo, linea); // INICIO DE ARCHIVO
    getline(archivo, linea); // HABITACIONES
    int numHabitaciones;
    archivo >> numHabitaciones;
    archivo.ignore();

    for (int i = 0; i < numHabitaciones; ++i) {
        getline(archivo, linea);
        int pos1 = linea.find(" ");
        int pos2 = linea.find("(");
        int id = stoi(linea.substr(0, pos1));
        string nombre = linea.substr(pos1 + 1, pos2 - pos1 - 2);
        string tipo = linea.substr(pos2 + 1);
        tipo.pop_back();
        getline(archivo, linea);
        string descripcion = linea;
        habitaciones[id] = new Habitacion{id, nombre, tipo, descripcion};
        totalHabitaciones++;
    }

    getline(archivo, linea); // ARCOS
    int numArcos;
    archivo >> numArcos;
    archivo.ignore();
    for (int i = 0; i < numArcos; ++i) {
        getline(archivo, linea);
        int pos = linea.find("->");
        int a = stoi(linea.substr(0, pos - 1));
        int b = stoi(linea.substr(pos + 3));
        for (int j = 0; j < 3; ++j) {
            if (adyacencia[a][j] == 0) {
                adyacencia[a][j] = b;
                break;
            }
        }
    }

    getline(archivo, linea); // ENEMIGOS
    archivo >> totalEnemigos;
    archivo.ignore();
    for (int i = 0; i < totalEnemigos; ++i) {
        getline(archivo, linea);
        size_t p1 = linea.find("|");
        size_t p2 = linea.find("|", p1 + 1);
        size_t p3 = linea.find("|", p2 + 1);
        size_t p4 = linea.find("|", p3 + 1);
        enemigos[i].nombre = linea.substr(0, p1 - 1);
        enemigos[i].vida = stoi(linea.substr(p1 + 6, p2 - p1 - 6));
        enemigos[i].ataque = stoi(linea.substr(p2 + 9, p3 - p2 - 9));
        enemigos[i].precision = stof(linea.substr(p3 + 11, p4 - p3 - 11));
        enemigos[i].probabilidad = stof(linea.substr(p4 + 16));
    }

    getline(archivo, linea); // EVENTOS
    archivo >> totalEventos;
    archivo.ignore();
    for (int i = 0; i < totalEventos; ++i) {
        getline(archivo, linea); // &
        getline(archivo, eventos[i].nombre);
        getline(archivo, linea);
        eventos[i].probabilidad = stof(linea.substr(linea.find(" ") + 1));
        getline(archivo, eventos[i].opcionA.texto);
        getline(archivo, eventos[i].opcionA.consecuencia);
        getline(archivo, linea);
        eventos[i].opcionA.cambioVida = (linea.find("Vida") != string::npos) ? stoi(linea) : 0;
        getline(archivo, eventos[i].opcionB.texto);
        getline(archivo, eventos[i].opcionB.consecuencia);
        getline(archivo, linea);
        eventos[i].opcionB.cambioVida = (linea.find("Vida") != string::npos) ? stoi(linea) : 0;
    }

    archivo.close();
}

// ------------------ CONSTRUCCIÓN DEL ÁRBOL ------------------
NodoHabitacion* construirArbol(int id) {
    if (habitaciones[id] == NULL) return NULL;
    NodoHabitacion* nodo = new NodoHabitacion{habitaciones[id], NULL, NULL, NULL};
    for (int i = 0; i < 3; ++i) {
        if (adyacencia[id][i] != 0) {
            if (i == 0) nodo->hijo1 = construirArbol(adyacencia[id][i]);
            if (i == 1) nodo->hijo2 = construirArbol(adyacencia[id][i]);
            if (i == 2) nodo->hijo3 = construirArbol(adyacencia[id][i]);
        }
    }
    return nodo;
}

// ------------------ COMBATE ------------------
void combate(Jugador& jugador) {
    ColaCombate cola;
    for (int i = 0; i < totalEnemigos; ++i) {
        if (randomFloat() < enemigos[i].probabilidad) {
            Enemigo* e = new Enemigo(enemigos[i]);
            cola.encolar(e);
        }
    }
    if (cola.vacia()) return;

    while (!cola.vacia()) {
        Enemigo* e = cola.frente->enemigo;
        if (randomFloat() < jugador.precision) e->vida -= jugador.ataque;
        if (e->vida <= 0) {
            cola.desencolar();
            continue;
        }
        if (randomFloat() < e->precision) jugador.vida -= e->ataque;
        if (jugador.vida <= 0) {
            cout << "Has muerto.\n";
            exit(0);
        }
    }

    jugador.vida += jugador.recuperacion;
}

// ------------------ EVENTOS ------------------
void evento(Jugador& jugador) {
    for (int i = 0; i < totalEventos; ++i) {
        if (randomFloat() < eventos[i].probabilidad) {
            cout << eventos[i].nombre << endl;
            cout << "A: " << eventos[i].opcionA.texto << "\n";
            cout << "B: " << eventos[i].opcionB.texto << "\n";
            char c;
            cin >> c;
            if (c == 'A' || c == 'a') {
                jugador.vida += eventos[i].opcionA.cambioVida;
            } else {
                jugador.vida += eventos[i].opcionB.cambioVida;
            }
            return;
        }
    }
}

// ------------------ JUGAR ------------------
void jugar(NodoHabitacion* nodo, Jugador& jugador) {
    if (!nodo) return;
    cout << "\n--- " << nodo->habitacion->nombre << " ---\n";
    cout << nodo->habitacion->descripcion << endl;

    if (nodo->habitacion->tipo == "COMBATE") combate(jugador);
    else if (nodo->habitacion->tipo == "EVENTO") evento(jugador);
    else if (nodo->habitacion->tipo == "FIN") {
        cout << "Final del juego.\n";
        return;
    }

    if (nodo->hijo1) cout << "1. " << nodo->hijo1->habitacion->nombre << endl;
    if (nodo->hijo2) cout << "2. " << nodo->hijo2->habitacion->nombre << endl;
    if (nodo->hijo3) cout << "3. " << nodo->hijo3->habitacion->nombre << endl;
    int op;
    cin >> op;
    if (op == 1) jugar(nodo->hijo1, jugador);
    else if (op == 2) jugar(nodo->hijo2, jugador);
    else if (op == 3) jugar(nodo->hijo3, jugador);
}

// ------------------ MAIN ------------------
int main() {
    srand(time(0));
    leerArchivo("data.map");
    NodoHabitacion* raiz = construirArbol(0);
    Jugador jugador;
    jugar(raiz, jugador);
    return 0;
}
