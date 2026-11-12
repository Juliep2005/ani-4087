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
    bool heldWithActive = false;
    bool heldWithoutActive = false;
    bool droppedWithoutActive = false;
    std::size_t picksWithActive = 0;
    std::size_t picksWithoutActive = 0;
    std::size_t dropsWithoutActive = 0;
    std::size_t trappedImages = 0;
    std::size_t differentImages = 0;

    for (std::size_t i = 0; i < imageCount; ++i)
    {
        const char state = states[i];
        const bool active = state != 'x';
        const bool current = state == '1';
        const bool changed = current != previousCurrent;

        if (active)
        {
            if (current && changed)
            {
                heldWithActive = true;
                ++picksWithActive;
            }
            else if (!current && changed)
            {
                heldWithActive = false;
            }
        }

        if (current && changed)
        {
            if (!droppedWithoutActive)
            {
                heldWithoutActive = true;
                ++picksWithoutActive;
            }
        }
        else if (!current && changed)
        {
            if (!active)
            {
                if (heldWithoutActive)
                {
                    heldWithoutActive = false;
                    droppedWithoutActive = true;
                    ++dropsWithoutActive;
                }
            }
            else
            {
                heldWithoutActive = false;
            }
        }

        if (state == '0' && heldWithActive)
        {
            ++trappedImages;
        }
        if (heldWithActive != heldWithoutActive)
        {
            ++differentImages;
        }

        std::cout << i + 1 << ' '
                  << (heldWithActive ? "TENU" : "LIBRE") << ' '
                  << (heldWithoutActive ? "TENU" : "LIBRE") << '\n';
        previousCurrent = current;
    }

    std::cout << "PRISES AVEC " << picksWithActive << '\n';
    std::cout << "PRISES SANS " << picksWithoutActive << '\n';
    std::cout << "CHUTES SANS " << dropsWithoutActive << '\n';
    std::cout << "COINCES AVEC " << trappedImages << '\n';
    std::cout << "DIFFERENCES " << differentImages << '\n';

    return 0;
}
