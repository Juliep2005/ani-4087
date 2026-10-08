#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

unsigned long long ValeurDrapeau(const string& drapeau) {
if (drapeau == "RENDER2D") return 1;
if (drapeau == "RENDER3D") return 2;
if (drapeau == "TEXT") return 4;
if (drapeau == "UI") return 8;
if (drapeau == "SHADOW") return 16;
if (drapeau == "POST_PROCESS") return 32;
if (drapeau == "ALL") return 4294967295ULL;
return 0;
}

struct Configuration {
string nom;
unsigned long long valeur = 0;
vector<long long> temps;
};

Configuration LireConfiguration() {
Configuration configuration;
int k;


cin >> configuration.nom >> k;

for (int i = 0; i < k; ++i) {
    string drapeau;
    cin >> drapeau;
    configuration.valeur |= ValeurDrapeau(drapeau);
}

configuration.temps.resize(10);

for (int i = 0; i < 10; ++i) {
    cin >> configuration.temps[i];
}

return configuration;


}

long long Mediane(vector<long long> temps) {
sort(temps.begin(), temps.end());
return (temps[4] + temps[5]) / 2;
}

long long Moyenne(const vector<long long>& temps) {
long long somme = 0;


for (long long temps_ms : temps) {
    somme += temps_ms;
}

return somme / 10;

}

int main() {
Configuration premiere = LireConfiguration();
Configuration seconde = LireConfiguration();


long long mediane1 = Mediane(premiere.temps);
long long mediane2 = Mediane(seconde.temps);

long long moyenne1 = Moyenne(premiere.temps);
long long moyenne2 = Moyenne(seconde.temps);

cout << premiere.nom << " VALEUR " << premiere.valeur << '\n';
cout << premiere.nom << " MEDIANE " << mediane1 << '\n';
cout << premiere.nom << " MOYENNE " << moyenne1 << '\n';

cout << seconde.nom << " VALEUR " << seconde.valeur << '\n';
cout << seconde.nom << " MEDIANE " << mediane2 << '\n';
cout << seconde.nom << " MOYENNE " << moyenne2 << '\n';

cout << "ECART MEDIANES " << mediane1 - mediane2 << '\n';
cout << "ECART MOYENNES " << moyenne1 - moyenne2 << '\n';

return 0;

}
