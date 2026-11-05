#include <cstdint>
#include <iostream>
#include <string>

int main()
{
    std::uint64_t orderCount = 0;
    if (!(std::cin >> orderCount))
    {
        return 0;
    }

    std::uint64_t currentTarget = 0;
    std::uint64_t storedHandle = 0;
    std::uint64_t nextTarget = 101;
    std::uint64_t imageCount = 0;
    std::uint64_t displayedCount = 0;
    std::uint64_t blackCount = 0;
    std::uint64_t expiredCount = 0;

    for (std::uint64_t order = 0; order < orderCount; ++order)
    {
        std::string command;
        std::cin >> command;

        if (command == "cible")
        {
            std::uint64_t width = 0;
            std::uint64_t height = 0;
            std::cin >> width >> height;
            currentTarget = nextTarget++;
            std::cout << "CIBLE " << currentTarget << ' '
                      << width << ' ' << height << '\n';
        }
        else if (command == "chaine")
        {
            storedHandle = 0;
        }
        else if (command == "enregistre")
        {
            storedHandle = currentTarget;
        }
        else if (command == "image")
        {
            ++imageCount;
            if (storedHandle == 0)
            {
                ++blackCount;
                std::cout << "IMAGE " << imageCount << " NOIR\n";
            }
            else if (storedHandle == currentTarget)
            {
                ++displayedCount;
                std::cout << "IMAGE " << imageCount
                          << " AFFICHE " << storedHandle << '\n';
            }
            else
            {
                ++expiredCount;
                std::cout << "IMAGE " << imageCount
                          << " PERIMEE " << storedHandle << '\n';
            }
        }
    }

    std::cout << "AFFICHEES " << displayedCount << '\n';
    std::cout << "NOIRES " << blackCount << '\n';
    std::cout << "PERIMEES " << expiredCount << '\n';

    return 0;
}
