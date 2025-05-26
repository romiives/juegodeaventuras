#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

using namespace std;

//TDA Jugador 

class Jugador {
private:
    int vida;
    int ataque;
    float precision;
    int recuperacion;

public:
    Jugador(int v = 30, int a = 5, float p = 0.75f, int r = 3) {
        vida = v;
        ataque = a;
        precision = p;
        recuperacion = r;
    }

    int getVida() const { return vida; }
    int getAtaque() const { return ataque; }
    float getPrecision() const { return precision; }
    int getRecuperacion() const { return recuperacion; }

    void modificarVida(int delta) { vida += delta; if (vida < 0) vida = 0; }
    void mejorarAtaque(int delta = 1) { ataque += delta; }
    void mejorarPrecision(float delta = 0.1f) { precision += delta; }
    void mejorarRecuperacion(int delta = 1) { recuperacion += delta; }

    bool estaVivo() const { return vida > 0; }

    void mostrarEstado() const {
        cout << "Jugador ---> \nVida: " << vida << "\nAtaque: " << ataque
             << "\nPrecisiónROMINAHOLA: " << precision << "\nRecuperación: " << recuperacion << endl;
    }
};

//Struct y uso de data.map

struct Habitacion {
    int id;
    string nombre;
    string descripcion;
    string tipo;
    Habitacion* hijo1;
    Habitacion* hijo2;
    Habitacion* hijo3;

    Habitacion(int _id, string _nombre, string _descripcion, string _tipo) {
        id = _id;
        nombre = _nombre;
        descripcion = _descripcion;
        tipo = _tipo;
        hijo1 = hijo2 = hijo3 = NULL;
    }
};

const int MAX_HABITACIONES = 100;
Habitacion* habitaciones[MAX_HABITACIONES];
bool conexiones[MAX_HABITACIONES][3];
int hijos[MAX_HABITACIONES][3];
int contadorHijos[MAX_HABITACIONES];

string obtenerTipoDesdeNombre(const string& nombre) {
    if (nombre.find("(INICIO)") != string::npos) return "INICIO";
    if (nombre.find("(COMBATE)") != string::npos) return "COMBATE";
    if (nombre.find("(EVENTO)") != string::npos) return "EVENTO";
    return "FIN";
}

bool cargarArchivo(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo.c_str());
    if (!archivo.is_open()) {
        cout << "Error: no se pudo abrir el archivo '" << nombreArchivo << "'." << endl;
        return false;
    }

    string linea;
    while (getline(archivo, linea)) {
        if (linea == "HABITACIONES") {
            int cantidad;
            archivo >> cantidad;
            archivo.ignore();
            for (int i = 0; i < cantidad; i++) {
                getline(archivo, linea);
                stringstream ss(linea);
                int id;
                string nombreParte;
                ss >> id;
                getline(ss, nombreParte);
                getline(archivo, linea);
                string tipo = obtenerTipoDesdeNombre(nombreParte);
                habitaciones[id] = new Habitacion(id, nombreParte, linea, tipo);
            }
        } else if (linea == "ARCOS") {
            int cantidad;
            archivo >> cantidad;
            archivo.ignore();
            for (int i = 0; i < MAX_HABITACIONES; i++) contadorHijos[i] = 0;
            for (int i = 0; i < cantidad; i++) {
                getline(archivo, linea);
                int desde, hacia;
                sscanf(linea.c_str(), "%d -> %d", &desde, &hacia);
                if (contadorHijos[desde] < 3) {
                    hijos[desde][contadorHijos[desde]] = hacia;
                    contadorHijos[desde]++;
                }
            }
        }
    }
    archivo.close();

    for (int i = 0; i < MAX_HABITACIONES; i++) {
        if (habitaciones[i] != NULL) {
            for (int j = 0; j < contadorHijos[i]; j++) {
                if (j == 0) habitaciones[i]->hijo1 = habitaciones[hijos[i][j]];
                if (j == 1) habitaciones[i]->hijo2 = habitaciones[hijos[i][j]];
                if (j == 2) habitaciones[i]->hijo3 = habitaciones[hijos[i][j]];
            }
        }
    }

    return true;
}
//cambiar nombre de habitación
void jugar(Habitacion* actual, Jugador& jugador) {
    while (actual != NULL && jugador.estaVivo()) {
        cout << "\n " << actual->nombre << "" << endl;
        cout << actual->descripcion << endl;

        if (actual->tipo == "FIN") {
            cout << "\n=== Has llegado a un final del juego. ===" << endl;
            break;
        }

        jugador.mostrarEstado();

        if (actual->hijo1 == NULL && actual->hijo2 == NULL && actual->hijo3 == NULL) break;
 
        cout << "\nA dónde quieres ir?" << endl;
        if (actual->hijo1) cout << "1. " << actual->hijo1->nombre << endl;
        if (actual->hijo2) cout << "2. " << actual->hijo2->nombre << endl;
        if (actual->hijo3) cout << "3. " << actual->hijo3->nombre << endl;

        int opcion;
        cin >> opcion;

        if (opcion == 1 && actual->hijo1) actual = actual->hijo1;
        else if (opcion == 2 && actual->hijo2) actual = actual->hijo2;
        else if (opcion == 3 && actual->hijo3) actual = actual->hijo3;
        else cout << "Opcion invalida, intenta de nuevo." << endl;
    }
}

//Main
 
int main() {
    cout << "¡Bienvenido al juego!" << endl;

    string nombreArchivo = "data.map";
    cout << "Cargando archivo '" << nombreArchivo << "'..." << endl;

    if (!cargarArchivo(nombreArchivo)) {
        cout << "No se pudo cargar el mapa. Cerrando juego." << endl;
        return 1;
    }

    Jugador jugador;
    jugador.mostrarEstado();

    Habitacion* inicio = NULL;
    for (int i = 0; i < MAX_HABITACIONES; i++) {
        if (habitaciones[i] != NULL && habitaciones[i]->tipo == "INICIO") {
            inicio = habitaciones[i];
            break;
        }
    }

    if (inicio == NULL) {
        cout << "No se encontró una habitación." << endl;
        return 1;
    }

    jugar(inicio, jugador);

    cout << "\nGracias por jugar. Hasta la próxima!" << endl;
    return 0;
}
