#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct Habitacion{
    int id;
    string nombre;
    string tipo;
    string descripcion;

    Habitacion* hijo1 = nullptr;
    Habitacion* hijo2 = nullptr;
    Habitacion* hijo3= nullptr;
};

int main(){
    ifstream archivo("data.map");
    if (!archivo.is_open()){
        cout<<"Error al abrir el archivo."<<endl;
        return 1;
    }

    string linea;
    getline(archivo, linea);

    int totalHabitaciones;
    archivo >> totalHabitaciones;
    archivo.ignore();
    Habitacion** habitaciones = new Habitacion[totalHabitaciones];

    for(int i=0; i<totalHabitaciones; i++){
        string linea;
        getline(archivo, linea);
        
        int id;
        string nombre, tipo;
        size_t pos1 = linea.find(" ");
        size_t pos2 = linea.find("(");
        size_t pos3 = linea.find(")");
        id = stoi(linea.substr(0, pos1));
        nombre = linea.substr(pos1 + 1, pos2 - pos1 -2);
        tipo = linea.substr(pos2 + 1, pos3 - pos2 - 1);
        
        string descripcion;
        getline(archivo, descripcion);
        
        Habitacion* h = new Habitacion;
        h->id = id;
        h->nombre = nombre;
        h->tipo = tipo;
        h->descripcion = descripcion;
        habitaciones[id]= h;
    }

    getline(archivo, linea);
    int cantidadArcos;
    archivo >> cantidadArcos;
    archivo.ignore();
    
    for(int i=0; i<cantidadArcos; i++){
        string linea;
        getline(archivo, linea);
        size_t pos = linea.find("->");
        int origen = stoi(linea.substr(0, pos));
        int destino = stoi(linea.substr(pos + 2));
        
        Habitacion* padre = habitaciones[origen];
        Habitacion* hijo = habitaciones[destino];
        
        if (padre->hijo1==nullptr){
            padre->hijo1 = hijo;
        }else if(padre->hijo2==nullptr){
            padre->hijo2 = hijo;
        }else if(padre->hijo3==nullptr){
            padre->hijo3 = hijo;
        }else{
            cout<<"Error: La habitacion"<< origen <<"ya tiene 3 hijos."<<endl;
        }
    }

    archivo.close();
    return 0;
}

