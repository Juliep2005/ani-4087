#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

struct Target
{
    std::uint64_t width;
    std::uint64_t height;
};

int main()
{
    std::uint64_t windowWidth = 0;
    std::uint64_t windowHeight = 0;
    std::uint64_t bufferCount = 0;
    std::uint64_t orderCount = 0;

    if (!(std::cin >> windowWidth >> windowHeight >> bufferCount >> orderCount))
    {
        return 0;
    }

    std::vector<Target> targets;
    std::vector<bool> busyBuffers(static_cast<std::size_t>(bufferCount), false);

    bool offscreenEnabled = false;
    bool blocked = false;
    bool drawingToWindow = true;
    std::uint64_t currentTarget = 0;
    std::uint64_t renderWidth = windowWidth;
    std::uint64_t renderHeight = windowHeight;
    std::uint64_t frameCount = 0;
    std::uint64_t imageCount = 0;

    for (std::uint64_t order = 0; order < orderCount; ++order)
    {
        std::string command;
        std::cin >> command;

        if (blocked)
        {
            break;
        }

        if (command == "allume")
        {
            std::string subsystem;
            std::cin >> subsystem;
            if (subsystem == "OFFSCREEN")
            {
                offscreenEnabled = true;
            }
        }
        else if (command == "cible")
        {
            std::uint64_t width = 0;
            std::uint64_t height = 0;
            std::cin >> width >> height;

            if (!offscreenEnabled)
            {
                std::cout << "CIBLE NULLE\n";
            }
            else
            {
                targets.push_back({width, height});
                std::cout << "CIBLE " << targets.size() << ' '
                          << width << ' ' << height << '\n';
            }
        }
        else if (command == "vers")
        {
            std::uint64_t target = 0;
            std::cin >> target;

            if (target == 0)
            {
                drawingToWindow = true;
            }
            else if (target <= targets.size())
            {
                drawingToWindow = false;
                currentTarget = target;
            }
            else
            {
                std::cout << "VERS REFUSE\n";
            }
        }
        else if (command == "taille")
        {
            std::cin >> renderWidth >> renderHeight;
        }
        else if (command == "image")
        {
            ++frameCount;
            const std::uint64_t buffer = (frameCount - 1) % bufferCount;

            if (busyBuffers[static_cast<std::size_t>(buffer)])
            {
                std::cout << "BLOQUE " << frameCount << '\n';
                blocked = true;
                break;
            }

            ++imageCount;
            if (drawingToWindow)
            {
                std::cout << "IMAGE " << frameCount << " FENETRE\n";
            }
            else
            {
                const Target& target = targets[static_cast<std::size_t>(currentTarget - 1)];
                std::cout << "IMAGE " << frameCount << " CIBLE " << currentTarget << '\n';

                if (renderWidth > target.width || renderHeight > target.height)
                {
                    busyBuffers[static_cast<std::size_t>(buffer)] = true;
                }
            }
        }
    }

    std::cout << "IMAGES " << imageCount << '\n';
    std::cout << "ETAT " << (blocked ? "BLOQUE" : "LIBRE") << '\n';

    return 0;
}
