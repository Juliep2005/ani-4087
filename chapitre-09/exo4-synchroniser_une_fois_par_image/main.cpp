#include <iostream>

int main()
{
    std::size_t imageCount = 0;
    if (!(std::cin >> imageCount))
    {
        return 0;
    }

    bool current = false;
    bool changed = false;
    bool active = false;
    bool previousPressed = false;
    std::size_t realPresses = 0;
    std::size_t grabs = 0;
    std::size_t refusedSyncs = 0;

    for (std::size_t image = 1; image <= imageCount; ++image)
    {
        int pressedInput = 0;
        std::size_t syncCount = 0;
        int focusedInput = 0;
        std::cin >> pressedInput >> syncCount >> focusedInput;

        const bool pressed = pressedInput != 0;
        const bool focused = focusedInput != 0;
        if (pressed && !previousPressed)
        {
            ++realPresses;
        }
        previousPressed = pressed;

        for (std::size_t sync = 0; sync < syncCount; ++sync)
        {
            if (!focused)
            {
                ++refusedSyncs;
                continue;
            }

            changed = pressed != current;
            current = pressed;
            active = true;
        }

        const bool didGrab = current && changed;
        if (didGrab)
        {
            ++grabs;
        }

        std::cout << image << ' ' << current << ' ' << changed << ' ' << active
                  << ' ' << (didGrab ? "SAISIT" : "-") << '\n';
    }

    std::cout << "APPUIS REELS " << realPresses << '\n';
    std::cout << "SAISIES " << grabs << '\n';
    std::cout << "SYNC REFUSEES " << refusedSyncs << '\n';

    return 0;
}
