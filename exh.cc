#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct player{
    string name;
    string pos;
    int price;
    string team;
    int points;
};

using vp = vector <player>;
using vi = vector <int>;

/*ifstream nom_arxiu_entrada("file.txt") // input file stream
in >> var;
nom_arxiu_entrada.close();

ofstream nom_arxiu_sortida("file.txt") 
out << var;
nom_arxiu_sortida.close()*/
/*
constraints:
- maxim total de T euros
- maxim J euros per jugador
- n1 defenses, n2 migcampistes i n3 davanters
- 1 <= n1, n2, n3 <= 5
- n1 + n2 + n3 = 10
- 1 porter
*/

void write_result(const vp& alineacio, string output_file){
    ofstream out(output_file);

    int porter, defs, mig, dav, points, price;
    out << time << endl;
    out << "POR: " << porter << endl; 
    out << "DEF: " << defs << endl;
    out << "MIG: " << mig << endl;
    out << "DAV: " << dav << endl;
    out << "Punts: " << points << endl;
    out << "Preu: " << price << endl;
    out.close();
}

void generar_alineacio(vp& alineacio, int sum_price, vi n, string output_file){
    //la suma de price no pot superar T
    //no podem escollir un jugador que superi J euros
    //la suma de 
    //if jugadors_db[punter].pos == 'por'
    //else if jugadors
    //if tots els jugadors collocats
        //write_result(alineacio)
    
    //escriure 
}

/*
Idees:
- Podem comptar el total de cada posició i els count de visitats
*/

vector <vp> get_players(char** argv){
    ifstream data(argv[1]);
    vector <vp> v_players(4);
    string line;

    while(getline(data, line)){
        istringstream ss(line); //error en ss
        vector <string> elements;
        string word;
        while(getline(ss, word, ";")){ //error getline
            elements.push_back(word);
        }
        int idx;
        if (elements[1] == 'por') idx = 0;
        else if (elements[1] == 'def') idx = 1;       }
        else if (elements[1] == 'mig') idx = 2;
        else idx = 3;
        v_players[idx].push_back({elements[0], elements[1], elements[2]});
    }

    return v_player;

}

int main(int argc, char** argv){
    // llegim "n1 n2 n3 t j" del fitxer de consulta
    // format BD "Nom;Posicio;Preu;Equip;Punts"
    //argv[0]: nom arxieu BD
    //argv[1]: nom arxiu consulta
    
    ifstream query(argv[2]);

    int n1, n2, n3, t, j;
    query >> n1 >> n2 >> n3 >> t >> j;

    string output_file = argv[3];
    vp alineacio(1 + n1 + n2 + n3 + 1);
    vector<int> n = {1, n1, n2, n3};

    generar_alineacio(alineacio, 0, n, output_file);


    //generar vector de jugadors (1 porter, 2 + n1 - 1 mig, 2 + n1 + dav - 1, )
    //calcular valor de jugadors
    
    data.close();
    query.close();
    
    /*
    output:
    - double segons que triga amb 1 decimal
    - "POR: " + nom del porter
    - "DEF: " + defenses separats amb ";"
    - "MIG: " + migca
    - "DAV: "
    - "Punts: " punts equip
    - "Preu: " preu equip
    */
}