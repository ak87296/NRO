#define _USE_MATH_DEFINES
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <omp.h>
#include <ctime>

using namespace std;

int main() {

    // Inicializiramo matriko A
    vector<vector<double>> A;

    // Inicializiramo vektor b;
    vector<double> b;

    // Podamo ime datoteke
    std::string filename = "./datoteka_A_b.txt";

    // Preberemo datoteko
    std::ifstream infile;
    infile.open(filename);

    // Preberemo prvo vrstico, v kateri imamo podano velikost matrike A 
    std::string string_first_line;
    std::getline(infile, string_first_line);

    // String_first_line je enak 'A: n=256'
    // Vemo, da je delimiter pri '=', lahko zamenjamo npr. z ' '
    std::replace(string_first_line.begin(), string_first_line.end(), '=', ' ');

    // Definiramo stringstream, s katerim je nekoliko lažje shranjevati
    // podatke iz string-a, če so ločeni z ' '
    std::istringstream iss(string_first_line);
    std::string nepomemben_del1; // Sem bomo dali string 'A:' iz prve vrstice
    std::string nepomemben_del2; // Sem bomo dali string 'n' iz prve vrstice
    int n; // Sem bomo dali velikost matrike A

    // Istringstream loči glede na ' ', trenutno imamo 'A: n 256'
    iss >> nepomemben_del1;
    iss >> nepomemben_del2;
    iss >> n;

    std::cout << "Velikost matrike A: " << n << "x" << n << std::endl;;

    // V naslednjih n vrsticah imamo elemente matrike A, zato naredimo
    // iteracijo, da preberemo in zafilamo matriko A
    for (int iiA = 0; iiA < n; iiA++) {
        // Preberemo vrstico
        std::string line;
        std::getline(infile, line);
        // Zamenjamo ';' s  ' ', saj istringstream loči glede na ' '
        std::replace(line.begin(), line.end(), ';', ' ');

        // Z istringstream ponovno pretvorimo string
        std::istringstream iss_column(line);

        // Definiramo nov vektor, da shranimo vrstico 
        vector<double> row;

        // Sedaj lahko iteriramo po elementih v iss_column
        for (int column = 0; column < n; column++) {
            double element_a = 0;
            iss_column >> element_a;
            row.push_back(element_a);
        }

        // Ko imamo vektor za vrstico, ga damo v A
        A.push_back(row);
    }

    // Sedaj imamo sestavljeno matriko A. Naslednja vrstica je prazna,
    // zato jo samo preberemo in ne naredimo ničesar.
    std::string empty_line;
    std::getline(infile, empty_line);

    // Prebrati moramo še vektor b
    std::string string_line_b;
    std::getline(infile, string_line_b);

    // Lahko uporabimo podoben trik kot pri prvi vrstici
    std::replace(string_line_b.begin(), string_line_b.end(), '>', ' ');
    std::istringstream iss_b(string_line_b);
    int n_b; // Sem bomo dali velikost vektorja b (ki je identična velikosti A)

    iss_b >> nepomemben_del1;
    iss_b >> nepomemben_del2;
    iss_b >> n_b;

    std::cout << "Velikost vektorja b: " << n_b << std::endl;;

    // Naredimo iteracijo po naslednjem n_b številu vrstic
    for (int iib = 0; iib < n_b; iib++) {
        // Preberemo vrstico in shranimo element v vrstici v vektor b
        std::string line_b_element;
        std::getline(infile, line_b_element);
        std::istringstream iss_b_element(line_b_element);

        double b_element = 0;
        iss_b_element >> b_element;

        b.push_back(b_element);
    }

    // Inicializiramo vektor rešitve T in SOR parameter omega
    vector<double> T(n, 100.0);  // Definiramo začetni približek na 100
    vector<double> T_old(n, 100.0);  // Kopija za preverjanje konvergence
    double omega = 1.5;  // Relaksacijski faktor za SOR metodo
    double max_error = 1e-10;  // Toleračna napaka za preverjanje konvergence
    //Sama tolerančna napaka močno vpliva na čas izvajanja programa. Napaka 1e-6 izvede program v 0,38 sekunde, 1e-15 pa izvede program v 0,81 (pred implementacijo SOR). 
    // Glede na našo zahtevo določimo napako konvergence. 

    bool converged = false;  // Preverimo, ali smo dosegli konvergenco

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int iter = 0; iter < 2000 && !converged; iter++) {
        converged = true;  // Na začetku predpostavljam, da bo metoda konvergirala

        // Gauss-Seidelova metoda je iterativna metoda (potrebujemo začetni približek in število iteracij) za reševanje sistemov linearnih enačb, kjer se pri izračunu vsakega novega rezultata
        // uporablja prejšnje rezultate iz prejšnje iteracije. Zaradi tega je običajno težko paralelizirati to metodo, saj so vrednosti, ki jih izračuna ena nit, potrebne za izračun drugih vrednosti
        // v isti iteraciji.
        // Za izboljšanje učinkovitosti paralelizacije pa lahko uporabimo tehniko, imenovano ločevanje na parne in neparne indekse (rdeči in črni vozli). To omogoči, da se 
        // naenkrat izračunajo rezultati za parne indekse in neparne indekse brez medsebojnega vpliva, saj so te vrednosti izračunane z uporabo že izračunanih prejšnjih vrednosti.
        // Z uporabo ločevanja na parne in neparne indekse lahko tako hkrati izvajamo dve ločeni nalogi v paraleli, kjer ena nit računa vrednosti za parne indekse, druga pa za neparne
        // indekse. Ker med njima ni odvisnosti (parni indeksi ne vplivajo na neparne in obratno), omogočimo učinkovitejšo uporabo večjedrnih procesorjev in zmanjšamo potreben čas za iteracijo.

        // V nadaljevanju je omenjena metoda implementirana. Ista metoda je uporabljena tudi v projektu. 

        // Čas izračuna lahko zmanjšamo tudi z uporabo več niti. V tem primeru imamo relativno majhen primer, zato povečevanje število niti nima tako velikega vpliva, ampak se pozna
        // zgolj na kakšni decimalki. V primeru zahtevnejšega primera z več točkami (na primer pri projektu) pa nam uporaba večih jeder močno pohitri izračun.

        int num_threads = 10; // Nastavimo število niti

        // Določimo število niti glede na razpoložljiva procesorska jedra
        int available_procs = omp_get_num_procs();
        if (num_threads > available_procs) {
            num_threads = available_procs; // Če imamo manj procesorskih jeder, omejimo število niti
        }

        omp_set_num_threads(num_threads);

        // Gauss-Seidelova metoda - rdeči vozli (parni indeksi)
#pragma omp parallel for schedule(dynamic) shared(A, b, T, n)
        for (int i = 0; i < n; i++) {
            if (i % 2 == 0) {  // Rdeči vozli
                double sum = b[i];
                for (int j = 0; j < n; j++) {
                    if (j != i) sum -= A[i][j] * T[j]; 
                }
                double T_new = sum / A[i][i];  // Delitev z diagonalo
                T[i] = omega * T_new + (1 - omega) * T[i];  
            }
        }

        // Gauss-Seidel - črni vozli (neparni indeksi)
#pragma omp parallel for schedule(dynamic) shared(A, b, T, n)
        for (int i = 0; i < n; i++) {
            if (i % 2 != 0) {  // Črni vozli
                double sum = b[i];
                for (int j = 0; j < n; j++) {
                    if (j != i) sum -= A[i][j] * T[j]; 
                }
                double T_new = sum / A[i][i];  
                T[i] = omega * T_new + (1 - omega) * T[i];  // SOR posodobitev
            }
        }

        // Preverjanje konvergence
        for (int i = 0; i < n; i++) {
            if (abs(T[i] - T_old[i]) > max_error) {
                converged = false;  // Če ni konvergiralo, ponovimo iteracijo
            }
            T_old[i] = T[i];  // Shranimo trenutno rešitev za naslednjo iteracijo
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_duration = end_time - start_time;
    std::cout << "Time of Gauss-Seidel: " << time_duration.count() << " seconds" << std::endl;

    // Za izpis maksimalne vrednosti
    double max_T = 0;
    for (int iiT = 0; iiT < n_b; iiT++) {

        if (T[iiT] > max_T) {
            max_T = T[iiT];
        }
    }
    cout << "Max. temperature: " << max_T << "  °C." << endl;

    return 0;
}

// Ugotovimo, da je najvišja temperatura 500°C.

// Za hitrejšo konvergenco smo uporabili metodo SOR (Successive Over-Relaxation). S tem definiramo faktor relaksacije omega,
// ki ima vrednost med 1 in 2. Vrednost 1 predstavlja klasično Gauss-Seidelovo metodo. Z definiranjem omege pospešimo in 
// stabiliziramo reševanje sistema enačb.
// Hitrost izračuna se poveča za faktor 4 (iz približno 0,6 na 0,15 sekunde).