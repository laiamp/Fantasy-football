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
using vli = vector <int>;

string OUTPUT_FILE;
chrono::high_resolution_clock::time_point start;
float TOLERANCE = 0.25;


unordered_map <string, vp> get_players_pos(const vp& lineup){
    /*Returns an unordered map whose keys are the positions and the values 
    are the players of the lineup in that position*/
    unordered_map <string, vp> players_pos;
    for (Player p: lineup){
        players_pos[p.pos].push_back(p);
    }
    return players_pos;    
}


void write_result(const vp& lineup, const int& points, const int& price, const string& output_file){
    /*Writes the solution in the OUTPUT_FILE*/

    ofstream out(output_file);

    unordered_map <string, vp> players_pos = get_players_pos(lineup);   
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


const float A = 2;
const float B = -1;

bool comp(const Player& p1, const Player& p2){
    if (p1.points == p2.points) return p1.price < p2.price; //if both are 0, the return below wouldn't work
    if (p1.price == 0) return false; //p1 fake
    if (p2.price == 0) return true; //p2 fake

    return pow(p1.points, A)*pow(p1.price, B) > pow(p2.points, A)*pow(p2.price, B); 
}


Solution get_solution(vp& players, unordered_map <string, int> n, const int& T){
    /*
        Returns the lineup

        lineup: chosen players at the moment
        n: map with the amount of the needed players per position
        T: maximum total price
    */
    Solution solution = {{}, 0, 0};
    int i = 0;
    //assert de que sempre podrem fer una solucio

    sort(players.begin(), players.end(), comp);

    while (n["por"] > 0 or n["def"] > 0 or n["mig"] > 0 or n["dav"] > 0){
        Player player = players[i];
        if (n[player.pos] > 0 and solution.price + player.price <= T){
            solution.lineup.push_back(player);
            n[player.pos]--;
            solution.price += player.price;
            solution.points += player.points;
        }
        i++;
    }
    return solution;
}


vp get_players_from_data(string data_file, const int& J){
    /*Returns a vector of the players from data_file. Only contains the players whose price is 
    less than or equal to J.
    
    format DB "Name;Position;Price;club;points"
    */

    ifstream data(data_file);
    vp players;
    string line;
    

    while (not data.eof()) {
        string name, club, position, aux2;
        int price; int p;
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
        }
    }
    data.close();
    return players;
}


int main(int argc, char** argv){
    /*
    argv[1]: name DB file
    argv[2]: name query file 
    argv[3]: name output file*/

    if (argc != 4) {
        cout << "Syntax: " << argv[0] << " data_base.txt query.txt output.txt" << endl;
        exit(1);
    }
    string db_file = argv[1];
    string query_file = argv[2];
    string output_file = argv[3];   

    ifstream query(query_file);
    int N1, N2, N3, T, J;
    query >> N1 >> N2 >> N3 >> T >> J;
    query.close();
    
    start = chrono::high_resolution_clock::now();
    vp PLAYERS = get_players_from_data(db_file, J);
    unordered_map <string, int> n = {{"por", 1}, {"def", N1}, {"mig", N2}, {"dav", N3}};
    Solution solution = get_solution(PLAYERS, n, T);
    write_result(solution.lineup, solution.points, solution.price, output_file);
}
