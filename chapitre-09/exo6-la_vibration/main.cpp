#include <algorithm>
#include <cstdint>
#include <iostream>

int main()
{
    std::size_t callCount = 0;
    if (!(std::cin >> callCount))
    {
        return 0;
    }

    std::size_t goodCalls = 0;
    std::size_t longCalls = 0;
    std::size_t silentCalls = 0;
    std::int64_t totalDuration = 0;

    for (std::size_t call = 1; call <= callCount; ++call)
    {
        std::int64_t amplitude = 0;
        std::int64_t duration = 0;
        std::int64_t frequency = 0;
        std::cin >> amplitude >> duration >> frequency;

        amplitude = std::clamp(amplitude, std::int64_t{0}, std::int64_t{100});

        const bool minimumDuration = duration <= 0;
        const bool defaultFrequency = frequency <= 0;
        if (minimumDuration)
        {
            duration = 0;
        }
        else
        {
            totalDuration += duration;
        }

        const char* verdict = nullptr;
        if (amplitude == 0)
        {
            verdict = "MUETTE";
            ++silentCalls;
        }
        else if (minimumDuration || duration < 50)
        {
            verdict = "BREVE";
        }
        else if (duration <= 200)
        {
            verdict = "BONNE";
            ++goodCalls;
        }
        else
        {
            verdict = "LONGUE";
            ++longCalls;
        }

        std::cout << call << ' ' << amplitude << ' ';
        if (minimumDuration)
        {
            std::cout << "MIN";
        }
        else
        {
            std::cout << duration;
        }
        std::cout << ' ';
        if (defaultFrequency)
        {
            std::cout << "DEFAUT";
        }
        else
        {
            std::cout << frequency;
        }
        std::cout << ' ' << verdict << '\n';
    }

    std::cout << "BONNES " << goodCalls << '\n';
    std::cout << "LONGUES " << longCalls << '\n';
    std::cout << "MUETTES " << silentCalls << '\n';
    std::cout << "TEMPS " << totalDuration << '\n';

    return 0;
}
