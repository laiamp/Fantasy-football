#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <limits>

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
vp PLAYERS;


unordered_map <string, vp> get_players_pos(const vp& lineup){
    /*
    Returns an unordered map whose keys are the positions and the values 
    are the players of the lineup in that position
    */
    
    unordered_map <string, vp> players_pos;
    for (Player p: lineup){
        players_pos[p.pos].push_back(p);
    }
    return players_pos;    
}


void write_solution(const Solution& sol){
    /*
    Writes the execution time, the players of each position of the lineup,
    its points and its price in the OUTPUT_FILE
    */

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


bool possible_complete_from_partial(int i, int points, const unordered_map <string, int>& n, 
                                    int max_points, const unordered_map <string, int>& unvisited){
    /*
    Returns whether from the i-th player onwards a solution may exist or not.

    Prerequisite: PLAYERS is sorted descending by points.

    i: index of the current player from PLAYERS
    n: map with the amount of the needed players per position
    points: sum of the points of the players currently in the lineup
    max_points: max of points of all the generated lineups until that moment
    */

    int remaining = n.at("por") + n.at("def") + n.at("mig") + n.at("dav");

    if (i + remaining > int(PLAYERS.size())) return false;  // not enough players left
    
    if (n.at(PLAYERS[i].pos) > unvisited.at(PLAYERS[i].pos)) return false;    // not enough players left of this position
    
    int potential_points = points;
    for (int j = i; j < i + remaining; j++){
        potential_points += PLAYERS[j].points;
    }
    return potential_points > max_points;
}


int lower_bound_price(const Player& player, int current_price, const unordered_map <string, int>& n, 
                       const unordered_map <string, int>& min_price){
    /*
    Returns the lower bound of the price if the current player is added.
    It is computed considering that the remaining players have the lowest price 
    of their position.
    */
    
    return player.price + current_price
            + n.at("por")*min_price.at("por") + n.at("def")*min_price.at("def")
            + n.at("mig")*min_price.at("mig") + n.at("dav")*min_price.at("dav")
            - min_price.at(player.pos);
}


void gen_solution(int i, Solution& sol, unordered_map <string, int> n,
                  int& max_points, unordered_map <string, int> unvisited, 
                  const unordered_map <string, int>& min_price, const int T){
    /*
        Fills up the lineup vector of sol and computes its points and price.
        The target is to maximize max_points while meeting the constraints given.

        Prerequisite: PLAYERS is sorted decreasingly by points.

        i: index of the current player from PLAYERS
        sol: solution being generated at the moment. Contains lineup vector, its points and its price
        n: map with the amount of needed players per position
        max_points: max of points of all the generated lineups until that moment
        unvisited: map with the amount of unvisited players per position
        min_price: map with the minimum price among all the players of that position
    */
    
    if (n["por"] == 0 and n["def"] == 0 and n["mig"] == 0 and n["dav"] == 0){
        
        if (sol.points > max_points){
            max_points = sol.points;
            write_solution(sol);
        }
    }
    else if (possible_complete_from_partial(i, sol.points, n, max_points, unvisited)){
        Player player = PLAYERS[i];
        unvisited[player.pos]--;

        if (n[player.pos] > 0 and lower_bound_price(player, sol.price, n, min_price) <= T){
            n[player.pos]--;
            sol.lineup.push_back(player);
            sol.price += player.price;
            sol.points += player.points;
            gen_solution(i+1, sol, n, max_points, unvisited, min_price, T);
            sol.price -= player.price;
            sol.points -= player.points;
            n[player.pos]++;
            sol.lineup.pop_back();
        }
        gen_solution(i+1, sol, n, max_points, unvisited, min_price, T);
    }
}


vp get_DB_players(string data_file, unordered_map <string, int>& unvisited,
                  unordered_map <string, int>& min_price, const int J){
    /*
    Returns a vector of the players from data_file. Only contains the players whose price is less than 
    or equal to J. 
    
    Modifies the maps
    unvisited: includes how many players of each position don't exceed price J.
    min_price: minimum price among all players in that position
    
    DB format "Name;Position;Price;club;points"
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
            unvisited[position]++;
            min_price[position] = min(min_price[position], price);
        }
    }
    data.close();
    return players;
}


bool comp(const Player& a, const Player& b) {
    /*Auxiliar function to sort the players descending by points*/
    if(a.points == b.points) return a.price < b.price;
    return a.points > b.points;
}


int main(int argc, char** argv){
    /*
    argv[1]: name DB file
    argv[2]: name query file 
    argv[3]: name output file
    */

    if (argc != 4) {
        cout << "Syntax: " << argv[0] << " data_base.txt query.txt output.txt" << endl;
        exit(1);
    }
    
    OUTPUT_FILE = argv[3];

    ifstream query(argv[2]);
    
    int N1, N2, N3, T, J;
    query >> N1 >> N2 >> N3 >> T >> J;
    query.close();
    
    unordered_map <string, int> n = {{"por", 1}, {"def", N1}, {"mig", N2}, {"dav", N3}};
    unordered_map <string, int> unvisited = {{"por", 0}, {"def", 0}, {"mig", 0}, {"dav", 0}};
    unordered_map <string, int> min_price = {{"por", numeric_limits<int>::max()},
                                                {"def", numeric_limits<int>::max()},
                                                {"mig", numeric_limits<int>::max()},
                                                {"dav", numeric_limits<int>::max()}
                                                };

    start = chrono::high_resolution_clock::now();
    
    PLAYERS = get_DB_players(argv[1], unvisited, min_price, J);    // unvisited is modified
    sort(PLAYERS.begin(), PLAYERS.end(), comp);
    
    Solution sol = {{}, 0, 0};
    int max_points = 0;
    gen_solution(0, sol, n, max_points, unvisited, min_price, T);
}
