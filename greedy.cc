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
    float ratio;
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

bool comp(const Player& a, const Player& b) {
    /*Auxiliar function to sort the players depending on their points*/
    if(a.points == b.points) return a.price < b.price;
    return a.points > b.points;
}

vp get_filtered_players(vp players, float avg_ratio){
    /*Returns a filtered vector with the players whose ratio is over the threshold.

    */

    vp filtered;
    float threshold = avg_ratio*(1 - TOLERANCE);
    for (Player p: players){
        if (p.ratio >= threshold) filtered.push_back(p);
    }
    return filtered;
}


Solution get_solution(vp players, unordered_map <string, int> n, const int& T,
                        const float& avg_ratio){
    /*
        Returns the lineup

        lineup: chosen players at the moment
        n: map with the amount of the needed players per position
        cost: sum of the prices of the players currently in the lineup
        points: sum of the points of the players currently in the lineup
        max_points: max of points of all the generated lineups until that moment
        unvisited: map with the amount of unvisited players per position
    */
    Solution solution = {{}, 0, 0};
    int i = 0;
    //justificar q i mai sera mes gran que players.size()

    players = get_filtered_players(players, avg_ratio); //filter players based on their ratio
    sort(players.begin(), players.end(), comp); //sort decreasingly by points

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


float get_ratio(int points, int price){
    /*Returns the ratio given the points and the price. The higher the ratio the better*/
    if (price == 0) return 1;
    return float(points)/ price * 10000;
}


vp get_players_from_data(string data_file, const int& J, float& avg_ratio){
    /*Returns a vector of the players from data_file. Only contains the players whose price is 
    less than or equal to J.

    Players with a bad points-price relationship are filtered.
    Players with price = 0 are always kept.
    
    format DB "Name;Position;Price;club;points"
    */

    ifstream data(data_file);
    vp players;
    string line;
    float sum_ratio = 0;

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
            float r = get_ratio(p, price);
            if (price > 0) sum_ratio += r;
            Player player = {name, position, price, club, p, r};
            players.push_back(player);      
        }
    }
    avg_ratio = sum_ratio / int(players.size());
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
    float avg_ratio;
    vp PLAYERS = get_players_from_data(db_file, J, avg_ratio); //data filtered
    unordered_map <string, int> n = {{"por", 1}, {"def", N1}, {"mig", N2}, {"dav", N3}};
    Solution solution = get_solution(PLAYERS, n, T, avg_ratio);
    write_result(solution.lineup, solution.points, solution.price, output_file);
}
