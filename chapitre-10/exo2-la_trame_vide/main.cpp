#include <iostream>
#include <string>

int main()
{
    std::size_t chainSize = 0;
    std::size_t imageCount = 0;
    if (!(std::cin >> chainSize >> imageCount))
    {
        return 0;
    }
    if (chainSize == 0 || chainSize > 4)
    {
        std::cout << "CHAINE REFUSEE\n";
        return 0;
    }

    std::string signs;
    std::cin >> signs;
    std::size_t nextImage = 0;
    std::size_t fins = 0;
    std::size_t rendues = 0;
    std::size_t sautees = 0;

    for (std::size_t image = 1; image <= imageCount; ++image)
    {
        ++fins;
        if (signs[image - 1] == '1')
        {
            std::cout << image << " OUI " << (image - 1) << ' ' << nextImage << '\n';
            ++rendues;
            nextImage = (nextImage + 1) % chainSize;
        }
        else
        {
            std::cout << image << " NON 0\n";
            ++sautees;
        }
    }

    if (imageCount != 0)
    {
        std::cout << "FINS " << fins << '\n';
    }
    else
    {
        std::cout << "FINS 0\n";
    }
    std::cout << "RENDUES " << rendues << '\n';
    std::cout << "SAUTEES " << sautees << '\n';
    std::cout << "PROCHAINE " << nextImage << '\n';
    return 0;
}
