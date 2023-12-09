#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <cstring>

using namespace std;

struct Player{
    string name;
    string pos;
    long long int price;
    string team;
    long long int points;
};


using vp = vector <Player>;
using vi = vector <int>;
using vli = vector <long long int>;


string OUTPUT_FILE;
chrono::high_resolution_clock::time_point start;
int N1, N2, N3;
vp PLAYERS;
long long int T;


unordered_map <string, vector<Player>> get_players_pos(const vp& alineacio){
    unordered_map <string, vector<Player>> players_pos = {{"por", {}}, {"def", {}}, {"mig", {}}, {"dav", {}}};
    for (Player p: alineacio){
        players_pos[p.pos].push_back(p);
    }
    return players_pos;    
}


void write_result(const vp& alineacio, const long long int& points, const long long int& price){
    
    ofstream out(OUTPUT_FILE);

    auto end = chrono::high_resolution_clock::now();
    auto time = chrono::duration_cast<std::chrono::seconds>(end - start);
    out << time.count() << endl;

    unordered_map <string, vector<Player>> players_pos = get_players_pos(alineacio);   
    vector <string> positions = {"por", "def", "mig", "dav"};

    for (string pos: positions){
        for (char c: pos) out << toupper(c); 
        out << ": ";
        for (int i = 0; i < int(players_pos[pos].size()); i++){
            out << (i == 0 ? "" : ";") << players_pos[pos][i].name;
        }
        out << endl;
    }
    
    out << "Punts: " << points << endl;
    out << "Preu: " << price << endl;

    out.close();
}

/*
PODES:
    - No rebutjar si N_i - index  0 --> he de precalcular quants jugadors hi ha de cada posicio
    - Precalcular el mínim d'euros per posició (o els n_i de menys euros) amb una pque
    - Comptar el total de cada posició i els count de visitats

    - Faig una array mida 450. A cada posició hi haurà 4 vectors.
*/
void check_alineacio(const vp& alineacio){
    for (Player p: alineacio) cout << p.name <<" "<< p.price <<" " << p.points << endl;
}

bool promising(int i, unordered_map <string, int> n){
    return true;
}

void generar_alineacio(int i, vp& alineacio, unordered_map <string, int> n, long long int total,
                        long long int points, long long int& max_points,  
                        unordered_map<string, long long int>& m_min_price){
    /*
        i: index jugador de les dades
        alineacio: jugadors escollits
        n: map amb el nombre de jugadors per cada posicio
        total: suma dels preus de tots els jugadors de l'alineació que portem fins ara
        points: suma dels punts de tots els jugadors de l'alineació que portem fins ara
        max_points: maxim de punts que hem aconseguit en totes les alineacions generades
        m_min_price:

        create map unvisited
    */
    //check_alineacio(alineacio);
    if (11 == int(alineacio.size()) and n["por"] == 0 and n["def"] == 0 and n["mig"] == 0 and n["dav"] == 0){ // totes les posicions assignades
        //
        if (points > max_points){
            max_points = points;
            write_result(alineacio, points, total);
        }
    }

    else if (i + n["por"] + n["def"] + n["mig"] + n["dav"] - 1 < int(PLAYERS.size())){
        
        Player player = PLAYERS[i];
        
        long long int points_estimats = points + player.points*(n["por"] + n["def"] + n["mig"] + n["dav"]);
        
    
        if (n[player.pos] > 0 and total + player.price <= T and points_estimats >= max_points){
            n[player.pos]--;
            alineacio.push_back(player);
            generar_alineacio(i+1, alineacio, n, total + player.price, points + player.points, max_points, m_min_price);
            n[player.pos]++;
            alineacio.pop_back();
        }
        
        generar_alineacio(i+1, alineacio, n, total, points, max_points, m_min_price);
    }
}


vp get_players_from_data(string data_file, unordered_map <string, long long int>& min_price, 
                        unordered_map <string, int>& unvisited,long long int J){
    ifstream data(data_file);
    vp players;
    string line;

    while (not data.eof()) {
        string name, club, position, aux2;
        long long int price; long long int p;
        char aux;
        getline(data,name,';');    if (name == "") break;
        getline(data,position,';');
        data >> price >> aux;
        getline(data,club,';');
        data >> p;
        getline(data, aux2);
        if (price <= J){
            Player player = {name, position, price, club, p};
            players.push_back(player);
            min_price[player.pos] = min(player.price, min_price[player.pos]);
            unvisited[position]++;
        }
    }
    data.close();
    return players;

}

bool comp(const Player& a, const Player& b) {
    if(a.points == b.points) return a.price > b.price;
    return a.points > b.points;
}

int main(int argc, char** argv){
    // llegim "n1 n2 n3 t j" del fitxer de consulta
    // format BD "Nom;Posicio;Preu;Equip;Punts"
    //argv[0]: nom arxiu BD
    //argv[1]: nom arxiu consulta
    //argv[2]: nom arxiu output

    if (argc != 4) {
        cout << "Syntax: " << argv[0] << " data_base.txt query.txt output.txt" << endl;
        exit(1);
    }
    
    unordered_map <string, long long int> m_min_price = {{"por", 999999999}, {"def", 999999999}, {"mig", 999999999}, {"dav", 999999999}};
    
    ifstream query(argv[2]);
    long long int J;

    query >> N1 >> N2 >> N3 >> T >> J;
    
    unordered_map <string, int> n = {{"por", 1}, {"def", N1}, {"mig", N2}, {"dav", N3}};
    unordered_map <string, int> unvisited = {{"por", 0}, {"def", 0}, {"mig", 0}, {"dav", 0}};

    start = chrono::high_resolution_clock::now();
    
    PLAYERS = get_players_from_data(argv[1], m_min_price, unvisited, J);
    sort(PLAYERS.begin(), PLAYERS.end(), comp);
    OUTPUT_FILE = argv[3];
    vp alineacio;
    
    long long int max_points = 0;
    
    generar_alineacio(0, alineacio, n, 0, 0, max_points, m_min_price);
    
    query.close();
}