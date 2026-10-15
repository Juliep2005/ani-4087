#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

int main() {
    int N;
    cin >> N;

    int deplaces = 0;
    long long pire = 0;

    for (int i = 0; i < N; ++i) {
        string nom;
        long long tx, ty, tz;
        long long sx, sy, sz;

        cin >> nom >> tx >> ty >> tz >> sx >> sy >> sz;

        long long mauvais_x = sx * tx / 1000;
        long long mauvais_y = sy * ty / 1000;
        long long mauvais_z = sz * tz / 1000;

        long long ecart_x = llabs(tx - mauvais_x);
        long long ecart_y = llabs(ty - mauvais_y);
        long long ecart_z = llabs(tz - mauvais_z);

        long long ecart = ecart_x;

        if (ecart_y > ecart) {
            ecart = ecart_y;
        }

        if (ecart_z > ecart) {
            ecart = ecart_z;
        }

        cout << nom << " "
             << mauvais_x << " "
             << mauvais_y << " "
             << mauvais_z << " "
             << ecart << '\n';

        if (ecart != 0) {
            ++deplaces;
        }

        if (ecart > pire) {
            pire = ecart;
        }
    }

    cout << "DEPLACES " << deplaces << '\n';
    cout << "PIRE " << pire << '\n';

    return 0;
}
