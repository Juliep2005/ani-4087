#include <iostream>
#include <string>

using namespace std;

struct Position {
    long long x;
    long long y;
    long long z;
};

Position PoserAuSol(long long x, long long z, long long sy) {
    Position position;
    position.x = x;
    position.y = sy / 2;
    position.z = z;
    return position;
}

Position PoserSurTable(long long x, long long z, long long sy, long long H) {
    Position position;
    position.x = x;
    position.y = H + sy / 2;
    position.z = z;
    return position;
}

int main() {
    long long L, P, H, ep, pied, tx, tz;
    cin >> L >> P >> H >> ep >> pied >> tx >> tz;

    long long plateau_y = H - ep / 2;

    cout << "PLATEAU "
         << tx << " "
         << plateau_y << " "
         << tz << '\n';

    long long dx = L / 2 - pied;
    long long dz = P / 2 - pied;
    long long hauteur_pied = H - ep;

    Position pied1 = PoserAuSol(tx - dx, tz - dz, hauteur_pied);
    Position pied2 = PoserAuSol(tx + dx, tz - dz, hauteur_pied);
    Position pied3 = PoserAuSol(tx - dx, tz + dz, hauteur_pied);
    Position pied4 = PoserAuSol(tx + dx, tz + dz, hauteur_pied);

    cout << "PIED "
         << pied1.x << " "
         << pied1.y << " "
         << pied1.z << '\n';

    cout << "PIED "
         << pied2.x << " "
         << pied2.y << " "
         << pied2.z << '\n';

    cout << "PIED "
         << pied3.x << " "
         << pied3.y << " "
         << pied3.z << '\n';

    cout << "PIED "
         << pied4.x << " "
         << pied4.y << " "
         << pied4.z << '\n';

    int N;
    cin >> N;

    for (int i = 0; i < N; ++i) {
        string nom;
        long long sx, sy, sz, x, z;
        string ou;

        cin >> nom >> sx >> sy >> sz >> x >> z >> ou;

        Position position;

        if (ou == "SOL") {
            position = PoserAuSol(x, z, sy);
        }
        else {
            position = PoserSurTable(x, z, sy, H);
        }

        cout << nom << " "
             << position.x << " "
             << position.y << " "
             << position.z << '\n';
    }

    return 0;
}