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
chrono::high_resolution_clock::time_point start;
int n1, n2, n3;

vector <vp> get_players_pos(const vp& alineacio, vi n){
    vector <vp> players_pos(4, vp(0));
    int idx = 0;

    for (int i = 0; i < 11; i++){
        n[idx]--;
        players_pos[idx].push_back(alineacio[i]);
        if (n[idx] == 0) idx++;
    }
    return players_pos;    
}

void write_result(const vp& alineacio){
    
    ofstream out(OUTPUT_FILE);

    auto end = chrono::high_resolution_clock::now();
    auto time = chrono::duration_cast<std::chrono::milliseconds>(end - start);
    out << time.count() << endl;

    //players per position
    vi n = {1, n1, n2, n3};
    vector <vp> players_pos = get_players_pos(alineacio, n);   
    vector <string> pos = {"POR: ", "DEF: ", "MIG: ", "DAV: "};
    //cout << players_pos[1][0].name;
    for (int i = 0; i < 4; i++){ // i posicion
        out << pos[i];
        for (int j = 0; j < n[i]; j++){ // j-essimo jugador de la posicion
            if (j > 0) out << ";";
            out << players_pos[i][j].name;
        }
        out << endl;
    }
    
    int points = 0, price = 0;
    for (Player p: alineacio){
        points += p.points;
        price += p.price;
    } 
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

void generar_alineacio(int i, int k, vp& alineacio, const vp& jugadors, vi n, int t, const int& j, int points, int& max_points){
    /*
        i: index jugador de les dades
        k: index alineacio
        alineacio: jugadors escollits
        jugadors: dades de tots els jugadors
        n: vector amb el nombre de jugadors per cada posicio
        t: total d'euros
        j: maxim d'euros per jugador
        points: punts fins el kèssim jugador de l'alineació
    */
    Player jugador = jugadors[i];
    if (n[0] == 0 and n[1] == 0 and n[2] == 0 and n[3] == 0){ // totes les posicions assignades
        if (points > max_points){
            max_points = points;
            write_result(alineacio);
        }
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
            generar_alineacio(i+1, k+1, alineacio, jugadors, n, t - jugador.price, j, points + jugador.points, max_points);
            n[idx]++;
        }
        else{
            generar_alineacio(i+1, k, alineacio, jugadors, n, t, j, points, max_points);
        }
    }
}


vp get_players_from_data(string data_file){
    ifstream data(data_file);
    vp players;
    string line;

    while (not data.eof()) {
        string name, club, position, aux2;
        int price, p;
        char aux;
        getline(data,name,';');    if (name == "") break;
        getline(data,position,';');
        data >> price;
        data >> aux;
        getline(data,club,';');
        data >> p;
        getline(data, aux2);
        Player player = {name, position, price, club, p};
        players.push_back(player);
    }
    data.close();
    return players;

}

int main(int argc, char** argv){
    // llegim "n1 n2 n3 t j" del fitxer de consulta
    // format BD "Nom;Posicio;Preu;Equip;Punts"
    //argv[0]: nom arxiu BD
    //argv[1]: nom arxiu consulta
    //argv[2]: nom arxiu output
    
    ifstream query(argv[2]);

    vp players = get_players_from_data(argv[1]);

    int t, j;
    query >> n1 >> n2 >> n3 >> t >> j;

    OUTPUT_FILE = argv[3];
    vp alineacio(11);
    vector<int> n = {1, n1, n2, n3};
    int max_points = 0;

    start = chrono::high_resolution_clock::now();
    generar_alineacio(0, 0, alineacio, players, n, t, j, 0, max_points);
    
    query.close();
}