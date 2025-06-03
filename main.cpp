#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>

using namespace std;

struct Jugador {
    int vida = 30;
    int ataque = 5;
    float precision = 0.7;
};

struct Enemigo {
    string nombre;
    int vida;
    int ataque;
    float precision;
    float probabilidad;
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
    float probabilidad;
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

Habitacion* habitaciones[100] = {nullptr};
int conexiones[100][3] = {{0}};
Evento eventos[10];
int totalHabitaciones = 0;
int totalEventos = 0;
Enemigo enemigos[10];
int totalEnemigos = 0;

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

void parsearEfectos(string linea, int &vida, float &precision) {
    vida = 0;
    precision = 0;
    istringstream ss(linea);
    string palabra;
    while (ss >> palabra) {
        try {
            float valor = stof(palabra);
            string unidad;
            ss >> unidad;
            if (unidad.find("prec") != string::npos)
                precision += valor;
            else if (unidad.find("vid") != string::npos)
                vida += static_cast<int>(valor);
        } catch (...) {
            continue;
        }
    }
}

NodoHabitacion* construirArbol(int id) {
    if (id < 0 || id >= 100 || habitaciones[id] == NULL) return NULL;
    NodoHabitacion* nodo = new NodoHabitacion{habitaciones[id], NULL, NULL, NULL};
    for (int i = 0; i < 3; ++i) {
        int destino = conexiones[id][i];
        if (destino >= 0 && destino < 100 && habitaciones[destino] != NULL) {
            if (i == 0) nodo->hijo1 = construirArbol(destino);
            if (i == 1) nodo->hijo2 = construirArbol(destino);
            if (i == 2) nodo->hijo3 = construirArbol(destino);
        }
    }
    return nodo;
}

void aplicarEvento(Jugador &jugador) {
    for (int i = 0; i < totalEventos; ++i) {
        if (randomFloat() < eventos[i].probabilidad) {
            Evento &e = eventos[i];
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
            cout << "Vida: " << jugador.vida << ", Precisión: " << jugador.precision << endl;
            return;
        }
    }
    cout << "No ocurrió ningún evento esta vez.\n";
}

void combate(Jugador &jugador) {
    bool huboCombate = false;
    for (int i = 0; i < totalEnemigos; ++i) {
        if (randomFloat() < enemigos[i].probabilidad) {
            Enemigo e = enemigos[i];
            cout << "\nCombate contra: " << e.nombre << endl;
            while (e.vida > 0 && jugador.vida > 0) {
                if (randomFloat() < jugador.precision) {
                    e.vida -= jugador.ataque;
                    cout << "Atacas e infliges " << jugador.ataque << ". Vida enemigo: " << e.vida << endl;
                } else cout << "Fallaste el ataque.\n";
                if (e.vida > 0 && randomFloat() < e.precision) {
                    jugador.vida -= e.ataque;
                    cout << e.nombre << " te golpea. Vida actual: " << jugador.vida << endl;
                } else if (e.vida > 0) {
                    cout << e.nombre << " falló el ataque.\n";
                }
            }
            if (jugador.vida <= 0) {
                cout << "Has muerto en combate...\n";
                exit(0);
            }
            huboCombate = true;
        }
    }
    if (!huboCombate) {
        cout << "No apareció ningún enemigo.\n";
    } else {
        cout << "\nElige una mejora:\n1. +3 Vida\n2. +0.2 Precisión\n3. +5 Ataque\n";
        int op;
        cin >> op;
        if (op == 1) jugador.vida += 3;
        else if (op == 2) jugador.precision += 0.2;
        else if (op == 3) jugador.ataque += 5;
        cout << "Mejoras aplicadas. Vida: " << jugador.vida << ", Precisión: " << jugador.precision << ", Ataque: " << jugador.ataque << endl;
    }
}

void jugar(NodoHabitacion* actual, Jugador &jugador) {
    if (!actual) return;
    cout << "\n>> " << actual->hab->nombre << " (" << actual->hab->tipo << ")\n";
    cout << actual->hab->descripcion << "\n";

    if (actual->hab->tipo == "COMBATE") combate(jugador);
    else if (actual->hab->tipo == "EVENTO") aplicarEvento(jugador);
    else if (actual->hab->tipo == "FIN") {
        cout << "\n==> FIN DEL JUEGO\n";
        if (actual->hab->id == 11)
            cout << "¡Has ganado el juego!\n";
        else
            cout << "Has llegado a un final alternativo.\n";
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
    else {
        cout << "Opción inválida. Fin del juego.\n";
    }
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
        string def = leerLineaSegura(archivo);
        string desc = leerLineaSegura(archivo);
        int id = stoi(def.substr(0, def.find(" ")));
        int pos1 = def.find(" ");
        int pos2 = def.find("(");
        string nombre = def.substr(pos1 + 1, pos2 - pos1 - 2);
        string tipo = def.substr(pos2 + 1);
        tipo.pop_back();
        if (id >= 0 && id < 100) {
            habitaciones[id] = new Habitacion{id, nombre, tipo, desc};
            totalHabitaciones++;
        }
    }

    leerLineaSegura(archivo); // ARCOS
    int cantidadArcos = leerEnteroSeguro(archivo);
    for (int i = 0; i < cantidadArcos; ++i) {
        string arco = leerLineaSegura(archivo);
        int a = stoi(arco.substr(0, arco.find("->") - 1));
        int b = stoi(arco.substr(arco.find("->") + 3));
        if (a >= 0 && a < 100 && b >= 0 && b < 100) {
            for (int j = 0; j < 3; ++j) {
                if (conexiones[a][j] == 0) {
                    conexiones[a][j] = b;
                    break;
                }
            }
        }
    }

    leerLineaSegura(archivo); // ENEMIGOS
    totalEnemigos = leerEnteroSeguro(archivo);
    for (int i = 0; i < totalEnemigos; ++i) {
        string linea = leerLineaSegura(archivo);
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

    leerLineaSegura(archivo); // EVENTOS
    int ev = leerEnteroSeguro(archivo);
    for (int i = 0; i < ev; ++i) {
        leerLineaSegura(archivo); // &
        eventos[i].nombre = leerLineaSegura(archivo);
        string prob = leerLineaSegura(archivo);
        eventos[i].probabilidad = stof(prob.substr(prob.find(" ") + 1));
        eventos[i].descripcion = leerLineaSegura(archivo);
        eventos[i].opcionA_texto = leerLineaSegura(archivo);
        eventos[i].opcionA_consecuencia = leerLineaSegura(archivo);
        string efectoA = leerLineaSegura(archivo);
        parsearEfectos(efectoA, eventos[i].opcionA_vida, eventos[i].opcionA_precision);
        eventos[i].opcionB_texto = leerLineaSegura(archivo);
        eventos[i].opcionB_consecuencia = leerLineaSegura(archivo);
        string efectoB = leerLineaSegura(archivo);
        parsearEfectos(efectoB, eventos[i].opcionB_vida, eventos[i].opcionB_precision);
        totalEventos++;
    }

    if (!habitaciones[0]) {
        cout << "Error: habitación inicial (ID 0) no existe.\n";
        return 1;
    }

    NodoHabitacion* raiz = construirArbol(0);
    Jugador jugador;
    jugar(raiz, jugador);
    return 0;
}
