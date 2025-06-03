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

// Arrays C-style globales
Habitacion* habitaciones[100]; // Max 100 habitaciones
int conexiones[100][3];        // Max 100 habitaciones, 3 conexiones por habitacion
Evento eventos[10];             // Max 10 eventos
Enemigo enemigos[10];           // Max 10 enemigos

// Contadores globales
int totalHabitaciones = 0;
int totalEventos = 0;
int totalEnemigos = 0;

float randomFloat() {
    return static_cast<float>(rand()) / RAND_MAX;
}

string leerLineaSegura(ifstream &archivo) {
    string linea;
    while (getline(archivo, linea)) {
        if (!linea.empty() && linea.find_first_not_of(" \t\n\r") != string::npos) {
            return linea;
        }
    }
    return ""; // Devolver cadena vacía para indicar fin o error específico
}

int leerEnteroSeguro(ifstream &archivo) {
    string linea = leerLineaSegura(archivo);
    if (linea.empty()) {
        cerr << "Error: Se esperaba un entero pero la linea esta vacia o el archivo termino." << endl;
        exit(1);
    }
    try {
        return stoi(linea);
    } catch (const invalid_argument& e) {
        cerr << "Error: Cadena no es un entero valido: '" << linea << "'" << endl;
        exit(1);
    } catch (const out_of_range& e) {
        cerr << "Error: Entero fuera de rango: '" << linea << "'" << endl;
        exit(1);
    }
}

void parsearEfectos(string linea, int &vida, float &precision) {
    vida = 0;
    precision = 0.0f;
    istringstream ss(linea);
    string palabra;
    string unidad;
    float valor;

    while (ss >> palabra) {
        try {
            valor = stof(palabra);
            if (ss >> unidad) {
                if (unidad.find("prec") != string::npos) {
                    precision += valor;
                } else if (unidad.find("vid") != string::npos) {
                    vida += static_cast<int>(valor);
                }
            }
        } catch (const invalid_argument& e) {
            continue;
        } catch (const out_of_range& e) {
            cerr << "Advertencia: Valor numerico fuera de rango en parsearEfectos: " << palabra << endl;
            continue;
        }
    }
}

NodoHabitacion* construirArbol(int id) {
    // Validar el ID contra el array fijo y el puntero nulo
    if (id < 0 || id >= 100 || habitaciones[id] == nullptr) { // 100 es el tamaño fijo
        return nullptr;
    }

    NodoHabitacion* nodo = new NodoHabitacion{habitaciones[id], nullptr, nullptr, nullptr};

    for (int i = 0; i < 3; ++i) {
        int destino_id = conexiones[id][i];
        // Solo construir el hijo si el destino es válido y el puntero al Habitacion no es nulo
        if (destino_id != -1 && destino_id < 100 && habitaciones[destino_id] != nullptr) { // 100 es el tamaño fijo
            if (i == 0) nodo->hijo1 = construirArbol(destino_id);
            else if (i == 1) nodo->hijo2 = construirArbol(destino_id);
            else if (i == 2) nodo->hijo3 = construirArbol(destino_id);
        }
    }
    return nodo;
}

void aplicarEvento(Jugador &jugador) {
    if (totalEventos == 0) {
        cout << "No hay eventos configurados.\n";
        return;
    }

    int elegido = rand() % totalEventos;
    Evento &e = eventos[elegido]; // Acceso al array fijo

    cout << "\n[Evento: " << e.nombre << "]\n" << e.descripcion << endl;
    cout << "A: " << e.opcionA_texto << endl;
    cout << "B: " << e.opcionB_texto << endl;
    
    char eleccion;
    cin >> eleccion;

    if (eleccion == 'A' || eleccion == 'a') {
        cout << e.opcionA_consecuencia << endl;
        jugador.vida += e.opcionA_vida;
        jugador.precision += e.opcionA_precision;
    } else if (eleccion == 'B' || eleccion == 'b') {
        cout << e.opcionB_consecuencia << endl;
        jugador.vida += e.opcionB_vida;
        jugador.precision += e.opcionB_precision;
    } else {
        cout << "Opcion invalida. No se aplica ningun efecto.\n";
    }
    cout << "Vida: " << jugador.vida << ", Precision: " << jugador.precision << endl;
}


void combate(Jugador &jugador) {
    if (totalEnemigos == 0) {
        cout << "No aparecio ningun enemigo.\n";
        return;
    }

    int indice_enemigo = rand() % totalEnemigos;
    Enemigo e = enemigos[indice_enemigo]; // Copia local del enemigo para el combate

    cout << "\nCombate contra: " << e.nombre << endl;
    while (e.vida > 0 && jugador.vida > 0) {
        cout << "\nTu turno. (Vida: " << jugador.vida << ", Enemigo Vida: " << e.vida << ")\n";
        if (randomFloat() < jugador.precision) {
            e.vida -= jugador.ataque;
            cout << "Atacas e infliges " << jugador.ataque << " de dano. Vida de " << e.nombre << ": " << e.vida << endl;
        } else {
            cout << "Fallaste el ataque.\n";
        }

        if (e.vida > 0) {
            if (randomFloat() < e.precision) {
                jugador.vida -= e.ataque;
                cout << e.nombre << " te golpea e inflige " << e.ataque << " de dano. Tu vida actual: " << jugador.vida << endl;
            } else {
                cout << e.nombre << " fallo el ataque.\n";
            }
        }
    }

    if (jugador.vida <= 0) {
        cout << "\nHas muerto en combate contra " << e.nombre << "...\n";
        exit(0);
    } else {
        cout << "\n¡Has derrotado a " << e.nombre << "!\n";
        cout << "\nElige una mejora:\n";
        cout << "1. +3 Vida\n";
        cout << "2. +0.2 Precision\n";
        cout << "3. +5 Ataque\n";
        int op;
        cin >> op;
        if (op == 1) jugador.vida += 3;
        else if (op == 2) jugador.precision += 0.2f;
        else if (op == 3) jugador.ataque += 5;
        else cout << "Opcion de mejora invalida. No se aplico ninguna mejora.\n";
        cout << "Mejoras aplicadas. Vida: " << jugador.vida << ", Precision: " << jugador.precision << ", Ataque: " << jugador.ataque << endl;
    }
}

void liberarArbol(NodoHabitacion* nodo) {
    if (!nodo) return;
    liberarArbol(nodo->hijo1);
    liberarArbol(nodo->hijo2);
    liberarArbol(nodo->hijo3);
    delete nodo;
}

void jugar(NodoHabitacion* actual, Jugador &jugador) {
    if (!actual) {
        cout << "Error: Habitacion actual es nula. Fin del juego inesperado.\n";
        return;
    }

    cout << "\n>> " << actual->hab->nombre << " (" << actual->hab->tipo << ")\n";
    cout << actual->hab->descripcion << "\n";

    if (actual->hab->tipo == "COMBATE") {
        combate(jugador);
    } else if (actual->hab->tipo == "EVENTO") {
        aplicarEvento(jugador);
    } else if (actual->hab->tipo == "FIN") {
        cout << "\n==> FIN DEL JUEGO <==\n";
        if (actual->hab->id == 11)
            cout << "¡Has ganado el juego!\n";
        else
            cout << "Has llegado a un final alternativo.\n";
        
        liberarArbol(actual); // Libera los nodos del árbol
        // Liberar la memoria de los objetos Habitacion
        for (int i = 0; i < totalHabitaciones; ++i) {
            delete habitaciones[i];
            habitaciones[i] = nullptr; // Opcional: para evitar doble borrado si se llama de nuevo
        }
        exit(0);
    }

    if (jugador.vida <= 0) {
        cout << "Tu vida ha llegado a 0. ¡Fin del juego!\n";
        // Liberar la memoria antes de salir si el juego termina por muerte aquí
        liberarArbol(actual); // Libera el subárbol actual
        for (int i = 0; i < totalHabitaciones; ++i) {
            delete habitaciones[i];
            habitaciones[i] = nullptr;
        }
        exit(0);
    }

    cout << "\nOpciones de destino:\n";
    int num_opciones = 0;
    if (actual->hijo1) { cout << "1. " << actual->hijo1->hab->nombre << endl; num_opciones++; }
    if (actual->hijo2) { cout << "2. " << actual->hijo2->hab->nombre << endl; num_opciones++; }
    if (actual->hijo3) { cout << "3. " << actual->hijo3->hab->nombre << endl; num_opciones++; }

    if (num_opciones == 0) {
        cout << "No hay mas caminos a seguir desde aqui. Fin del juego.\n";
        liberarArbol(actual); // Libera el subárbol actual
        for (int i = 0; i < totalHabitaciones; ++i) {
            delete habitaciones[i];
            habitaciones[i] = nullptr;
        }
        return; // Salir de la recursión si no hay más opciones
    }

    cout << "Seleccione una opcion: ";
    int opcion;
    cin >> opcion;

    switch (opcion) {
        case 1:
            if (actual->hijo1) jugar(actual->hijo1, jugador);
            else cout << "Opcion invalida. Intente de nuevo.\n";
            break;
        case 2:
            if (actual->hijo2) jugar(actual->hijo2, jugador);
            else cout << "Opcion invalida. Intente de nuevo.\n";
            break;
        case 3:
            if (actual->hijo3) jugar(actual->hijo3, jugador);
            else cout << "Opcion invalida. Intente de nuevo.\n";
            break;
        default:
            cout << "Opcion invalida. Fin del juego.\n";
            liberarArbol(actual); // Libera el subárbol actual
            for (int i = 0; i < totalHabitaciones; ++i) {
                delete habitaciones[i];
                habitaciones[i] = nullptr;
            }
            exit(0);
            break;
    }
}

int main() {
    // Inicializar punteros de Habitacion a nullptr
    for (int i = 0; i < 100; ++i) {
        habitaciones[i] = nullptr;
        for (int j = 0; j < 3; ++j) {
            conexiones[i][j] = -1;
        }
    }
    srand(time(0));

    ifstream archivo("data.map");
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo data.map" << endl;
        return 1;
    }

    string temp_line;
    getline(archivo, temp_line); // "INICIO DE ARCHIVO"
    getline(archivo, temp_line); // "HABITACIONES"
    int cantidadHabitacionesArchivo = leerEnteroSeguro(archivo);

    // Si el número de habitaciones en el archivo excede el tamaño del array
    if (cantidadHabitacionesArchivo > 100) {
        cerr << "Error: El numero de habitaciones en el archivo (" << cantidadHabitacionesArchivo << ") excede el limite del programa (100)." << endl;
        return 1;
    }
    totalHabitaciones = cantidadHabitacionesArchivo; // Actualizar el contador global

    for (int i = 0; i < totalHabitaciones; ++i) { // Iterar hasta totalHabitaciones
        string def = leerLineaSegura(archivo);
        if (def.empty()) {
            cerr << "Error: Linea de definicion de habitacion vacia inesperada." << endl;
            exit(1);
        }
        string desc = leerLineaSegura(archivo);
        if (desc.empty()) {
            cerr << "Error: Linea de descripcion de habitacion vacia inesperada." << endl;
            exit(1);
        }
        
        size_t pos_id_end = def.find(" ");
        int id = stoi(def.substr(0, pos_id_end));
        
        size_t pos_name_start = pos_id_end + 1;
        size_t pos_type_start = def.find("(", pos_name_start);
        string nombre = def.substr(pos_name_start, pos_type_start - pos_name_start -1);
        
        string tipo = def.substr(pos_type_start + 1);
        tipo.pop_back();

        if (id >= 0 && id < 100) { // Usar 100 como límite
            habitaciones[id] = new Habitacion{id, nombre, tipo, desc};
        } else {
            cerr << "Error: ID de habitacion " << id << " fuera de rango o invalido.\n";
            // Liberar memoria ya asignada antes de salir
            for (int k = 0; k < 100; ++k) delete habitaciones[k];
            return 1;
        }
    }

    getline(archivo, temp_line); // "ARCOS"
    int cantidadArcos = leerEnteroSeguro(archivo);
    for (int i = 0; i < cantidadArcos; ++i) {
        string arco = leerLineaSegura(archivo);
        if (arco.empty()) {
             cerr << "Error: Linea de arco vacia inesperada." << endl;
             exit(1);
        }
        size_t arrow_pos = arco.find("->");
        int a = stoi(arco.substr(0, arrow_pos));
        int b = stoi(arco.substr(arrow_pos + 2));

        if (a >= 0 && a < 100) {
            bool added = false;
            for (int j = 0; j < 3; ++j) {
                if (conexiones[a][j] == -1) {
                    conexiones[a][j] = b;
                    added = true;
                    break;
                }
            }
            if (!added) {
                cerr << "Advertencia: Demasiadas conexiones para la habitacion " << a << ". Se ignoro la conexion a " << b << endl;
            }
        } else {
            cerr << "Error: ID de origen de arco " << a << " fuera de rango.\n";
        }
    }

    getline(archivo, temp_line); // "ENEMIGOS"
    int num_enemigos_archivo = leerEnteroSeguro(archivo);
    if (num_enemigos_archivo > 10) {
        cerr << "Error: El numero de enemigos en el archivo (" << num_enemigos_archivo << ") excede el limite del programa (10)." << endl;
        // Liberar memoria ya asignada antes de salir
        for (int k = 0; k < 100; ++k) delete habitaciones[k];
        return 1;
    }
    totalEnemigos = num_enemigos_archivo; // Actualizar contador global

    for (int i = 0; i < totalEnemigos; ++i) { // Iterar hasta totalEnemigos
        string linea = leerLineaSegura(archivo);
        if (linea.empty()) {
            cerr << "Error: Linea de enemigo vacia inesperada." << endl;
            exit(1);
        }
        size_t p_name_end = linea.find(" (Vida: ");
        if (p_name_end == string::npos) { cerr << "Error de formato de enemigo (Vida)." << endl; exit(1); }
        enemigos[i].nombre = linea.substr(0, p_name_end);

        size_t p_vida_start = p_name_end + 8;
        size_t p_vida_end = linea.find(", Ataque: ", p_vida_start);
        if (p_vida_end == string::npos) { cerr << "Error de formato de enemigo (Ataque)." << endl; exit(1); }
        enemigos[i].vida = stoi(linea.substr(p_vida_start, p_vida_end - p_vida_start));

        size_t p_ataque_start = p_vida_end + 10;
        size_t p_ataque_end = linea.find(", Precision: ", p_ataque_start);
        if (p_ataque_end == string::npos) { cerr << "Error de formato de enemigo (Precision)." << endl; exit(1); }
        enemigos[i].ataque = stoi(linea.substr(p_ataque_start, p_ataque_end - p_ataque_start));

        size_t p_prec_start = p_ataque_end + 13;
        size_t p_prec_end = linea.find(", Probabilidad: ", p_prec_start);
        if (p_prec_end == string::npos) { cerr << "Error de formato de enemigo (Probabilidad)." << endl; exit(1); }
        enemigos[i].precision = stof(linea.substr(p_prec_start, p_prec_end - p_prec_start));

        size_t p_prob_start = p_prec_end + 16;
        enemigos[i].probabilidad = stof(linea.substr(p_prob_start));
    }


    getline(archivo, temp_line); // "EVENTOS"
    int num_eventos_archivo = leerEnteroSeguro(archivo);
    if (num_eventos_archivo > 10) {
        cerr << "Error: El numero de eventos en el archivo (" << num_eventos_archivo << ") excede el limite del programa (10)." << endl;
        // Liberar memoria ya asignada antes de salir
        for (int k = 0; k < 100; ++k) delete habitaciones[k];
        return 1;
    }
    totalEventos = num_eventos_archivo; // Actualizar contador global

    for (int i = 0; i < totalEventos; ++i) { // Iterar hasta totalEventos
        getline(archivo, temp_line); // "&"
        eventos[i].nombre = leerLineaSegura(archivo);
        if (eventos[i].nombre.empty()) { cerr << "Error: nombre de evento vacio." << endl; exit(1); }

        string prob_line = leerLineaSegura(archivo);
        if (prob_line.empty()) { cerr << "Error: linea de probabilidad de evento vacia." << endl; exit(1); }
        eventos[i].probabilidad = stof(prob_line.substr(prob_line.find(" ") + 1));
        
        eventos[i].descripcion = leerLineaSegura(archivo);
        if (eventos[i].descripcion.empty()) { cerr << "Error: descripcion de evento vacia." << endl; exit(1); }

        eventos[i].opcionA_texto = leerLineaSegura(archivo);
        if (eventos[i].opcionA_texto.empty()) { cerr << "Error: texto A de evento vacio." << endl; exit(1); }
        eventos[i].opcionA_consecuencia = leerLineaSegura(archivo);
        if (eventos[i].opcionA_consecuencia.empty()) { cerr << "Error: consecuencia A de evento vacio." << endl; exit(1); }
        string efectoA = leerLineaSegura(archivo);
        if (efectoA.empty()) { cerr << "Error: efecto A de evento vacio." << endl; exit(1); }
        parsearEfectos(efectoA, eventos[i].opcionA_vida, eventos[i].opcionA_precision);

        eventos[i].opcionB_texto = leerLineaSegura(archivo);
        if (eventos[i].opcionB_texto.empty()) { cerr << "Error: texto B de evento vacio." << endl; exit(1); }
        eventos[i].opcionB_consecuencia = leerLineaSegura(archivo);
        if (eventos[i].opcionB_consecuencia.empty()) { cerr << "Error: consecuencia B de evento vacio." << endl; exit(1); }
        string efectoB = leerLineaSegura(archivo);
        if (efectoB.empty()) { cerr << "Error: efecto B de evento vacio." << endl; exit(1); }
        parsearEfectos(efectoB, eventos[i].opcionB_vida, eventos[i].opcionB_precision);
    }
    archivo.close();

    if (habitaciones[0] == nullptr) {
        cout << "Error: habitacion inicial (ID 0) no existe o no se cargo correctamente.\n";
        for (int i = 0; i < 100; ++i) delete habitaciones[i];
        return 1;
    }

    NodoHabitacion* raiz = construirArbol(0);
    if (!raiz) {
        cerr << "Error: No se pudo construir el arbol de habitaciones desde la raiz (ID 0).\n";
        for (int i = 0; i < 100; ++i) delete habitaciones[i];
        return 1;
    }

    Jugador jugador;
    jugar(raiz, jugador);

    // Este bloque de liberacion de memoria solo se alcanzará si la funcion jugar
    // no termina con exit(), lo cual es poco probable para un juego completo.
    // La liberacion de memoria principal se hace dentro de jugar() al alcanzar un FIN o morir.
    liberarArbol(raiz);
    for (int i = 0; i < 100; ++i) delete habitaciones[i];

    return 0;
}

