#include <iostream>
#include <string>

using namespace std;

struct Mur {
    string nom;
    long long xmin;
    long long xmax;
    long long zmin;
    long long zmax;
};

bool contient(const Mur& mur,
              long long xmin,
              long long xmax,
              long long zmin,
              long long zmax) {
    return mur.xmin <= xmin &&
           mur.xmax >= xmax &&
           mur.zmin <= zmin &&
           mur.zmax >= zmax;
}

int main() {
    long long L, e;
    cin >> L >> e;

    int N;
    cin >> N;

    Mur murs[100];

    for (int i = 0; i < N; ++i) {
        string nom;
        long long cx, cz, sx, sz;

        cin >> nom >> cx >> cz >> sx >> sz;

        murs[i].nom = nom;
        murs[i].xmin = cx - sx / 2;
        murs[i].xmax = cx + sx / 2;
        murs[i].zmin = cz - sz / 2;
        murs[i].zmax = cz + sz / 2;

        cout << murs[i].nom << " "
             << murs[i].xmin << " "
             << murs[i].xmax << " "
             << murs[i].zmin << " "
             << murs[i].zmax << '\n';
    }

    long long h = L / 2;

    long long angle_xmin[4] = {
        -h - e,
        h,
        -h - e,
        h
    };

    long long angle_xmax[4] = {
        -h,
        h + e,
        -h,
        h + e
    };

    long long angle_zmin[4] = {
        -h - e,
        -h - e,
        h,
        h
    };

    long long angle_zmax[4] = {
        -h,
        -h,
        h + e,
        h + e
    };

    string noms_angles[4] = {
        "FOND_GAUCHE",
        "FOND_DROIT",
        "ENTREE_GAUCHE",
        "ENTREE_DROIT"
    };

    int trous = 0;

    for (int i = 0; i < 4; ++i) {
        int murs_contenants = 0;

        for (int j = 0; j < N; ++j) {
            if (contient(murs[j],
                         angle_xmin[i],
                         angle_xmax[i],
                         angle_zmin[i],
                         angle_zmax[i])) {
                ++murs_contenants;
            }
        }

        if (murs_contenants == 1) {
            cout << noms_angles[i] << " BOUCHE\n";
        }
        else {
            cout << noms_angles[i] << " TROU\n";
            ++trous;
        }
    }

    cout << "TROUS " << trous << '\n';

    return 0;
}
