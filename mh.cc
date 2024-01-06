#include <iostream>

using namespace std;


/*
GRASP:




VARIABLE NEIGHBORHOOD SEARCH

nborhood: totes les solucions obtingudes fixant k jugadors formen part 
            dun mateix nborhood

nomes considerarem els subsets formats per jugadors prometedors

jugador prometedor: jugador amb bon ratio punts/preu



s = generateInitialSolution()

while termination conditions not met do
    k = 1
    while k <= kmax do
        s' = pickrandom(N_k(s))
        s'' = localSearch(s')

        if f(s'') < f(s) then
            s = s''
            k = 1
        else:
            k = k + 1

        endif
    endwhile
endwhile


greedy:
- agrupemgit  jugadors per posicio i filtrem els que tinguin un preu > J
- per cada posicio, els agrupem en k grups segons preus (default k = 3)
- dins de cada grup per preu, els ordenem descendentment segons puntuacio
- fem totes les combinacions m_1, ... , m_k tals que sum(m_i) = N (n1, n2 o n3 segons posicio)

els punts i el total que ens sobri duna posicio sera linput per la seguent posicio 
*/

int main(){

}