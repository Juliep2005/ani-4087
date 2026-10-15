#include <iostream>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <iomanip>

using namespace std;

int main() {
    int N;
    cin >> N;

    const uint32_t ALL = 4294967295u;

    unordered_map<string, uint32_t> drapeaux = {
        {"RENDER2D", 1u},
        {"RENDER3D", 2u},
        {"TEXT", 4u},
        {"UI", 8u},
        {"SHADOW", 16u},
        {"POST_PROCESS", 32u},
        {"VFX", 64u},
        {"ANIMATION", 128u},
        {"OVERLAY", 256u},
        {"SIMULATION", 512u},
        {"OFFSCREEN", 1024u},
        {"RAYTRACING", 2048u},
        {"GPU_CULLING", 4096u},
        {"NONE", 0u},
        {"2D_ESSENTIALS", 1u | 4u},
        {"3D_BASE", 2u | 16u | 32u},
        {"DEBUG", 256u | 512u},
        {"ALL", ALL}
    };

    uint32_t valeur = 0;

    if (N == 0) {
        valeur = ALL;
    }
    else {
        for (int i = 0; i < N; ++i) {
            string nom;
            cin >> nom;

            auto it = drapeaux.find(nom);

            if (it == drapeaux.end()) {
                cout << "INCONNU " << nom << '\n';
            }
            else {
                valeur |= it->second;
            }
        }
    }

    cout << "VALEUR " << valeur << '\n';

    cout << "HEXA 0x"
         << uppercase
         << hex
         << setfill('0')
         << setw(8)
         << valeur
         << dec
         << '\n';

    if ((valeur & 4u) != 0 && (valeur & 1u) == 0) {
        cout << "MANQUE TEXT RENDER2D\n";
    }

    if ((valeur & 8u) != 0) {
        if ((valeur & 1u) == 0) {
            cout << "MANQUE UI RENDER2D\n";
        }

        if ((valeur & 4u) == 0) {
            cout << "MANQUE UI TEXT\n";
        }
    }

    if ((valeur & 16u) != 0 && (valeur & 2u) == 0) {
        cout << "MANQUE SHADOW RENDER3D\n";
    }

    if ((valeur & 256u) != 0) {
        if ((valeur & 1u) == 0) {
            cout << "MANQUE OVERLAY RENDER2D\n";
        }

        if ((valeur & 4u) == 0) {
            cout << "MANQUE OVERLAY TEXT\n";
        }
    }

    const uint32_t drapeaux_simples =
        1u | 2u | 4u | 8u | 16u | 32u | 64u |
        128u | 256u | 512u | 1024u | 2048u | 4096u;

    uint32_t allumes = 0;

    for (uint32_t bit = 1u; bit <= 4096u; bit <<= 1) {
        if ((valeur & bit) != 0) {
            ++allumes;
        }
    }

    (void)drapeaux_simples;

    cout << "ALLUMES " << allumes << '\n';
    cout << "ETEINTS " << 13 - allumes << '\n';

    return 0;
}
