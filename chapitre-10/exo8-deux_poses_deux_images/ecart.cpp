#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <iostream>

int main()
{
    std::size_t count = 0;
    std::int64_t d = 0;
    if (!(std::cin >> count >> d))
    {
        return 0;
    }

    std::int64_t positionSum = 0;
    std::int64_t angleSum = 0;
    for (std::size_t i = 1; i <= count; ++i)
    {
        std::int64_t x1 = 0;
        std::int64_t z1 = 0;
        std::int64_t a1 = 0;
        std::int64_t x2 = 0;
        std::int64_t z2 = 0;
        std::int64_t a2 = 0;
        std::cin >> x1 >> z1 >> a1 >> x2 >> z2 >> a2;

        const std::int64_t dx = x2 - x1;
        const std::int64_t dz = z2 - z1;
        const long double position = std::sqrt(
            static_cast<long double>(dx) * dx +
            static_cast<long double>(dz) * dz);
        const std::int64_t roundedPosition =
            static_cast<std::int64_t>(position + 0.5L);
        const std::int64_t angle = std::llabs(a2 - a1);

        std::cout << i << " POSITION " << roundedPosition
                  << " ANGLE " << angle << '\n';
        positionSum += roundedPosition;
        angleSum += angle;
    }

    if (count == 0 || d == 0)
    {
        std::cout << "AUCUNE\n";
        return 0;
    }

    const std::int64_t meanPosition = positionSum / count;
    const std::int64_t meanAngle = angleSum / count;
    const std::int64_t speed = angleSum / (count * d);

    std::cout << "MOYENNE POSITION " << meanPosition << '\n';
    std::cout << "MOYENNE ANGLE " << meanAngle << '\n';
    std::cout << "VITESSE " << speed << '\n';
    return 0;
}
