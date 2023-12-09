#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <iomanip>

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

    unordered_map <string, vector<Player>> players_pos = get_players_pos(alineacio);   
    vector <string> positions = {"por", "def", "mig", "dav"};

    chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    double time = duration.count();
    out << fixed << setprecision(1) << time << endl;

    for (string pos: positions){
        string pos_upper = pos;
        transform(pos_upper.begin(), pos_upper.end(), pos_upper.begin(), ::toupper);
        out << pos_upper << ": "; 
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
    - No rebutjar si N_i --> he de precalcular quants jugadors hi ha de cada posicio
    - Precalcular el mínim d'euros per posició (o els n_i de menys euros) amb una pque
*/
void check_alineacio(const vp& alineacio){
    for (Player p: alineacio) cout << p.name <<" "<< p.price <<" " << p.points << endl;
}


bool candidate(int i, unordered_map <string, int> n, const long long int & points, const long long int& total,
                const long long int& max_points){
    /*returns whether or not exists a promising solution picking the i-th player*/
    Player player = PLAYERS[i];
    if (n[player.pos] == 0) return false;
    if (total + player.price > T) return false;

    int k = n["por"] + n["def"] + n["mig"] + n["dav"];
    if (i + k >= int(PLAYERS.size())) return false;

    //if with the current player and the next k-1 players we dont reach max_points, we discard the candidate
    long long int potential_points = points;
    for (int j = i; j < i+k; j++){
        potential_points += PLAYERS[j].points;
    }
    if (potential_points <= max_points) return false;

    //afegir condició relacionada amb el preu
    return true; 
}


void generar_alineacio(int i, vp& alineacio, unordered_map <string, int> n, long long int total,
                        long long int points, long long int& max_points,  
                        unordered_map<string, long long int>& m_min_price,
                        unordered_map <string, int> unvisited){
    /*
        i: index jugador de les dades
        alineacio: jugadors escollits
        n: map amb el nombre de jugadors per cada posicio
        total: suma dels preus de tots els jugadors de l'alineació que portem fins ara
        points: suma dels punts de tots els jugadors de l'alineació que portem fins ara
        max_points: maxim de punts que hem aconseguit en totes les alineacions generades
        m_min_price:
        unvisited: 

    */
    
    Player player = PLAYERS[i];

    if (11 == int(alineacio.size()) and n["por"] == 0 and n["def"] == 0 and n["mig"] == 0 and n["dav"] == 0){ // totes les posicions assignades
        
        if (points > max_points){
            max_points = points;
            write_result(alineacio, points, total);
        }
    }

    else if (i < int(PLAYERS.size()) and n[player.pos] <= unvisited[player.pos]){
        
        unvisited[player.pos]--;
            
        if (candidate(i, n, points, total, max_points)){
            n[player.pos]--;
            alineacio.push_back(player);
            generar_alineacio(i+1, alineacio, n, total + player.price, points + player.points, max_points, m_min_price, unvisited);
            n[player.pos]++;
            alineacio.pop_back();
        }
        generar_alineacio(i+1, alineacio, n, total, points, max_points, m_min_price, unvisited);
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
            min_price[position] = min(price, min_price[position]);
            unvisited[position]++;
        }
    }
    data.close();
    return players;

}

bool comp(const Player& a, const Player& b) {
    /*auxiliar function to sort the players depending on their points*/
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
    
    generar_alineacio(0, alineacio, n, 0, 0, max_points, m_min_price, unvisited);
    
    query.close();
}