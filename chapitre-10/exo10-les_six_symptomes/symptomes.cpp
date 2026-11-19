#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::size_t count = 0;
    if (!(std::cin >> count))
    {
        return 0;
    }

    std::size_t muettes = 0;
    for (std::size_t i = 1; i <= count; ++i)
    {
        std::string fin;
        int g = 0;
        int d = 0;
        char instant = 'P';
        char pose = 'R';
        char espace = 'S';
        std::cin >> fin >> g >> d >> instant >> pose >> espace;

        std::vector<std::string> casque;
        if (fin == "JAMAIS")
        {
            casque.push_back("FIGE");
        }
        if (fin == "SI_RENDU")
        {
            casque.push_back("NE_REPART_PAS");
        }
        if (g == 0)
        {
            casque.push_back("OEIL_NOIR_G");
        }
        if (d == 0)
        {
            casque.push_back("OEIL_NOIR_D");
        }
        if (instant == 'N')
        {
            casque.push_back("TREMBLE");
        }
        if (pose == 'F')
        {
            casque.push_back("DE_TRAVERS");
        }
        if (espace == 'L')
        {
            casque.push_back("DECALE");
        }

        std::cout << i << " CASQUE";
        if (casque.empty())
        {
            std::cout << " AUCUN";
        }
        else
        {
            for (const std::string& symptom : casque)
            {
                std::cout << ' ' << symptom;
            }
        }
        std::cout << '\n';

        const bool simulatorSays = fin != "TOUJOURS" || g == 0 || d == 0;
        std::cout << i << " SIMULATEUR " << (simulatorSays ? "DIT" : "MUET") << '\n';
        if (!casque.empty() && !simulatorSays)
        {
            ++muettes;
        }
    }

    std::cout << "MUETTES " << muettes << '\n';
    return 0;
}
