#include <cstdint>
#include <cstddef>
#include <iostream>
#include <string>

int main()
{
    std::uint64_t imageCount = 0;
    std::uint32_t initialValue = 0;
    std::uint64_t orderCount = 0;
    if (!(std::cin >> imageCount >> initialValue >> orderCount))
    {
        return 0;
    }

    std::uint32_t variables[26];
    for (std::uint32_t& value : variables)
    {
        value = initialValue;
    }

    std::uint64_t nextImage = 0;
    bool borrowed = false;

    for (std::uint64_t order = 0; order < orderCount; ++order)
    {
        std::string command;
        std::cin >> command;

        if (command == "acquire")
        {
            char variable = '\0';
            std::cin >> variable;
            std::uint32_t& value = variables[static_cast<std::size_t>(variable - 'a')];

            if (borrowed)
            {
                std::cout << "ACQUIRE FAUX " << variable << ' ' << value << '\n';
            }
            else
            {
                value = static_cast<std::uint32_t>(nextImage);
                borrowed = true;
                std::cout << "ACQUIRE VRAI " << variable << ' ' << value << '\n';
            }
        }
        else if (command == "release")
        {
            if (borrowed)
            {
                nextImage = (nextImage + 1) % imageCount;
                borrowed = false;
                std::cout << "RELEASE VRAI\n";
            }
            else
            {
                std::cout << "RELEASE FAUX\n";
            }
        }
        else if (command == "last")
        {
            const std::uint64_t lastImage =
                (nextImage + imageCount - 1) % imageCount;
            std::cout << "LAST " << lastImage << '\n';
        }
    }

    std::cout << "EMPRUNTEE " << (borrowed ? "OUI" : "NON") << '\n';
    std::cout << "PROCHAINE " << nextImage << '\n';

    return 0;
}
