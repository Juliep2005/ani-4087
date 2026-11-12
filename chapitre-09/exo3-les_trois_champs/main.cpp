#include <iostream>
#include <string>

int main()
{
    std::size_t imageCount = 0;
    if (!(std::cin >> imageCount))
    {
        return 0;
    }

    std::string states;
    if (imageCount > 0)
    {
        std::cin >> states;
    }

    bool previousCurrent = false;
    std::size_t presses = 0;
    std::size_t releases = 0;
    std::size_t falseReleases = 0;
    std::size_t heldImages = 0;

    for (std::size_t i = 0; i < imageCount; ++i)
    {
        const char state = states[i];
        const bool active = state != 'x';
        const bool current = state == '1';
        const bool changed = current != previousCurrent;

        if (current && changed)
        {
            ++presses;
        }
        if (!current && changed)
        {
            ++releases;
            if (!active)
            {
                ++falseReleases;
            }
        }
        if (current)
        {
            ++heldImages;
        }

        std::cout << i + 1 << ' ' << current << ' ' << changed << ' ' << active
                  << '\n';
        previousCurrent = current;
    }

    std::cout << "APPUIS " << presses << '\n';
    std::cout << "RELACHES " << releases << '\n';
    std::cout << "FAUX RELACHES " << falseReleases << '\n';
    std::cout << "TENUES " << heldImages << '\n';

    return 0;
}
