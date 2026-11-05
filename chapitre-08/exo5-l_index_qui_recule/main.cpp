#include <cstdint>
#include <iostream>

int main()
{
    std::uint64_t stateCount = 0;
    if (!(std::cin >> stateCount))
    {
        return 0;
    }

    std::uint64_t rejectedCount = 0;
    std::uint64_t signedDifferenceCount = 0;
    std::uint64_t unsignedDifferenceCount = 0;

    for (std::uint64_t state = 0; state < stateCount; ++state)
    {
        std::uint64_t imageCount = 0;
        std::uint64_t releases = 0;
        std::cin >> imageCount >> releases;

        if (imageCount == 0 || imageCount > 4)
        {
            ++rejectedCount;
            std::cout << imageCount << ' ' << releases << " REFUSEE\n";
            continue;
        }

        const std::uint64_t nextImage = releases % imageCount;
        const std::uint64_t correct =
            (nextImage + imageCount - 1) % imageCount;

        const int signedNext = static_cast<int>(nextImage);
        const int signedImageCount = static_cast<int>(imageCount);
        const int signedResult = (signedNext - 1) % signedImageCount;

        const std::uint32_t unsignedNext =
            static_cast<std::uint32_t>(nextImage);
        const std::uint32_t unsignedImageCount =
            static_cast<std::uint32_t>(imageCount);
        const std::uint32_t unsignedResult =
            (unsignedNext - std::uint32_t{1}) % unsignedImageCount;

        if (signedResult != static_cast<int>(correct))
        {
            ++signedDifferenceCount;
        }
        if (unsignedResult != correct)
        {
            ++unsignedDifferenceCount;
        }

        std::cout << imageCount << ' ' << releases << ' ' << nextImage << ' '
                  << correct << ' ' << signedResult << ' ' << unsignedResult << '\n';
    }

    std::cout << "REFUSEES " << rejectedCount << '\n';
    std::cout << "FAUX SIGNE " << signedDifferenceCount << '\n';
    std::cout << "FAUX NON SIGNE " << unsignedDifferenceCount << '\n';

    return 0;
}
