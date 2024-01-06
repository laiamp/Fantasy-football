#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <cmath>

using namespace std;

struct Player{
    string name;
    string pos;
    int price;
    string team;
    int points;
};

struct Solution{
    vector <Player> lineup;
    int points;
    int price;
};

using vp = vector <Player>;

string OUTPUT_FILE;
chrono::high_resolution_clock::time_point start;

unordered_map <string, vp> get_players_pos(const vp& lineup){
    /*Returns an unordered map whose keys are the positions and the values 
    are the players of the lineup in that position*/
    unordered_map <string, vp> players_pos;
    for (Player p: lineup){
        players_pos[p.pos].push_back(p);
    }
    return players_pos;    
}


void write_solution(const Solution& sol){
    /*Writes the solution in the OUTPUT_FILE*/

    ofstream out(OUTPUT_FILE);

    unordered_map <string, vp> players_pos = get_players_pos(sol.lineup);   

    chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    double time = duration.count();
    out << fixed << setprecision(1) << time << endl;

    const vector <string> POSITIONS = {"por", "def", "mig", "dav"};

    for (string pos: POSITIONS){
        string pos_upper = pos;
        transform(pos_upper.begin(), pos_upper.end(), pos_upper.begin(), ::toupper);
        out << pos_upper << ": "; 
        for (int i = 0; i < int(players_pos[pos].size()); i++){
            out << (i == 0 ? "" : ";") << players_pos[pos][i].name;
        }
        out << endl;
    }

    out << "Punts: " << sol.points << endl;
    out << "Preu: " << sol.price << endl;
    out.close();
}


bool comp(const Player& p1, const Player& p2){
    /*
    Auxiliar function to sort the players based on the players points and price
    A and B are empirically adjusted coefficients.
    */

    const float A = 3.2;
    const float B = -1;

    if (p1.points == p2.points) return p1.price < p2.price; //if both are 0, the return below wouldn't work
    if (p1.price == 0) return false; //p1 fake
    if (p2.price == 0) return true; //p2 fake

    return pow(p1.points, A)*pow(p1.price, B) > pow(p2.points, A)*pow(p2.price, B); 
}


Solution get_solution(vp& players, unordered_map <string, int> n, const int& T){
    /*
        Returns a struct with the lineup, its points and its price

        players: vector with the players with a price <= J
        n: map with the amount of the needed players per position
        T: maximum total price
    */
    
    vp lineup;
    int i = 0, price = 0, points = 0;

    sort(players.begin(), players.end(), comp);

    while (n["por"] > 0 or n["def"] > 0 or n["mig"] > 0 or n["dav"] > 0){
        Player player = players[i];
        if (n[player.pos] > 0 and price + player.price <= T){
            lineup.push_back(player);
            n[player.pos]--;
            price += player.price;
            points += player.points;
        }
        i++;
    }
    return {lineup, points, price};
}


vp get_DB_players(const string& db_file, const int& J){
    /*Returns a vector of the players from db_file. Only contains the players 
    whose price is less than or equal to J.
    
    DB format: "Name;Position;Price;club;points"
    */

    ifstream data(db_file);
    vp players;
    string line;

    while (not data.eof()) {
        string name, club, position, aux2;
        int price, points;
        char aux;
        getline(data,name,';');    if (name == "") break;
        getline(data,position,';');
        data >> price >> aux;
        getline(data,club,';');
        data >> points;
        getline(data, aux2);
        if (price <= J){
            Player player = {name, position, price, club, points};
            players.push_back(player);      
        }
    }
    data.close();
    return players;
}


int main(int argc, char** argv){

    if (argc != 4) {
        cout << "Syntax: " << argv[0] << " data_base.txt query.txt output.txt" << endl;
        exit(1);
    }

    OUTPUT_FILE = argv[3];   

    ifstream query(argv[2]);
    int N1, N2, N3, T, J;
    query >> N1 >> N2 >> N3 >> T >> J;
    query.close();
    
    start = chrono::high_resolution_clock::now();
    vp players = get_DB_players(argv[1], J);
    unordered_map <string, int> n = {{"por", 1}, {"def", N1}, {"mig", N2}, {"dav", N3}};
    Solution solution = get_solution(players, n, T);
    write_solution(solution);
}
