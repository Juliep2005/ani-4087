#include <cstdint>
#include <cstddef>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

int main()
{
    std::uint64_t imageCount = 0;
    std::uint64_t orderCount = 0;
    if (!(std::cin >> imageCount >> orderCount))
    {
        return 0;
    }

    std::vector<std::uint64_t> images(
        static_cast<std::size_t>(imageCount), 0);
    std::unordered_set<std::uint64_t> textures;
    std::uint64_t deadCount = 0;
    std::uint64_t largeSetCount = 0;
    constexpr std::uint64_t maxUint32 = 4294967295ULL;

    for (std::uint64_t order = 0; order < orderCount; ++order)
    {
        std::string command;
        std::cin >> command;

        if (command == "cree")
        {
            std::uint64_t handle = 0;
            std::cin >> handle;
            textures.insert(handle);
        }
        else if (command == "detruit")
        {
            std::uint64_t handle = 0;
            std::cin >> handle;
            textures.erase(handle);
        }
        else if (command == "set")
        {
            std::uint64_t index = 0;
            std::uint64_t handle = 0;
            std::cin >> index >> handle;

            if (index >= imageCount)
            {
                std::cout << "SET IGNORE " << index << '\n';
            }
            else
            {
                images[static_cast<std::size_t>(index)] = handle;
                if (handle > maxUint32)
                {
                    ++largeSetCount;
                }
            }
        }
        else if (command == "get")
        {
            std::uint64_t index = 0;
            std::cin >> index;
            const std::uint64_t handle =
                index < imageCount ? images[static_cast<std::size_t>(index)] : 0;

            if (handle == 0)
            {
                std::cout << "GET " << index << " 0 VIDE\n";
            }
            else if (textures.find(handle) != textures.end())
            {
                std::cout << "GET " << index << ' ' << handle << " VIVANTE\n";
            }
            else
            {
                ++deadCount;
                std::cout << "GET " << index << ' ' << handle << " MORTE\n";
            }
        }
    }

    std::cout << "MORTES " << deadCount << '\n';
    std::cout << "LARGES " << largeSetCount << '\n';

    return 0;
}
