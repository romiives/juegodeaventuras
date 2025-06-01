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
             << "\nPrecisión: " << precision << "\nRecuperación: " << recuperacion << endl;
    }
};

//Struct y uso de data.map

struct Enemigo{
    string nombre;
    int vida;
    int ataque;
    float precision;
    float probabilidad;
};

struct NodoEnemigo{
    Enemigo enemigo;
    NodoEnemigo* sig;
};

struct ColaEnemigos{
    NodoEnemigo* frente = nullptr;
    NodoEnemigo* final = nullptr;
    void encolar(Enemigo e){
        NodoEnemigo* nuevo = new NodoEnemigo{e, nullptr};
        if (!frente) frente = final = nuevo;
        else{
            final->sig = nuevo;
            final = nuevo;
        }
    }
    void desencolar(){
        if (frente){
            NodoEnemigo* temp = frente;
            frente = frente->sig;
            delete temp;
        }
    }
    bool estaVacia(){
        return frente == nullptr;
    }
};

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

struct Evento{
    string nombre;
    float probabilidad;
    string descripcion;
    string opcionA;
    string efectoA;
    string opcionB;
    string efectoB;
};
const int MAX_EVENTOS = 10;
Evento eventos[MAX_EVENTOS];
int totalEventos = 0;

const int MAX_ENEMIGOS = 10;
Enemigo enemigos[MAX_ENEMIGOS];
int totalEnemigos = 0;

const int MAX_MEJORAS = 10;
string mejorasCombate[MAX_MEJORAS];
int totalMejoras = 0;

const int MAX_HABITACIONES = 100;
Habitacion* habitaciones[MAX_HABITACIONES];
bool conexiones[MAX_HABITACIONES][3];
int hijos[MAX_HABITACIONES][3];
int contadorHijos[MAX_HABITACIONES];

string obtenerTipoDesdeNombre(const string& nombreOriginal) {
    string nombre = nombreOriginal;
    nombre.erase(remove_if(nombre.begin(), nombre.end(), ::isspace), nombre.end());
    if (nombre.find("(INICIO)") != string::npos) return "INICIO";
    if (nombre.find("(COMBATE)") != string::npos) return "COMBATE";
    if (nombre.find("(EVENTO)") != string::npos) return "EVENTO";
    if (nombre.find("(FIN)") != string::npos) return "FIN";
    return "DESCONOCIDO";
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
                ss >> id;
                string nombreParte;
                getline(ss, nombreParte);
                nombreParte.erase(0, nombreParte.find_first_not_of(" \t\r\n"));
                nombreParte.erase(nombreParte.find_last_not_of(" \t\r\n")+1);
                string descripcion;
                getline(archivo, descripcion);
                string tipo = obtenerTipoDesdeNombre(nombreParte);
                habitaciones[id] = new Habitacion(id, nombreParte, descripcion, tipo);

                cout <<"[DEBUG] ID: " << id << ", Nombre: " << nombreParte << ", Tipo detectado:" << tipo << endl;
            }
        } else if (linea == "ARCOS") {
            int cantidad;
            archivo >> cantidad;
            archivo.ignore();
            for (int i = 0; i < MAX_HABITACIONES; i++) contadorHijos[i] = 0;
            for (int i = 0; i < cantidad; i++) {
                getline(archivo, linea);
                //int desde, hacia;
                size_t arrowPos = linea.find("->");
                if (arrowPos != string::npos){
                    int desde = stoi(linea.substr(0, arrowPos));
                    int hacia = stoi(linea.substr(arrowPos + 2));
                    if (contadorHijos[desde] < 3) {
                        hijos[desde][contadorHijos[desde]] = hacia;
                        contadorHijos[desde]++;
                    }
                }
            
            }
        } else if (linea=="EVENTOS"){
            archivo>>totalEventos;
            archivo.ignore();
            for (int i=0; i<totalEventos; i++){
                getline(archivo, linea);
                getline(archivo, eventos[i].nombre);
                getline(archivo, linea);
                eventos[i].probabilidad = stof(linea.substr(linea.find(" ")+1));
                getline(archivo, eventos[i].descripcion);
                getline(archivo, linea);
                eventos[i].opcionA = linea.substr(3);
                getline(archivo, eventos[i].efectoA);
                getline(archivo, linea);
                eventos[i].opcionB = linea.substr(3);
                getline(archivo, eventos[i].efectoB);
            }
        } else if (linea == "ENEMIGOS") {
            archivo>>totalEnemigos;
            archivo.ignore();
            for (int i = 0; i < totalEnemigos; i++) {
                getline(archivo, linea);
                stringstream ss(linea);
                string nombre, temp;
                int vida, ataque;
                float precision, probabilidad;
                getline(ss, nombre, '|');
                ss>>temp>>vida>>temp>>ataque>>temp>>precision>>temp>>probabilidad;
                nombre.erase(0, nombre.find_first_not_of(" "));
                nombre.erase(nombre.find_last_not_of(" ") + 1);
                enemigos[i]={nombre, vida, ataque, precision, probabilidad};
            }
        } else if (linea == "MEJORAS DE COMBATE"){
            while (getline(archivo, linea)) {
                if (linea == "FIN DE ARCHIVO") break;
                if (linea.empty()) continue;
                mejorasCombate[totalMejoras++]=linea;
            }
        }


    }
    archivo.close();

    for (int i = 0; i < MAX_HABITACIONES; i++) {
        if (habitaciones[i] != nullptr) {
            for (int j = 0; j < contadorHijos[i]; j++) {
                if (j == 0) habitaciones[i]->hijo1 = habitaciones[hijos[i][j]];
                if (j == 1) habitaciones[i]->hijo2 = habitaciones[hijos[i][j]];
                if (j == 2) habitaciones[i]->hijo3 = habitaciones[hijos[i][j]];
            }
            cout << "[DEBUG] Habitacion ID " << i << " tiene hijos: ";
            if (habitaciones[i]->hijo1) cout << habitaciones[i]->hijo1->id << " ";
            if (habitaciones[i]->hijo2) cout << habitaciones[i]->hijo2->id << " ";
            if (habitaciones[i]->hijo3) cout << habitaciones[i]->hijo3->id << " ";
            cout << endl;
        }
    }

    return true;
}

ColaEnemigos generarColaDeEnemigos(){
    ColaEnemigos cola;
    for (int i = 0; i < totalEnemigos; i++){
        float r = static_cast<float>(rand()) / RAND_MAX;
        if (r <= enemigos[i].probabilidad) {
            cola.encolar(enemigos[i]);
        }
    }
    if (cola.estaVacia()){
        cola.encolar(enemigos[0]);
    }
    return cola;
}

bool combatir(Jugador& jugador){
    ColaEnemigos enemigosCola = generarColaDeEnemigos();
    cout << "\n=== COMBATE INICIADO ===" <<endl;
    while (!enemigosCola.estaVacia() && jugador.estaVivo()){
        Enemigo& enemigo = enemigosCola.frente->enemigo;
        float r = static_cast<float>(rand()) / RAND_MAX;
        if (r <= jugador.getPrecision()){
            cout<<"Atacas a "<<enemigo.nombre<<" y le haces "<<jugador.getAtaque()<<" de daño."<<endl;
            enemigo.vida -= jugador.getAtaque();
        } else {
            cout<<"Fallaste el ataque a "<<enemigo.nombre<<"!"<<endl;
        }
        if (enemigo.vida <= 0){
            cout<<enemigo.nombre<<" ha sido derrotado!"<<endl;
            enemigosCola.desencolar();
            continue;
        }
        r = static_cast<float>(rand()) / RAND_MAX;
        if (r <= enemigo.precision){
            cout<< enemigo.nombre<< " te ataca y te hace "<< enemigo.ataque<< " de daño."<<endl;
            jugador.modificarVida(-enemigo.ataque);
        } else {
            cout<< enemigo.nombre<< " falló su ataque."<<endl;
        }
        jugador.mostrarEstado();
    }
    if (jugador.estaVivo()){
        cout<< "\n¡Ganaste el combate!"<<endl;
        return true;
    } else {
        cout<< "\nHas sido derrotado..."<<endl;
        return false;
    }
}

void aplicarMejora(Jugador& jugador, const string& mejora){
    if (mejora.find("Vida") != string::npos){
        int cantidad=stoi(mejora);
        jugador.modificarVida(cantidad);
    } else if (mejora.find("Precision") != string::npos){
        float cantidad=stof(mejora);
        jugador.mejorarPrecision(cantidad);
    } else if (mejora.find("Ataque") != string::npos){
        int cantidad=stoi(mejora);
        jugador.mejorarAtaque(cantidad);
    } else if (mejora.find("Recuperacion") != string::npos){
        int cantidad=stoi(mejora);
        jugador.mejorarRecuperacion(cantidad);
    }
}

void aplicarEfecto(Jugador& jugador, const string& efecto){
    stringstream ss(efecto);
    string parte;
    while (getline(ss, parte, ',')){
        parte.erase(0, parte.find_first_not_of(" "));
        parte.erase(parte.find_last_not_of(" ") + 1);
        if (parte.find("Vida") != string::npos){
            int cantidad=stoi(parte);
            jugador.modificarVida(cantidad);
        } else if (parte.find("Precision") != string::npos){
            float cantidad=stof(parte);
            jugador.mejorarPrecision(cantidad);
        } else if (parte.find("Ataque") != string::npos){
            int cantidad=stoi(parte);
            jugador.mejorarAtaque(cantidad);
        } else if (parte.find("Recuperacion") != string::npos){
            int cantidad=stoi(parte);
            jugador.mejorarRecuperacion(cantidad);
        }
    }
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

        if (actual->tipo == "EVENTO"){
            float r = static_cast<float>(rand()) / RAND_MAX;
            float acumulado = 0;
            Evento elegido;
            for (int i = 0; i < totalEventos; i++){
                acumulado += eventos[i].probabilidad;
                if (r <= acumulado){
                    elegido = eventos[i];
                    break;
                }
            }
            cout << "\n=== EVENTO: " << elegido.nombre << " ===" << endl;
            cout << elegido.descripcion << endl;
            cout << "A: " << elegido.opcionA << "\nB: " << elegido.opcionB << endl;
            char eleccion;
            cout << "Elige A o B: ";
            cin >> eleccion;
            if (eleccion == 'A' || eleccion == 'a') aplicarEfecto(jugador, elegido.efectoA);
            else if (eleccion == 'B' || eleccion == 'b') aplicarEfecto(jugador, elegido.efectoB);
            else cout << "Opcion invalida. No se aplica efecto." << endl;
            
            jugador.modificarVida(jugador.getRecuperacion());
            cout << "Recuperas " << jugador.getRecuperacion() << " de vida tras el evento.\n";
        }

        if (actual->tipo == "COMBATE"){
            bool gano = combatir(jugador);
            if (!gano) break;
            jugador.modificarVida(jugador.getRecuperacion());
            cout << "Recuperas " << jugador.getRecuperacion() << " de vida tras el combate.\n";

            cout << "\n--- Elige una mejora de combate ---" << endl;
            for (int i=0; i < totalMejoras; i++){
                cout << i+1<< ". " << mejorasCombate[i] << endl;
            }
            int opcion = 0;
            while (opcion < 1 || opcion > totalMejoras){
                cout << "Opcion (1-" << totalMejoras << "): ";
                cin >> opcion;
            }
            aplicarMejora(jugador, mejorasCombate[opcion -1]);
        }
        if (actual->hijo1 || actual->hijo2 || actual->hijo3){
            cout << "\n¿A dónde quieres ir?" << endl;
            if (actual->hijo1) cout << "1. " << actual->hijo1->nombre << endl;
            if (actual->hijo2) cout << "2. " << actual->hijo2->nombre << endl;
            if (actual->hijo3) cout << "3. " << actual->hijo3->nombre << endl;

            int opcion = 0;
            bool valido = false;
            while (!valido){
                cout << "Opcion: ";
                cin >> opcion;
                if (opcion == 1 && actual->hijo1) { actual = actual->hijo1; valido = true; }
                else if (opcion == 2 && actual->hijo2) { actual = actual->hijo2; valido = true; }
                else if (opcion == 3 && actual->hijo3) { actual = actual->hijo3; valido = true; }
                else cout << "Opción inválida. Intenta de nuevo." << endl;
            }
        } else {
            cout << "\nNo hay más caminos disponibles en esta habitación."<< endl;
            break;
        }
    }
}

//liberar memoria
void liberarMemoria() {
    for (int i = 0;i<MAX_HABITACIONES;i++){
        if (habitaciones[i] != nullptr){
            delete habitaciones[i];
            habitaciones[i] = nullptr;
        }
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
        for (int i=0; i < MAX_HABITACIONES; i++){
            if (habitaciones[i]) {
                cout << "  - ID: " << i << ": tipo = " << habitaciones[i]->tipo << endl;
            }
        }
        return 1;
    }

    jugar(inicio, jugador);

    cout << "\nGracias por jugar. Hasta la próxima!" << endl;

    liberarMemoria();
    return 0;
}

