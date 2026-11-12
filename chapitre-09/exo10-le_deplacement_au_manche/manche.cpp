#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_set>

int main()
{
    std::int64_t speed = 0;
    std::int64_t imageCount = 0;
    if (!(std::cin >> speed >> imageCount) || imageCount < 0)
    {
        return 0;
    }

    const int forwardX[4] = {0, -1, 0, 1};
    const int forwardZ[4] = {-1, 0, 1, 0};
    const int rightX[4] = {1, 0, -1, 0};
    const int rightZ[4] = {0, -1, 0, 1};

    std::int64_t offsetX = 0;
    std::int64_t offsetZ = 0;
    std::int64_t diagonalImages = 0;

    for (std::int64_t image = 1; image <= imageCount; ++image)
    {
        int yawInput = 0;
        std::size_t arrowCount = 0;
        std::cin >> yawInput >> arrowCount;

        std::unordered_set<std::string> heldArrows;
        for (std::size_t i = 0; i < arrowCount; ++i)
        {
            std::string arrow;
            std::cin >> arrow;
            heldArrows.insert(arrow);
        }

        const int yaw = ((yawInput % 4) + 4) % 4;
        int x = 0;
        int y = 0;
        if (heldArrows.count("F_DROITE") != 0)
        {
            ++x;
        }
        if (heldArrows.count("F_GAUCHE") != 0)
        {
            --x;
        }
        if (heldArrows.count("F_HAUT") != 0)
        {
            ++y;
        }
        if (heldArrows.count("F_BAS") != 0)
        {
            --y;
        }

        if (x != 0 && y != 0)
        {
            ++diagonalImages;
        }

        offsetX += speed * (y * forwardX[yaw] + x * rightX[yaw]);
        offsetZ += speed * (y * forwardZ[yaw] + x * rightZ[yaw]);

        std::cout << image << ' ' << x << ',' << y << ' ' << offsetX << ' '
                  << offsetZ << '\n';
    }

    std::cout << "DECALAGE " << offsetX << ' ' << offsetZ << '\n';
    std::cout << "DIAGONALES " << diagonalImages << '\n';

    return 0;
}
