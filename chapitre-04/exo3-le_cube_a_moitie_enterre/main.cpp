#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

int main() {
    int N;
    cin >> N;

    int a_corriger = 0;
    long long pire = 0;

    for (int i = 0; i < N; ++i) {
        string nom;
        long long e;
        long long y;

        cin >> nom >> e >> y;

        long long demi_hauteur = e / 2;
        long long bas = y - demi_hauteur;
        long long haut = y + demi_hauteur;

        string verdict;

        if (haut <= 0) {
            verdict = "SOUS LE SOL";
        }
        else if (bas < 0) {
            verdict = "ENTERRE";
        }
        else if (bas == 0) {
            verdict = "POSE";
        }
        else {
            verdict = "FLOTTE";
        }

        long long ecart = llabs(bas);
        if (ecart > pire) {
            pire = ecart;
        }

        if (verdict != "POSE") {
            ++a_corriger;
        }

        cout << nom << " "
             << bas << " "
             << haut << " "
             << verdict << " "
             << demi_hauteur << '\n';
    }

    cout << "A CORRIGER " << a_corriger << '\n';
    cout << "PIRE " << pire << '\n';

    return 0;
}