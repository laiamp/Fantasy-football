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
    long long int price;
    string team;
    long long int points;
    float ratio;
};

using vp = vector <Player>;
using vli = vector <long long int>;

string OUTPUT_FILE;
chrono::high_resolution_clock::time_point start;
float THRESHOLD_RATIO = 0.5;


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


vp get_lineup(vp players, unordered_map <string, int> n, const long long int& T){
    /*
        Returns the lineup

        lineup: chosen players at the moment
        n: map with the amount of the needed players per position
        cost: sum of the prices of the players currently in the lineup
        points: sum of the points of the players currently in the lineup
        max_points: max of points of all the generated lineups until that moment
        m_min_price: map with the minimum price of all players from the same position 
        unvisited: map with the amount of unvisited players per position
    */
    vp lineup;
    long long int total = T;
    int i = 0;
    //justificar q i mai sera mes gran que players.size()

    while (n["por"] > 0 or n["def"] > 0 or n["mig"] > 0 or n["dav"] > 0){
        if (n[players[i].pos] > 0 and total - players[i].price >= 0){
            lineup.push_back(players[i]);
            n[players[i].pos]--;
            total -= players[i].price;
        }
        i++;
    }
    return lineup;
    
}


bool comp(const Player& a, const Player& b) {
    /*Auxiliar function to sort the players depending on their points*/
    if(a.points == b.points) return a.price < b.price;
    return a.points > b.points;
}


float get_ratio(long long int points, long long int price){
    /*Returns the ratio given the points and the price. The higher the ratio the better*/
    if (price == 0) return 1;
    return float(points)/ price * 100000;
}


vp get_players_from_data(string data_file, const long long int& J){
    /*Returns a vector of the players from data_file. Only contains the players whose price is less than 
    or equal to J. 

    Players with a bad points-price relationship are filtered.
    Players with price = 0 are always kept.
    
    format DB "Name;Position;Price;club;points"
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
            float r = get_ratio(p, price);
            cout << r << endl;

            if (r >= THRESHOLD_RATIO){
                Player player = {name, position, price, club, p, r};
                players.push_back(player);
            }   
        }
    }
    //sort decreasingly by points
    sort(players.begin(), players.end(), comp);
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
    
    OUTPUT_FILE = argv[3];

    unordered_map <string, long long int> m_min_price = {{"por", 999999999}, {"def", 999999999}, {"mig", 999999999}, {"dav", 999999999}};
    
    ifstream query(argv[2]);
    long long int J, T;
    int N1, N2, N3;

    query >> N1 >> N2 >> N3 >> T >> J;
    query.close();
    
    unordered_map <string, int> n = {{"por", 1}, {"def", N1}, {"mig", N2}, {"dav", N3}};
    start = chrono::high_resolution_clock::now();

    vp PLAYERS = get_players_from_data(argv[1], J);

    vp lineup = get_lineup(PLAYERS, n, T);
    long long int points = 0, price = 0;

    for (int i = 0; i < 11; i++){
        points += lineup[i].points;
        price += lineup[i].price;
        cout << lineup[i].ratio << endl;
    }
    
    write_result(lineup, points, price);
}
