#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
using namespace std;

struct Player{
    string name;
    string pos;
    int price;
    string team;
    int points;
};

using vp = vector <Player>;
using vi = vector <int>;

string OUTPUT_FILE;
int n1;
int n2;
int n3;

void write_players_pos()

void write_result(const vp& alineacio){
    auto end = chrono::high_resolution_clock::now();
    auto time = chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    ofstream out(OUTPUT_FILE);
    //porter, defs, mig, dav,
    int  points, price;
    out << time << endl;
    out << "POR: ";
    write_players_pos() porter << endl; 
    out << "DEF: " << defs << endl;
    out << "MIG: " << mig << endl;
    out << "DAV: " << dav << endl;
    out << "Punts: " << points << endl;
    out << "Preu: " << price << endl;
    out.close();
}

/*
PODES:
    - No rebutjar si N_i - index  0
    - No agafar cap jugador que superi els J euros
    - No agafar cap jugador que superi els T euros
    - Precalcular el màxim de punts per posició amb una pque
    - Precalcular el mínim d'euros per posició (o els n_i de menys euros) amb una pque
    - Comptar el total de cada posició i els count de visitats
*/

void generar_alineacio(int i, int k, vp& alineacio, const vp& jugadors, vi n, int t, const int& j){
    /*
        i: index jugador de les dades
        k: index alineaco
        alineacio: jugadors escollits
        jugadors: dades de tots els jugadors
        n: vector amb el nombre de jugadors per cada posicio
        t: total d'euros
        j: maxim d'euros per jugador
    */
    Player jugador = jugadors[i];
    if (n[0] == 0 and n[1] == 0 and n[2] == 0 and n[3] == 0){
        write_result(alineacio);
    }
    else{
        int idx;
        if (jugador.pos == "por") idx = 0;
        else if (jugador.pos == "def") idx = 1;
        else if (jugador.pos == "mig") idx = 2;
        else idx = 3;

        if (n[idx] > 0 and jugador.price <= j and t - jugador.price >= 0){
            n[idx]--;
            alineacio[k] = jugador;
            generar_alineacio(i+1, k+1, alineacio, jugadors, n, t - jugador.price, j);
            n[idx]++;
        }
        else{
            generar_alineacio(i+1, k, alineacio, jugadors, n, t, j);
        }
    }
}


vector <vp> get_players(char** argv){
    ifstream data(argv[1]);
    vp v_players;
    string line;

    while (not data.eof()) {
        string name, club, position;
        int p;
        getline(data,name,';');    if (name == "") break;
        getline(data,position,';');
        int price; data >> price;
        char aux; data >> aux;
        getline(data,club,';');
        data >> p;
        string aux2;
        getline(data,aux2);
        v_players.push_back(Player(name,position,price,club,p))
    
    }
    return v_player;

}

int main(int argc, char** argv){
    // llegim "n1 n2 n3 t j" del fitxer de consulta
    // format BD "Nom;Posicio;Preu;Equip;Punts"
    //argv[0]: nom arxieu BD
    //argv[1]: nom arxiu consulta
    
    ifstream query(argv[2]);

    int n1, n2, n3, t, j;
    query >> n1 >> n2 >> n3 >> t >> j;

    OUTPUT_FILE = argv[3];
    vp alineacio(1 + n1 + n2 + n3 + 1);
    vector<int> n = {1, n1, n2, n3};
    

    auto start = std::chrono::high_resolution_clock::now();
    generar_alineacio(0, 0, alineacio, jugadors, n, t, j);
    
    data.close();
    query.close();
    
    /*
    output:
    - double segons que triga amb 1 decimal
    - "POR: " + nom del porter
    - "DEF: " + defenses separats amb ";"
    - "MIG: " + migca
    - "DAV: "
    - "Punts: " punts equip
    - "Preu: " preu equip
    */
}