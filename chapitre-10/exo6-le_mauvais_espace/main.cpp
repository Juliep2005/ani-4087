#include <cstdint>
#include <iostream>

struct Pose
{
    char demand;
    char declared;
    std::int64_t x;
    std::int64_t y;
    std::int64_t z;
    int yaw;
};

Pose rotateAndCompose(const Pose& pose, const Pose& anchor)
{
    std::int64_t rx = pose.x;
    std::int64_t rz = pose.z;
    switch (pose.yaw)
    {
    case 1: rx = pose.z; rz = -pose.x; break;
    case 2: rx = -pose.x; rz = -pose.z; break;
    case 3: rx = -pose.z; rz = pose.x; break;
    }
    return {'L', 'S',
            anchor.x + rx,
            anchor.y + pose.y,
            anchor.z + rz,
            (anchor.yaw + pose.yaw) % 4};
}

int main()
{
    Pose anchor{};
    if (!(std::cin >> anchor.x >> anchor.y >> anchor.z >> anchor.yaw))
    {
        return 0;
    }
    anchor.demand = 'S';
    anchor.declared = 'S';

    std::size_t count = 0;
    std::cin >> count;
    std::size_t justes = 0;
    std::size_t fausses = 0;

    for (std::size_t i = 1; i <= count; ++i)
    {
        Pose pose{};
        std::cin >> pose.demand >> pose.declared
                 >> pose.x >> pose.y >> pose.z >> pose.yaw;

        Pose truePose = pose;
        if (pose.demand == 'L')
        {
            truePose = rotateAndCompose(pose, anchor);
        }
        else
        {
            truePose.declared = 'S';
        }

        Pose crude = pose;
        if (pose.declared == 'L')
        {
            crude = rotateAndCompose(pose, anchor);
        }
        else
        {
            crude.declared = 'S';
        }

        const std::int64_t dx = crude.x - truePose.x;
        const std::int64_t dy = crude.y - truePose.y;
        const std::int64_t dz = crude.z - truePose.z;
        const int dk = (crude.yaw - truePose.yaw + 4) % 4;

        std::cout << i << " CRU " << crude.x << ' ' << crude.y << ' '
                  << crude.z << ' ' << crude.yaw << '\n';
        std::cout << i << " ECART " << dx << ' ' << dy << ' ' << dz
                  << ' ' << dk << '\n';

        if (dx == 0 && dy == 0 && dz == 0 && dk == 0)
        {
            ++justes;
        }
        else
        {
            ++fausses;
        }
    }

    std::cout << "JUSTES " << justes << '\n';
    std::cout << "FAUSSES " << fausses << '\n';
    return 0;
}
