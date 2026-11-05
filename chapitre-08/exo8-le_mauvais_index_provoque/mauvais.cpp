#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::uint64_t imageCount = 0;
    std::uint64_t frameCount = 0;
    std::int64_t startPosition = 0;
    std::int64_t step = 0;
    if (!(std::cin >> imageCount >> frameCount >> startPosition >> step))
    {
        return 0;
    }

    std::string modes;
    std::cin >> modes;

    std::vector<std::uint64_t> lastPaintedFrame(
        static_cast<std::size_t>(imageCount), 0);
    std::uint64_t nextImage = 0;
    bool previousFrameShown = false;
    std::int64_t previousShownPosition = 0;

    for (std::uint64_t frame = 1; frame <= frameCount; ++frame)
    {
        const std::uint64_t borrowedImage = nextImage;
        lastPaintedFrame[static_cast<std::size_t>(borrowedImage)] = frame;

        const char mode = modes[static_cast<std::size_t>(frame - 1)];
        std::uint64_t submittedImage = borrowedImage;
        if (mode == 'P')
        {
            submittedImage = (nextImage + imageCount - 1) % imageCount;
        }

        nextImage = (nextImage + 1) % imageCount;
        if (mode == 'D')
        {
            submittedImage = (nextImage + imageCount - 1) % imageCount;
        }

        const std::uint64_t shownFrame =
            lastPaintedFrame[static_cast<std::size_t>(submittedImage)];
        if (shownFrame == 0)
        {
            std::cout << frame << " NOIR -\n";
            previousFrameShown = false;
            continue;
        }

        const std::int64_t shownPosition =
            startPosition + step * static_cast<std::int64_t>(shownFrame);
        std::cout << frame << ' ' << shownPosition << ' ';
        if (!previousFrameShown)
        {
            std::cout << "-\n";
        }
        else
        {
            std::cout << shownPosition - previousShownPosition << '\n';
        }

        previousFrameShown = true;
        previousShownPosition = shownPosition;
    }

    return 0;
}
