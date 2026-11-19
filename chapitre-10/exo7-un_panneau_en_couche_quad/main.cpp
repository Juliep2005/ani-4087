#include <cmath>
#include <cstdint>
#include <iostream>

struct Point
{
    std::int64_t x;
    std::int64_t y;
    std::int64_t z;
};

Point rotateXZ(std::int64_t x, std::int64_t z, int yaw)
{
    switch (yaw)
    {
    case 1: return {z, 0, -x};
    case 2: return {-x, 0, -z};
    case 3: return {-z, 0, x};
    default: return {x, 0, z};
    }
}

int main()
{
    Point anchor{};
    int anchorYaw = 0;
    if (!(std::cin >> anchor.x >> anchor.y >> anchor.z >> anchorYaw))
    {
        return 0;
    }
    Point eye{};
    std::cin >> eye.x >> eye.y >> eye.z;

    std::size_t count = 0;
    std::cin >> count;
    for (std::size_t i = 1; i <= count; ++i)
    {
        char space;
        std::int64_t cx = 0;
        std::int64_t cy = 0;
        std::int64_t cz = 0;
        int yaw = 0;
        std::int64_t width = 0;
        std::int64_t height = 0;
        std::cin >> space >> cx >> cy >> cz >> yaw >> width >> height;

        const bool local = (space == '-' || space == 'L');
        Point center{cx, cy, cz};
        int centerYaw = yaw;
        if (local)
        {
            Point rotated = rotateXZ(cx, cz, anchorYaw);
            center = {anchor.x + rotated.x, anchor.y + cy,
                      anchor.z + rotated.z};
            centerYaw = (anchorYaw + yaw) % 4;
        }

        std::cout << "PANNEAU " << i << ' ' << (local ? "LOCAL" : "STAGE") << '\n';
        const std::int64_t halfWidth = width / 2;
        const std::int64_t halfHeight = height / 2;
        const Point offsets[4] = {
            {-halfWidth, -halfHeight, 0},
            {halfWidth, -halfHeight, 0},
            {halfWidth, halfHeight, 0},
            {-halfWidth, halfHeight, 0}};

        for (const Point& offset : offsets)
        {
            Point rotated = rotateXZ(offset.x, offset.z, centerYaw);
            const std::int64_t x = center.x + rotated.x;
            const std::int64_t y = center.y + offset.y;
            const std::int64_t z = center.z + rotated.z;
            std::cout << x << ' ' << y << ' ' << z << '\n';
        }

        const long double dx = static_cast<long double>(center.x - eye.x);
        const long double dy = static_cast<long double>(center.y - eye.y);
        const long double dz = static_cast<long double>(center.z - eye.z);
        const long double distance = std::sqrt(dx * dx + dy * dy + dz * dz);
        std::cout << "DISTANCE " << static_cast<std::int64_t>(distance + 0.5L) << '\n';
    }

    std::cout << "PANNEAUX " << count << '\n';
    return 0;
}
