#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

string choisirInterface(const string& plateforme,
                        const vector<string>& interfaces,
                        int& ignorees)
{
    vector<string> ordre;

    if (plateforme == "WINDOWS") {
        ordre = {"VULKAN", "DX12", "DX11", "OPENGL", "SOFTWARE"};
    }
    else if (plateforme == "MACOS") {
        ordre = {"METAL", "OPENGL", "SOFTWARE"};
    }
    else if (plateforme == "IOS") {
        ordre = {"METAL", "SOFTWARE"};
    }
    else if (plateforme == "ANDROID") {
        ordre = {"VULKAN", "OPENGL", "SOFTWARE"};
    }
    else {
        ordre = {"VULKAN", "OPENGL", "SOFTWARE"};
    }

    for (const string& interfaceNom : interfaces) {
        if (interfaceNom == "SOFTWARE") {
            continue;
        }

        if (find(ordre.begin(), ordre.end(), interfaceNom) == ordre.end()) {
            ++ignorees;
        }
    }

    for (const string& interfaceNom : ordre) {
        if (interfaceNom == "SOFTWARE") {
            return "Software";
        }

        if (find(interfaces.begin(), interfaces.end(), interfaceNom)
            != interfaces.end()) {

            if (interfaceNom == "VULKAN") {
                return "Vulkan";
            }
            if (interfaceNom == "DX12") {
                return "DirectX 12";
            }
            if (interfaceNom == "DX11") {
                return "DirectX 11";
            }
            if (interfaceNom == "OPENGL") {
                return "OpenGL";
            }
            if (interfaceNom == "METAL") {
                return "Metal";
            }
        }
    }

    return "Software";
}

int main()
{
    int N;
    cin >> N;

    int ignorees = 0;
    int logiciel = 0;

    set<string> differentes;

    for (int i = 0; i < N; ++i) {

        string nom;
        string plateforme;
        int k;

        cin >> nom >> plateforme >> k;

        vector<string> interfaces(k);

        for (int j = 0; j < k; ++j) {
            cin >> interfaces[j];
        }

        string choix = choisirInterface(
            plateforme,
            interfaces,
            ignorees
        );

        cout << nom << " " << choix << '\n';

        if (choix == "Software") {
            ++logiciel;
        }

        differentes.insert(choix);
    }

    cout << "IGNOREES " << ignorees << '\n';
    cout << "LOGICIEL " << logiciel << '\n';
    cout << "DIFFERENTES " << differentes.size() << '\n';

    return 0;
}