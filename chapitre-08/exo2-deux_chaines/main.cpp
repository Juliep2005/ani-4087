#include <cstdint>
#include <iostream>
#include <string>

int main()
{
    std::uint64_t chainCount = 0;
    if (!(std::cin >> chainCount))
    {
        return 0;
    }

    std::uint64_t accepted = 0;
    std::uint64_t occupied = 0;
    std::uint64_t leftEye = 0;
    std::uint64_t rightEye = 0;
    std::uint64_t unnamed = 0;

    for (std::uint64_t index = 0; index < chainCount; ++index)
    {
        std::uint64_t width = 0;
        std::uint64_t height = 0;
        std::uint64_t imageCount = 0;
        int eye = 0;
        std::string name;
        std::cin >> width >> height >> imageCount >> eye >> name;

        const std::string displayName = name == "-" ? "SANS-NOM" : name;

        if (width == 0 || height == 0)
        {
            std::cout << "REFUSEE " << displayName << " TAILLE\n";
        }
        else if (imageCount == 0 || imageCount > 4)
        {
            std::cout << "REFUSEE " << displayName << " IMAGES\n";
        }
        else
        {
            ++accepted;
            occupied += imageCount;
            if (eye == 0)
            {
                ++leftEye;
            }
            else
            {
                ++rightEye;
            }
            if (name == "-")
            {
                ++unnamed;
            }

            std::cout << "CHAINE " << displayName << ' '
                      << width << ' ' << height << ' ' << imageCount << ' '
                      << (eye == 0 ? "GAUCHE" : "DROIT") << '\n';
        }
    }

    std::cout << "ACCEPTEES " << accepted << '\n';
    std::cout << "EMPLACEMENTS " << accepted * 4 << '\n';
    std::cout << "OCCUPES " << occupied << '\n';
    std::cout << "GAUCHE " << leftEye << '\n';
    std::cout << "DROIT " << rightEye << '\n';
    std::cout << "SANS NOM " << unnamed << '\n';

    return 0;
}
