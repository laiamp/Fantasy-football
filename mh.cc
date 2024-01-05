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
#include <random>

using namespace std;

struct Player{
    string name;
    string pos;
    long long int price;
    string team;
    long long int points;
};

struct Solution{
    vector<Player> lineup;
    long long int price;
    long long int points;

};

using vp = vector <Player>;

string OUTPUT_FILE;
chrono::high_resolution_clock::time_point start;

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

    out << "Punts: " << points << endl;
    out << "Preu: " << price << endl;
    out.close();
}


bool in_vector(const vp& v, const Player& P){
    /*Returns true if Player p is found in vp v, otherwise returns false*/
    for (Player p : v){
        if ((P.name == p.name)
            and (P.pos == p.pos)
            and (P.price == p.price)
            and (P.team == p.team)
            and(P.points == p.points))  return true;
    }

    return false;
}


Solution pick_neighbor(const vp& players, Solution s, const long long int& available){
    /*
    Returns a solution with a player of the lineup changed with a random player from the available ones
    which can fit in the lineup given an amount of money
    */
    const long long int N = players.size();
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(seed);
    uniform_int_distribution<> unif1(0, 10);    // Used to generate a lineup index
    uniform_int_distribution<> unif2(0, N-1);   // Used to generate a player's list's index
    int lineup_idx = unif1(gen), player_idx;
    string pos = s.lineup[lineup_idx].pos;  // Pick random player's position
    bool found = false;
    long long int price_diff;
    Player candidate; 
    while (!found){
        player_idx = unif2(gen);
        candidate = players[player_idx];
        price_diff = candidate.price - s.lineup[lineup_idx].price;
        if (pos == candidate.pos && price_diff <= available && !in_vector(s.lineup, candidate)){
            found = true;
            // Update new solution's parameters
            s.points += (candidate.points - s.lineup[lineup_idx].points);
            s.price += price_diff;
            s.lineup[lineup_idx] = candidate;
        }
    }

    return s;
}


Solution lineup_improvement(const vp& players, Solution lineup, long long int available){
    /*Given a possible lineup, it improves it and returns it as a solution
    Implements a Simulated Annealing*/
    double T = 1e5, p;
    const double alpha = 0.99; 
    Solution best = lineup, s;
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(seed);
    uniform_real_distribution<> unif(0.0, 1.0); // Used to generate a random p to test
    int it = 0, maxIt = 100;
    while(it < maxIt){  // We stablish a limit of 100 iterations without changing the candidate lineup
        s = pick_neighbor(players, lineup, available);
        p = exp(-double(lineup.points - s.points)/T);   // p computed with the Boltzmann distribution
        if (s.points > lineup.points || p > unif(gen)){ // Better solution or random pick
            available -= (s.price-lineup.price);
            lineup = s;
            it = 0;
        } else  it += 1;

        if (lineup.points > best.points)    best = lineup;  // Improvement

        if(T > 0.01)    T *= alpha; // Reduce the T value
    }

    return best;
}


vp gen_candidate_list(const vp& players, unordered_map <string, int> n, const long long int& available, const int& alpha, const vector<Player>& banned){
    /*Generates a candidate list with the players that could fit in the lineup with a max of alpha candidates
    Prerequisite: players sorted by descending points*/
    vp candidates(0);
    int i = 0;
    Player player;
    int N = players.size();
    while(int(candidates.size()) < alpha and i < N){
        player = players[i];
        if(player.price <= available && n[player.pos] > 0 && !in_vector(banned, player)){
            candidates.push_back(player);
        }
        i += 1;
    }

    return candidates;
}


Solution gen_arbitrary_lineup(const vp& players, unordered_map <string, int> n, const long long int& T){
    /*Generates a pseudo-greedy lineup given an array of players, the number of players in each position and a maximum cost
    Returns the vector of players*/
    int alpha = 11, randIdx;    // Max number of candidate players
    long long int available = T, price, points;
    vp candidates, lineup;
    Player candidate;
    vector<Player> banned;  // Players already in the lineup
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(seed);
    for(int i = 0; i < 11; i++){    // Chooses every player from the lineup
        candidates = gen_candidate_list(players, n, available, alpha, banned);
        uniform_int_distribution<> unif(0, candidates.size() - 1);  // To generate a random candidate position
        randIdx = unif(gen);
        candidate = candidates[randIdx];
        lineup.push_back(candidate);
        banned.push_back(candidate);
        available -= candidate.price;
        price += candidate.price;
        points += candidate.points;
        n[candidate.pos] -= 1;
    }

    return {lineup, price, points};
}


Solution get_lineup(const vp& players, unordered_map <string, int> n, const long long int& T){
    /*Generates a random lineup and improves it. Returns it as a Solution*/
    Solution solution = gen_arbitrary_lineup(players, n, T);
    return lineup_improvement(players, solution, T-solution.price);
}


bool comp(const Player& a, const Player& b){
    /*Auxiliar function used in sorting players by points descending*/
    if (a.points == b.points)    return a.price < b.price;
    return a.points > b.points;
}


vp get_players_from_data(string data_file, const long long int& J){
    /*Returns a vector of the players from data_file. Only contains the players whose price is less than 
    or equal to J. 

    format DB "Name;Position;Price;club;points"
    */

    ifstream data(data_file);
    vp players;
    string line;

    while (not data.eof()) {
        string name, club, position, aux2;
        long long int price; long long int p;
        char aux;
        Player player;
        getline(data,name,';');    if (name == "") break;
        getline(data,position,';');
        data >> price >> aux;
        getline(data,club,';');
        data >> p;
        getline(data, aux2);
        if (price <= J){
            player = {name, position, price, club, p};
            players.push_back(player);
        }
    }
    data.close();

    sort(players.begin(), players.end(), comp);
    return players;
}


int main(int argc, char** argv){

    if (argc != 4) {
        cout << "Syntax: " << argv[0] << " data_base.txt query.txt output.txt" << endl;
        exit(1);
    }
    
    OUTPUT_FILE = argv[3];
    ifstream query(argv[2]);
    long long int J, T;
    int N1, N2, N3;

    query >> N1 >> N2 >> N3 >> T >> J;
    
    query.close();
    
    unordered_map <string, int> n = {{"por", 1}, {"def", N1}, {"mig", N2}, {"dav", N3}};
    start = chrono::high_resolution_clock::now();

    const vp PLAYERS = get_players_from_data(argv[1], J);

    long long int best_points = -1;
    Solution solution;
    while(true){
        solution = get_lineup(PLAYERS, n, T);
        if (solution.points > best_points){
            write_result(solution.lineup, solution.points, solution.price);
            best_points = solution.points;
        }
    }
}   