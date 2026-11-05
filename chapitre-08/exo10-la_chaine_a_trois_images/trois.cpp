#include <cstdint>
#include <iostream>

int main()
{
    std::uint64_t scenarioCount = 0;
    if (!(std::cin >> scenarioCount))
    {
        return 0;
    }

    for (std::uint64_t scenario = 0; scenario < scenarioCount; ++scenario)
    {
        std::uint64_t imageCount = 0;
        std::uint64_t holdFrames = 0;
        std::uint64_t frameCount = 0;
        std::cin >> imageCount >> holdFrames >> frameCount;

        std::uint64_t conflictCount = 0;
        std::cout << "CIBLES " << imageCount << ' ' << holdFrames;
        for (std::uint64_t frame = 1; frame <= frameCount; ++frame)
        {
            std::cout << ' ' << (frame - 1) % imageCount;
            if (frame > imageCount && holdFrames >= imageCount)
            {
                ++conflictCount;
            }
        }
        std::cout << '\n';
        std::cout << "CONFLITS " << imageCount << ' ' << holdFrames << ' '
                  << conflictCount << '\n';
    }

    return 0;
}
