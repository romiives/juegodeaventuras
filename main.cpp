// tarea2.cpp — versión completa con elecciones, combate, eventos y mejoras
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

struct Jugador {
    int vida = 30;
    int ataque = 5;
    float precision = 0.7;
};

struct Habitacion {
    int id;
    string nombre;
    string tipo;
    string descripcion;
};

struct NodoHabitacion {
    Habitacion* hab;
    NodoHabitacion* hijo1;
    NodoHabitacion* hijo2;
    NodoHabitacion* hijo3;
};

struct Evento {
    string nombre;
    string descripcion;
    string opcionA_texto;
    string opcionA_consecuencia;
    int opcionA_vida = 0;
    float opcionA_precision = 0;
    string opcionB_texto;
    string opcionB_consecuencia;
    int opcionB_vida = 0;
    float opcionB_precision = 0;
};

Habitacion* habitaciones[100];
int conexiones[100][3];
Evento eventos[10];
int totalHabitaciones = 0;
int totalEventos = 0;

float randomFloat() {
    return static_cast<float>(rand()) / RAND_MAX;
}

string leerLineaSegura(ifstream &archivo) {
    string linea;
    while (getline(archivo, linea)) {
        if (!linea.empty()) return linea;
    }
    cerr << "Error: línea vacía inesperada en archivo." << endl;
    exit(1);
}

int leerEnteroSeguro(ifstream &archivo) {
    string linea = leerLineaSegura(archivo);
    try {
        return stoi(linea);
    } catch (...) {
        cerr << "Error al convertir a entero: " << linea << endl;
        exit(1);
    }
}

NodoHabitacion* construirArbol(int id) {
    if (habitaciones[id] == NULL) return NULL;
    NodoHabitacion* nodo = new NodoHabitacion{habitaciones[id], NULL, NULL, NULL};
    for (int i = 0; i < 3; ++i) {
        if (conexiones[id][i] != 0 || (id == 0 && conexiones[id][i] != 0)) {
            if (i == 0) nodo->hijo1 = construirArbol(conexiones[id][i]);
            if (i == 1) nodo->hijo2 = construirArbol(conexiones[id][i]);
            if (i == 2) nodo->hijo3 = construirArbol(conexiones[id][i]);
        }
    }
    return nodo;
}

void aplicarEvento(Jugador &jugador) {
    int i = rand() % totalEventos;
    Evento e = eventos[i];
    cout << "\n[Evento: " << e.nombre << "]\n" << e.descripcion << endl;
    cout << "A: " << e.opcionA_texto << endl;
    cout << "B: " << e.opcionB_texto << endl;
    char eleccion;
    cin >> eleccion;
    if (eleccion == 'A' || eleccion == 'a') {
        cout << e.opcionA_consecuencia << endl;
        jugador.vida += e.opcionA_vida;
        jugador.precision += e.opcionA_precision;
    } else {
        cout << e.opcionB_consecuencia << endl;
        jugador.vida += e.opcionB_vida;
        jugador.precision += e.opcionB_precision;
    }
    cout << "Vida actual: " << jugador.vida << ", Precisión: " << jugador.precision << endl;
}

void combate(Jugador &jugador) {
    cout << "¡Enfrentas a un enemigo!\n";
    if (randomFloat() < jugador.precision) {
        cout << "¡Lo derrotaste antes de que atacara!\n";
    } else {
        jugador.vida -= 5;
        cout << "El enemigo te golpea. Pierdes 5 de vida.\n";
    }
    cout << "Vida restante: " << jugador.vida << "\n";
    if (jugador.vida <= 0) {
        cout << "Has muerto en combate...\n";
        exit(0);
    }

    cout << "\nElige una mejora:\n1. +3 Vida\n2. +0.2 Precisión\n3. +5 Ataque\n";
    int op;
    cin >> op;
    if (op == 1) jugador.vida += 3;
    else if (op == 2) jugador.precision += 0.2;
    else if (op == 3) jugador.ataque += 5;
    cout << "\nEstadísticas tras mejora — Vida: " << jugador.vida << ", Precisión: " << jugador.precision << ", Ataque: " << jugador.ataque << endl;
}

void jugar(NodoHabitacion* actual, Jugador &jugador) {
    if (!actual) return;
    cout << "\n>> " << actual->hab->nombre << " (" << actual->hab->tipo << ")\n";
    cout << actual->hab->descripcion << "\n";

    if (actual->hab->tipo == "COMBATE") combate(jugador);
    else if (actual->hab->tipo == "EVENTO") aplicarEvento(jugador);
    else if (actual->hab->tipo == "FIN") {
        cout << "\n==> FIN DEL JUEGO\n";
        return;
    }

    cout << "\nOpciones:\n";
    if (actual->hijo1) cout << "1. " << actual->hijo1->hab->nombre << endl;
    if (actual->hijo2) cout << "2. " << actual->hijo2->hab->nombre << endl;
    if (actual->hijo3) cout << "3. " << actual->hijo3->hab->nombre << endl;
    cout << "Seleccione una opción: ";
    int opcion;
    cin >> opcion;

    if (opcion == 1 && actual->hijo1) jugar(actual->hijo1, jugador);
    else if (opcion == 2 && actual->hijo2) jugar(actual->hijo2, jugador);
    else if (opcion == 3 && actual->hijo3) jugar(actual->hijo3, jugador);
    else cout << "Opción inválida.\n";
}

int main() {
    srand(time(0));
    ifstream archivo("data.map");
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo data.map" << endl;
        return 1;
    }

    leerLineaSegura(archivo); // INICIO DE ARCHIVO
    leerLineaSegura(archivo); // HABITACIONES
    int cantidadHabitaciones = leerEnteroSeguro(archivo);

    for (int i = 0; i < cantidadHabitaciones; ++i) {
        string definicion = leerLineaSegura(archivo);
        string descripcion = leerLineaSegura(archivo);
        int pos1 = definicion.find(" ");
        int pos2 = definicion.find("(");
        int id = stoi(definicion.substr(0, pos1));
        string nombre = definicion.substr(pos1 + 1, pos2 - pos1 - 2);
        string tipo = definicion.substr(pos2 + 1);
        tipo.pop_back();
        habitaciones[id] = new Habitacion{id, nombre, tipo, descripcion};
        totalHabitaciones++;
    }

    leerLineaSegura(archivo); // ARCOS
    int cantidadArcos = leerEnteroSeguro(archivo);
    for (int i = 0; i < cantidadArcos; ++i) {
        string arco = leerLineaSegura(archivo);
        int flecha = arco.find("->");
        int a = stoi(arco.substr(0, flecha - 1));
        int b = stoi(arco.substr(flecha + 3));
        for (int j = 0; j < 3; j++) {
            if (conexiones[a][j] == 0) {
                conexiones[a][j] = b;
                break;
            }
        }
    }

    leerLineaSegura(archivo); // ENEMIGOS
    int enemigos = leerEnteroSeguro(archivo);
    for (int i = 0; i < enemigos; ++i) leerLineaSegura(archivo); // ignorar enemigos

    leerLineaSegura(archivo); // EVENTOS
    int ev = leerEnteroSeguro(archivo);
    for (int i = 0; i < ev; ++i) {
        leerLineaSegura(archivo); // &
        eventos[i].nombre = leerLineaSegura(archivo);
        eventos[i].descripcion = leerLineaSegura(archivo);
        eventos[i].opcionA_texto = leerLineaSegura(archivo);
        eventos[i].opcionA_consecuencia = leerLineaSegura(archivo);
        string efectoA = leerLineaSegura(archivo);
        if (efectoA.find("Vida") != string::npos) eventos[i].opcionA_vida = stoi(efectoA);
        if (efectoA.find("Precision") != string::npos) eventos[i].opcionA_precision = stof(efectoA);
        eventos[i].opcionB_texto = leerLineaSegura(archivo);
        eventos[i].opcionB_consecuencia = leerLineaSegura(archivo);
        string efectoB = leerLineaSegura(archivo);
        if (efectoB.find("Vida") != string::npos) eventos[i].opcionB_vida = stoi(efectoB);
        if (efectoB.find("Precision") != string::npos) eventos[i].opcionB_precision = stof(efectoB);
        totalEventos++;
    }

    Jugador jugador;
    NodoHabitacion* raiz = construirArbol(0);
    jugar(raiz, jugador);
    return 0;
}
