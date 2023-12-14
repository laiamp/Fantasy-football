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
using vli = vector <long long int>;


string OUTPUT_FILE;
chrono::high_resolution_clock::time_point start;
vp PLAYERS;
long long int T;


unordered_map <string, vector<Player>> get_players_pos(const vp& lineup){
    /*Returns an unordered map whose keys are the positions and the values 
    are the players of the lineup in that position*/
    unordered_map <string, vector<Player>> players_pos;
    for (Player p: lineup){
        players_pos[p.pos].push_back(p);
    }
    return players_pos;    
}


void write_result(const vp& lineup, const long long int& points, const long long int& price){
    /*Writes the solution in the OUTPUT_FILE*/


    ofstream out(OUTPUT_FILE);

    unordered_map <string, vector<Player>> players_pos = get_players_pos(lineup);   
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

void check_lineup(const vp& lineup){
    /*Auxiliar function for debugging purposes*/
    for (Player p: lineup) cout << p.name <<" "<< p.price <<" " << p.points << endl;
}


bool candidate(int i, unordered_map <string, int> n, const long long int & points, const long long int& cost,
                const long long int& max_points){
    /*Returns whether or not exists a promising solution picking the i-th player
    
    i: index of the current player from PLAYERS
    n: map with the amount of the needed players per position
    points: sum of the points of the players currently in the lineup
    cost: sum of the prices of the players currently in the lineup
    max_points: max of points of all the generated lineups until that moment
    */

    Player player = PLAYERS[i];
    if (n[player.pos] == 0) return false;
    if (cost + player.price > T) return false;

    int k = n["por"] + n["def"] + n["mig"] + n["dav"];
    if (i + k >= int(PLAYERS.size())) return false;

    //if with the current player and the next k-1 players we dont reach max_points, we discard the candidate
    long long int potential_points = points;
    for (int j = i; j < i+k; j++){
        potential_points += PLAYERS[j].points;
    }
    if (potential_points <= max_points) return false;

    //afegir condició relacionada amb el preu!!
    return true; 
}


void generate_lineup(int i, vp& lineup, unordered_map <string, int> n, long long int cost,
                        long long int points, long long int& max_points,  
                        unordered_map<string, long long int>& m_min_price,
                        unordered_map <string, int> unvisited){
    /*
        Fills up the lineup vector

        i: index of the current player from PLAYERS
        lineup: chosen players at the moment
        n: map with the amount of the needed players per position
        cost: sum of the prices of the players currently in the lineup
        points: sum of the points of the players currently in the lineup
        max_points: max of points of all the generated lineups until that moment
        m_min_price: map with the minimum price of all players from the same position 
        unvisited: map with the amount of unvisited players per position
    */
    
    Player player = PLAYERS[i];

    if (11 == int(lineup.size()) and n["por"] == 0 and n["def"] == 0 and n["mig"] == 0 and n["dav"] == 0){ // totes les posicions assignades
        
        if (points > max_points){
            max_points = points;
            write_result(lineup, points, cost);
        }
    }

    else if (i < int(PLAYERS.size()) and n[player.pos] <= unvisited[player.pos]){
        
        unvisited[player.pos]--;
            
        if (candidate(i, n, points, cost, max_points)){
            n[player.pos]--;
            lineup.push_back(player);
            generate_lineup(i+1, lineup, n, cost + player.price, points + player.points, max_points, m_min_price, unvisited);
            n[player.pos]++;
            lineup.pop_back();
        }
        generate_lineup(i+1, lineup, n, cost, points, max_points, m_min_price, unvisited);
    }
}


vp get_players_from_data(string data_file, unordered_map <string, long long int>& min_price, 
                        unordered_map <string, int>& unvisited, const long long int& J){
    /*Returns a vector of the players from data_file. Only contains the players whose price is less than 
    or equal to J. The maps min_price and unvisited are modified.
    
    format BD "Name;Position;Price;club;points"
    */

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
    /*Auxiliar function to sort the players depending on their points*/
    if(a.points == b.points) return a.price > b.price;
    return a.points > b.points;
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
    
    OUTPUT_FILE = argv[3];

    unordered_map <string, long long int> m_min_price = {{"por", 999999999}, {"def", 999999999}, {"mig", 999999999}, {"dav", 999999999}};
    
    ifstream query(argv[2]);
    long long int J;
    int N1, N2, N3;

    query >> N1 >> N2 >> N3 >> T >> J;
    
    unordered_map <string, int> n = {{"por", 1}, {"def", N1}, {"mig", N2}, {"dav", N3}};
    unordered_map <string, int> unvisited = {{"por", 0}, {"def", 0}, {"mig", 0}, {"dav", 0}};

    start = chrono::high_resolution_clock::now();
    
    PLAYERS = get_players_from_data(argv[1], m_min_price, unvisited, J);
    sort(PLAYERS.begin(), PLAYERS.end(), comp);

    vp lineup;
    long long int max_points = 0;
    
    generate_lineup(0, lineup, n, 0, 0, max_points, m_min_price, unvisited);
    
    query.close();
}