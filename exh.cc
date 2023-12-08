#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

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
long long int T, J;

unordered_map <string, vector<Player>> get_players_pos(const vp& alineacio){
    unordered_map <string, vector<Player>> players_pos = {{"por", {}}, {"def", {}}, {"mig", {}}, {"dav", {}}};
    for (Player p: alineacio){
        players_pos[p.pos].push_back(p);
    }
    return players_pos;    
}


void write_result(const vp& alineacio){
    
    ofstream out(OUTPUT_FILE);

    auto end = chrono::high_resolution_clock::now();
    auto time = chrono::duration_cast<std::chrono::milliseconds>(end - start);
    out << time.count() << endl;

    unordered_map <string, vector<Player>> players_pos = get_players_pos(alineacio);   
    vector <string> positions = {"por", "def", "mig", "dav"};

    for (string pos: positions){
        out << pos << ": ";
        for (int i = 0; i < int(players_pos[pos].size()); i++){
            if (i > 0) out << ";";
            out << players_pos[pos][i].name;
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
    - No rebutjar si N_i - index  0 --> he de precalcular quants jugadors hi ha de cada posicio
    - No agafar cap jugador que superi els J euros
    - No agafar cap jugador que superi els T euros
    - Precalcular el màxim de punts per posició amb una pque
    - Precalcular el mínim d'euros per posició (o els n_i de menys euros) amb una pque
    - Comptar el total de cada posició i els count de visitats

    - Faig una array mida 450. A cada posició hi haurà 4 vectors.
*/
void check_alineacio(const vp& alineacio){
    for (Player p: alineacio) cout << p.name <<" "<< p.price <<" " << p.points << endl;
}

void generar_alineacio(int i, vp& alineacio, unordered_map <string, int> n, long long int t,
                        long long int points, long long int& max_points, 
                        unordered_map<string, long long int>& m_max_points, 
                        unordered_map<string, long long int>& m_min_price){
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
    bool check = false;
    if (PLAYERS[i].name == "Marchesín") check = true;

    if (11 == int(alineacio.size()) and n["por"] == 0 and n["def"] == 0 and n["mig"] == 0 and n["dav"] == 0){ // totes les posicions assignades
        //check_alineacio(alineacio);
        if (points > max_points){
            max_points = points;
            write_result(alineacio);
        }
    }

    else if (i < int(PLAYERS.size())){
        //el segon mig que magafa hauria de ser "Youba Diarra"
        Player player = PLAYERS[i];
        
        //pot fallar si n[players.pos] == 0, pero no ens importa
        long long int points_estimats = points + n["por"]*m_max_points["por"] 
                                        + n["def"]*m_max_points["def"]
                                        + n["mig"]*m_max_points["mig"]
                                        + n["dav"]*m_max_points["dav"]
                                        - m_max_points[player.pos]
                                        + player.points;


        if (player.name == "Youba Diarra" and n[player.pos] > 0){
            cout << player.price <<" " << J << endl;
            cout << "points estimats: " << points_estimats <<endl;
            cout << "maxpoints: " << max_points << endl;
        }
        if (n[player.pos] > 0 and player.price <= J and t - player.price >= 0 and points_estimats >= max_points){
            n[player.pos]--;
            alineacio.push_back(player);
            generar_alineacio(i+1, alineacio, n, t - player.price, points + player.points, max_points, m_max_points, m_min_price);
            n[player.pos]++;
            alineacio.pop_back();
        }
        //no l'agafo o be pq no puc
        generar_alineacio(i+1, alineacio, n, t, points, max_points, m_max_points, m_min_price);
    }
}


vp get_players_from_data(string data_file, unordered_map <string, long long int>& min_price, unordered_map <string, long long int>& max_points){
    ifstream data(data_file);
    vp players;
    string line;
    //vi total_players(4, 0);

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
        Player player = {name, position, price, club, p};
        players.push_back(player);
        if (player.price <= J) max_points[player.pos] = max(player.points, max_points[player.pos]);
        min_price[player.pos] = min(player.price, min_price[player.pos]);
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
    
    unordered_map <string, long long int> m_min_price = {{"por", 999999999}, {"def", 999999999}, {"mig", 999999999}, {"dav", 999999999}};
    unordered_map <string, long long int> m_max_points = {{"por", 0}, {"def", 0}, {"mig", 0}, {"dav", 0}};

    
    PLAYERS = get_players_from_data(argv[1], m_min_price, m_max_points);
    cout << m_max_points["dav"];
    ifstream query(argv[2]);


    query >> N1 >> N2 >> N3 >> T >> J;
    
    unordered_map <string, int> n = {{"por", 1}, {"def", N1}, {"mig", N2}, {"dav", N3}};
    
    
    //vi n = {1, N1, N2, N3};
    OUTPUT_FILE = argv[3];
    vp alineacio;
    
    long long int max_points = 0, points = 0;

    start = chrono::high_resolution_clock::now();
    generar_alineacio(0, alineacio, n, T, points, max_points, m_max_points, m_min_price);
    
    query.close();
}