#include <cstdint>
#include <iostream>
#include <string>

int main()
{
    std::uint64_t imageCount = 0;
    std::uint64_t frameCount = 0;
    if (!(std::cin >> imageCount >> frameCount))
    {
        return 0;
    }

    std::uint64_t successfulAcquires = 0;
    std::uint64_t refusedAcquires = 0;
    std::uint64_t releases = 0;
    std::uint64_t firstLeak = 0;
    bool borrowed = false;

    for (std::uint64_t frame = 1; frame <= frameCount; ++frame)
    {
        std::string action;
        std::cin >> action;

        if (borrowed)
        {
            ++refusedAcquires;
            std::cout << frame << " BLOQUEE\n";
            continue;
        }

        ++successfulAcquires;
        borrowed = true;

        if (action == "sortie")
        {
            if (firstLeak == 0)
            {
                firstLeak = frame;
            }
            std::cout << frame << " FUITE\n";
        }
        else if (action == "double")
        {
            ++refusedAcquires;
            ++releases;
            borrowed = false;
            std::cout << frame << " DOUBLE\n";
        }
        else
        {
            ++releases;
            borrowed = false;
            std::cout << frame << " OK\n";
        }
    }

    std::cout << "REUSSIES " << successfulAcquires << '\n';
    std::cout << "REFUSEES " << refusedAcquires << '\n';
    std::cout << "LIBERATIONS " << releases << '\n';
    std::cout << "ECART " << successfulAcquires - releases << '\n';
    if (firstLeak == 0)
    {
        std::cout << "PREMIERE FUITE AUCUNE\n";
    }
    else
    {
        std::cout << "PREMIERE FUITE " << firstLeak << '\n';
    }

    return 0;
}
