#include <iostream>
#include <string>

using namespace std;

int main() {
    int N;
    cin >> N;

    int visibles = 0;
    int en_panne = 0;

    for (int i = 0; i < N; ++i) {
        string nom;
        long long drapeaux;
        long long sx, sy, sz;
        long long distance;
        long long lumieres;
        long long ambiante;
        long long proche;

        cin >> nom >> drapeaux >> sx >> sy >> sz
            >> distance >> lumieres >> ambiante >> proche;

        string verdict;

        if ((drapeaux & 2) == 0) {
            verdict = "RENDER3D ETEINT";
        }
        else if (sx == 0 || sy == 0 || sz == 0) {
            verdict = "ECHELLE NULLE";
        }
        else {
            long long face_avant = distance - sz / 2;

            if (face_avant <= 0) {
                verdict = "CAMERA DANS LE CUBE";
            }
            else if (face_avant < proche) {
                verdict = "COUPE PAR LE PLAN PROCHE";
            }
            else if (lumieres == 0 && ambiante == 0) {
                verdict = "PAS DE LUMIERE";
            }
            else {
                verdict = "VISIBLE";
            }
        }

        cout << nom << " " << verdict << '\n';

        if (verdict == "VISIBLE") {
            ++visibles;
        }
        else {
            ++en_panne;
        }
    }

    cout << "VISIBLES " << visibles << '\n';
    cout << "EN PANNE " << en_panne << '\n';

    return 0;
}