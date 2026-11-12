#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

struct Vector3
{
    std::int64_t x;
    std::int64_t y;
    std::int64_t z;
};

struct HeadPose
{
    Vector3 position;
    int yaw;
};

Vector3 transformOffset(const HeadPose& head, Vector3 offset)
{
    std::int64_t x = offset.x;
    std::int64_t z = offset.z;

    switch (head.yaw)
    {
    case 1:
        offset.x = z;
        offset.z = -x;
        break;
    case 2:
        offset.x = -x;
        offset.z = -z;
        break;
    case 3:
        offset.x = -z;
        offset.z = x;
        break;
    default:
        break;
    }

    return {
        head.position.x + offset.x,
        head.position.y + offset.y,
        head.position.z + offset.z,
    };
}

std::size_t closestObject(
    const Vector3& handPose,
    const std::vector<Vector3>& objects)
{
    const long double maxDistanceSquared = 22500.0L;
    long double closestDistanceSquared = maxDistanceSquared;
    std::size_t closest = objects.size();

    for (std::size_t i = 0; i < objects.size(); ++i)
    {
        const long double dx = static_cast<long double>(objects[i].x) -
                              static_cast<long double>(handPose.x);
        const long double dy = static_cast<long double>(objects[i].y) -
                              static_cast<long double>(handPose.y);
        const long double dz = static_cast<long double>(objects[i].z) -
                              static_cast<long double>(handPose.z);
        const long double distanceSquared = dx * dx + dy * dy + dz * dz;

        if (distanceSquared <= maxDistanceSquared &&
            (closest == objects.size() ||
             distanceSquared < closestDistanceSquared))
        {
            closest = i;
            closestDistanceSquared = distanceSquared;
        }
    }

    return closest;
}

int main()
{
    std::size_t headCount = 0;
    std::size_t objectCount = 0;
    if (!(std::cin >> headCount >> objectCount))
    {
        return 0;
    }

    std::vector<HeadPose> heads(headCount);
    for (HeadPose& head : heads)
    {
        std::cin >> head.position.x >> head.position.y >> head.position.z >>
            head.yaw;
    }

    std::vector<Vector3> objects(objectCount);
    for (Vector3& object : objects)
    {
        std::cin >> object.x >> object.y >> object.z;
    }

    for (std::size_t i = 0; i < heads.size(); ++i)
    {
        const HeadPose& head = heads[i];
        const Vector3 palmRight =
            transformOffset(head, {180, -300, -250});
        const Vector3 aimRight =
            transformOffset(head, {180, -250, -400});
        const Vector3 palmLeft =
            transformOffset(head, {-180, -300, -250});
        const Vector3 aimLeft =
            transformOffset(head, {-180, -250, -400});

        std::cout << "TETE " << i + 1 << '\n';
        std::cout << "PAUME D " << palmRight.x << ' ' << palmRight.y << ' '
                  << palmRight.z << '\n';
        std::cout << "VISEE D " << aimRight.x << ' ' << aimRight.y << ' '
                  << aimRight.z << '\n';
        std::cout << "PAUME G " << palmLeft.x << ' ' << palmLeft.y << ' '
                  << palmLeft.z << '\n';
        std::cout << "VISEE G " << aimLeft.x << ' ' << aimLeft.y << ' '
                  << aimLeft.z << '\n';

        const std::size_t palmObject = closestObject(palmRight, objects);
        if (palmObject == objects.size())
        {
            std::cout << "SAISIE PAUME AUCUNE\n";
        }
        else
        {
            std::cout << "SAISIE PAUME " << palmObject + 1 << '\n';
        }

        const std::size_t aimObject = closestObject(aimRight, objects);
        if (aimObject == objects.size())
        {
            std::cout << "SAISIE VISEE AUCUNE\n";
        }
        else
        {
            std::cout << "SAISIE VISEE " << aimObject + 1 << '\n';
        }
    }

    const long double dy = 50.0L;
    const long double dz = 150.0L;
    const auto distance = static_cast<std::int64_t>(
        std::llround(std::sqrt(dy * dy + dz * dz)));
    std::cout << "ECART " << distance << '\n';

    return 0;
}
