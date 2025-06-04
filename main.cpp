#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>


char toLowerChar(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');
    }
    return c;
}

using namespace std;

//struct

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
    Habitacion* habitacion;
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

Habitacion* habitaciones[100];
int conexiones[100][3];
Evento eventos[10];
Enemigo enemigos[10];

int totalHabitaciones = 0;
int totalEventos = 0;
int totalEnemigos = 0;

float generarFloatAleatorio() {
    return static_cast<float>(rand()) / RAND_MAX;
}

string limpiarEspacios(const string& str) {
    size_t primerNoEspacio = str.find_first_not_of(" \t\n\r");
    if (string::npos == primerNoEspacio) {
        return "";
    }
    size_t ultimoNoEspacio = str.find_last_not_of(" \t\n\r");
    return str.substr(primerNoEspacio, (ultimoNoEspacio - primerNoEspacio + 1));
}

string leerLineaSegura(ifstream &archivo) {
    string linea;
    while (getline(archivo, linea)) {
        linea = limpiarEspacios(linea);
        if (!linea.empty()) {
            return linea;
        }
    }
    return "";
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
    
    string lineaMinusculas = "";
    for (char c : linea) {
        lineaMinusculas += toLowerChar(c);
    }

    istringstream streamMinusculas(lineaMinusculas);
    string token;
    string valorActualStr;
    bool esperandoValor = false;

    while (streamMinusculas >> token) {
        if (token == "+" || token == "-") {
            valorActualStr = token;
            esperandoValor = true;
        } else if (esperandoValor) {
            valorActualStr += token;
            esperandoValor = false;
        } else if (token.find("vida") != string::npos || token.find("vid") != string::npos) {
            if (!valorActualStr.empty()) {
                try {
                    vida += stoi(valorActualStr);
                } catch (...) {}
                valorActualStr = "";
            }
        } else if (token.find("precision") != string::npos || token.find("prec") != string::npos) {
            if (!valorActualStr.empty()) {
                try {
                    precision += stof(valorActualStr);
                } catch (...) {}
                valorActualStr = "";
            }
        }
    }
}

NodoHabitacion* construirArbol(int id) {
    if (id < 0 || id >= 100 || habitaciones[id] == nullptr) {
        return nullptr;
    }

    NodoHabitacion* nodo = new NodoHabitacion{habitaciones[id], nullptr, nullptr, nullptr};

    for (int i = 0; i < 3; ++i) {
        int idDestino = conexiones[id][i];
        if (idDestino != -1 && idDestino < 100 && habitaciones[idDestino] != nullptr) {
            if (i == 0) nodo->hijo1 = construirArbol(idDestino);
            else if (i == 1) nodo->hijo2 = construirArbol(idDestino);
            else if (i == 2) nodo->hijo3 = construirArbol(idDestino);
        }
    }
    return nodo;
}

void aplicarEvento(Jugador &jugador) {
    if (totalEventos == 0) {
        cout << "No hay eventos configurados.\n";
        return;
    }

    int indiceElegido = rand() % totalEventos;
    Evento &eventoElegido = eventos[indiceElegido];

    cout << "\n[Evento: " << eventoElegido.nombre << "]\n";
    cout << eventoElegido.descripcion << endl;
    cout << eventoElegido.opcionA_texto << endl;
    cout << eventoElegido.opcionB_texto << endl;
    
    char eleccion;
    cin >> eleccion;

    if (eleccion == 'A' || eleccion == 'a') {
        cout << eventoElegido.opcionA_consecuencia << endl;
        jugador.vida += eventoElegido.opcionA_vida;
        jugador.precision += eventoElegido.opcionA_precision;
    } else if (eleccion == 'B' || eleccion == 'b') {
        cout << eventoElegido.opcionB_consecuencia << endl;
        jugador.vida += eventoElegido.opcionB_vida;
        jugador.precision += eventoElegido.opcionB_precision;
    } else {
        cout << "Opcion invalida. No se aplica ningun efecto.\n";
    }

    cout << "Vida: " << jugador.vida << ", Precision: " << jugador.precision << endl;
}

void combate(Jugador &jugador) {
    if (totalEnemigos == 0) {
        cout << "🎀 No hay criaturas malvadas por aquí. ¡Por ahora!\n";
        return;
    }

    int indiceEnemigo = rand() % totalEnemigos;
    Enemigo enemigoActual = enemigos[indiceEnemigo];

    cout << "\n⚔️ Te adentras y... Aparece " << enemigoActual.nombre << " con mirada desafiante.\n";
    cout << "¡Hora de combatir!\n\n";
    cout << "Tú | " << enemigoActual.nombre << " 👾\n";
    cout << jugador.vida << " ❤️ | " << enemigoActual.vida << " 💢\n";

    while (enemigoActual.vida > 0 && jugador.vida > 0) {
        cout << "\n🪄 Lanzas un hechizo de ataque con esperanza de hacerle aunque sea algo de daño...\n";
        if (generarFloatAleatorio() < jugador.precision) {
            enemigoActual.vida -= jugador.ataque;
            cout << "💥 ¡Boom! Le diste " << jugador.ataque << " de daño.\n";
        } else {
            cout << "❌ Tu magia falló. El viento se burla de ti, estamos decepcionados.\n";
        }

        cout << "Tú | " << enemigoActual.nombre << " 👾\n";
        cout << jugador.vida << " ❤️ | " << enemigoActual.vida << " 💢\n";

        if (enemigoActual.vida <= 0) break;

        cout << "\n ->" << enemigoActual.nombre << " contraataca...\n";
        if (generarFloatAleatorio() < enemigoActual.precision) {
            jugador.vida -= enemigoActual.ataque;
            cout << "💔 Has recibido " << enemigoActual.ataque << " de daño de magia oscura.\n";
        } else {
            cout << "🌪️ ¡Siii, lo esquivaste!\n";
        }

        cout << "Tú | " << enemigoActual.nombre << " 👾\n";
        cout << jugador.vida << " ❤️ | " << enemigoActual.vida << " 💢\n";
    }

    if (jugador.vida <= 0) {
        cout << "\n🪦 Haz perdido... pero tu espíritu seguirá con nosotros.\n";
        exit(0);
    } else {
        cout << "\n🏆 Esoo, ¡Ganaste! Has derrotado a " << enemigoActual.nombre << ".\n";

        // Mostrar estadísticas antes de elegir una mejora
        cout << "\n -> Tus estadísticas actuales:\n";
        cout << "❤️  Vida: " << jugador.vida << endl;
        cout << "🎯  Precisión: " << jugador.precision << endl;
        cout << "⚔️  Ataque: " << jugador.ataque << endl; 

        cout << "\n-> Elige una mejora:\n";
        cout << "1. ✨ +3 Vida\n";
        cout << "2. 🎯 +0.2 Precisión\n";
        cout << "3. 🔥 +5 Ataque\n";

        int opcion;
        cin >> opcion;

        if (opcion == 1) jugador.vida += 3;
        else if (opcion == 2) jugador.precision += 0.2f;
        else if (opcion == 3) jugador.ataque += 5;
        else cout << "-> No entendí tu deseo... no se aplicó ninguna mejora.\n";

        // Mostrar estadísticas actualizadas
        cout << "\n🧚 Estado tras mejora:\n";
        cout << "❤️ Vida = " << jugador.vida << " | 🎯 Precisión = " << jugador.precision << " | ⚔️ Ataque = " << jugador.ataque << "\n";
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
        cout << "Error: La habitacion actual es nula. Fin del juego inesperado.\n";
        return;
    }

    cout << "\n>> " << actual->habitacion->nombre << "\n";
    cout << actual->habitacion->descripcion << "\n";

    if (actual->habitacion->tipo == "COMBATE") {
        combate(jugador);
    } else if (actual->habitacion->tipo == "EVENTO") {
        aplicarEvento(jugador);
    } else if (actual->habitacion->tipo == "FIN") {
        cout << "\n==> FIN DEL JUEGO \n";
        if (actual->habitacion->id == 11)
            cout << "Súper, ¡Haz ganado el juego!\n";
        else
            cout << "Has llegado a un final alternativo.\n";
        
        liberarArbol(actual);
        for (int i = 0; i < totalHabitaciones; ++i) {
            delete habitaciones[i];
            habitaciones[i] = nullptr;
        }
        exit(0);
    }

    if (jugador.vida <= 0) {
        cout << "Tu vida ha llegado a 0. Haz perdido. ¡Fin del juego!\n";
        liberarArbol(actual);
        for (int i = 0; i < totalHabitaciones; ++i) {
            delete habitaciones[i];
            habitaciones[i] = nullptr;
        }
        exit(0);
    }

    cout << "\nOpciones de destino:\n";
    int numeroOpciones = 0;
    if (actual->hijo1) { cout << "1. " << actual->hijo1->habitacion->nombre << endl; numeroOpciones++; }
    if (actual->hijo2) { cout << "2. " << actual->hijo2->habitacion->nombre << endl; numeroOpciones++; }
    if (actual->hijo3) { cout << "3. " << actual->hijo3->habitacion->nombre << endl; numeroOpciones++; }

    if (numeroOpciones == 0) {
        cout << "No hay mas caminos a seguir desde aqui. Fin del juego.\n";
        liberarArbol(actual);
        for (int i = 0; i < totalHabitaciones; ++i) {
            delete habitaciones[i];
            habitaciones[i] = nullptr;
        }
        return;
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
            liberarArbol(actual);
            for (int i = 0; i < totalHabitaciones; ++i) {
                delete habitaciones[i];
                habitaciones[i] = nullptr;
            }
            exit(0);
            break;
    }
}

int main() {
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

    string lineaTemporal;
    getline(archivo, lineaTemporal);
    getline(archivo, lineaTemporal);
    int cantidadHabitacionesArchivo = leerEnteroSeguro(archivo);

    if (cantidadHabitacionesArchivo > 100) {
        cerr << "Error: El numero de habitaciones en el archivo (" << cantidadHabitacionesArchivo << ") excede el limite del programa (100)." << endl;
        return 1;
    }
    totalHabitaciones = cantidadHabitacionesArchivo;

    for (int i = 0; i < totalHabitaciones; ++i) {
        string definicion = leerLineaSegura(archivo);
        string descripcion = leerLineaSegura(archivo);
        
        size_t posFinId = definicion.find(" ");
        int id = stoi(definicion.substr(0, posFinId));
        
        size_t posInicioNombre = posFinId + 1;
        size_t posInicioTipo = definicion.find("(", posInicioNombre);
        string nombre = definicion.substr(posInicioNombre, posInicioTipo - posInicioNombre -1);
        
        string tipo = definicion.substr(posInicioTipo + 1);
        tipo.pop_back();

        if (id >= 0 && id < 100) {
            habitaciones[id] = new Habitacion{id, nombre, tipo, descripcion};
        } else {
            cerr << "Error: ID de habitacion " << id << " fuera de rango o invalido.\n";
            for (int k = 0; k < 100; ++k) delete habitaciones[k];
            return 1;
        }
    }

    getline(archivo, lineaTemporal);
    int cantidadArcos = leerEnteroSeguro(archivo);
    for (int i = 0; i < cantidadArcos; ++i) {
        string arco = leerLineaSegura(archivo);
        size_t posFlecha = arco.find("->");
        int origen = stoi(arco.substr(0, posFlecha));
        int destino = stoi(arco.substr(posFlecha + 2));

        if (origen >= 0 && origen < 100) {
            bool agregado = false;
            for (int j = 0; j < 3; ++j) {
                if (conexiones[origen][j] == -1) {
                    conexiones[origen][j] = destino;
                    agregado = true;
                    break;
                }
            }
            if (!agregado) {
                cerr << "Advertencia: Demasiadas conexiones para la habitacion " << origen << ". Se ignoro la conexion a " << destino << endl;
            }
        } else {
            cerr << "Error: ID de origen de arco " << origen << " fuera de rango.\n";
        }
    }

    getline(archivo, lineaTemporal);
    int numEnemigosArchivo = leerEnteroSeguro(archivo);
    if (numEnemigosArchivo > 10) {
        cerr << "Error: El numero de enemigos en el archivo (" << numEnemigosArchivo << ") excede el limite del programa (10)." << endl;
        for (int k = 0; k < 100; ++k) delete habitaciones[k];
        return 1;
    }
    totalEnemigos = numEnemigosArchivo;

    for (int i = 0; i < totalEnemigos; ++i) {
        string lineaEnemigo = leerLineaSegura(archivo);
        
        size_t posFinNombre = lineaEnemigo.find(" | Vida ");
        if (posFinNombre == string::npos) { cerr << "Error de formato de enemigo (Nombre). Linea: '" << lineaEnemigo << "'" << endl; exit(1); }
        enemigos[i].nombre = lineaEnemigo.substr(0, posFinNombre);

        size_t posInicioVida = posFinNombre + 8;
        size_t posFinVida = lineaEnemigo.find(" | Ataque ", posInicioVida);
        if (posFinVida == string::npos) { cerr << "Error de formato de enemigo (Vida). Linea: '" << lineaEnemigo << "'" << endl; exit(1); }
        enemigos[i].vida = stoi(lineaEnemigo.substr(posInicioVida, posFinVida - posInicioVida));

        size_t posInicioAtaque = posFinVida + 10;
        size_t posFinAtaque = lineaEnemigo.find(" | Precision ", posInicioAtaque);
        if (posFinAtaque == string::npos) { cerr << "Error de formato de enemigo (Precision). Linea: '" << lineaEnemigo << "'" << endl; exit(1); }
        enemigos[i].ataque = stoi(lineaEnemigo.substr(posInicioAtaque, posFinAtaque - posInicioAtaque));

        size_t posInicioPrecision = posFinAtaque + 13;
        size_t posFinPrecision = lineaEnemigo.find(" | Probabilidad ", posInicioPrecision);
        if (posFinPrecision == string::npos) { cerr << "Error de formato de enemigo (Probabilidad). Linea: '" << lineaEnemigo << "'" << endl; exit(1); }
        enemigos[i].precision = stof(lineaEnemigo.substr(posInicioPrecision, posFinPrecision - posInicioPrecision));

        size_t posInicioProbabilidad = posFinPrecision + 16;
        enemigos[i].probabilidad = stof(lineaEnemigo.substr(posInicioProbabilidad));
    }

    getline(archivo, lineaTemporal);
    int numEventosArchivo = leerEnteroSeguro(archivo);
    if (numEventosArchivo > 10) {
        cerr << "Error: El numero de eventos en el archivo (" << numEventosArchivo << ") excede el limite del programa (10)." << endl;
        for (int k = 0; k < 100; ++k) delete habitaciones[k];
        return 1;
    }
    totalEventos = numEventosArchivo;

    for (int i = 0; i < totalEventos; ++i) {
        getline(archivo, lineaTemporal);
        eventos[i].nombre = leerLineaSegura(archivo);
        string lineaProbabilidad = leerLineaSegura(archivo);
        size_t posEspacio = lineaProbabilidad.find(" ");
if (posEspacio == string::npos) {
    cerr << "Error: Formato inválido en la línea de probabilidad del evento: '" << lineaProbabilidad << "'\n";
    exit(1);
}

string valorStr = lineaProbabilidad.substr(posEspacio + 1);
try {
    eventos[i].probabilidad = stof(valorStr);
} catch (const invalid_argument&) {
    cerr << "Error: No se pudo convertir a float el valor: '" << valorStr << "'\n";
    exit(1);
} catch (const out_of_range&) {
    cerr << "Error: Valor fuera de rango para float: '" << valorStr << "'\n";
    exit(1);
}

        eventos[i].descripcion = leerLineaSegura(archivo);

        eventos[i].opcionA_texto = leerLineaSegura(archivo);
        eventos[i].opcionA_consecuencia = leerLineaSegura(archivo);
        string efectoA = leerLineaSegura(archivo);
        parsearEfectos(efectoA, eventos[i].opcionA_vida, eventos[i].opcionA_precision);

        eventos[i].opcionB_texto = leerLineaSegura(archivo);
        eventos[i].opcionB_consecuencia = leerLineaSegura(archivo);
        string efectoB = leerLineaSegura(archivo);
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

    Jugador jugadorPrincipal;
    jugar(raiz, jugadorPrincipal);

    liberarArbol(raiz);
    for (int i = 0; i < 100; ++i) delete habitaciones[i];

    return 0;
}
