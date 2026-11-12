#include <cstdint>
#include <iostream>
#include <vector>

struct Pose
{
    std::int64_t x;
    std::int64_t y;
    int k;
};

struct Vector2
{
    std::int64_t x;
    std::int64_t y;
};

Vector2 rotate(Vector2 point, int quarterTurns)
{
    switch (quarterTurns)
    {
    case 1:
        return {-point.y, point.x};
    case 2:
        return {-point.x, -point.y};
    case 3:
        return {point.y, -point.x};
    default:
        return point;
    }
}

Pose compose(const Pose& first, const Pose& second)
{
    const Vector2 offset = rotate({second.x, second.y}, first.k);
    return {
        first.x + offset.x,
        first.y + offset.y,
        (first.k + second.k) % 4,
    };
}

Pose inverse(const Pose& pose)
{
    const int inverseAngle = (4 - pose.k) % 4;
    const Vector2 position = rotate({-pose.x, -pose.y}, inverseAngle);
    return {position.x, position.y, inverseAngle};
}

int main()
{
    std::size_t objectCount = 0;
    std::size_t imageCount = 0;
    if (!(std::cin >> objectCount >> imageCount))
    {
        return 0;
    }

    std::vector<Pose> objects(objectCount);
    for (Pose& object : objects)
    {
        std::cin >> object.x >> object.y >> object.k;
    }

    const std::size_t noObject = objectCount;
    std::size_t heldObject = noObject;
    Pose attachment{0, 0, 0};
    bool previousPressed = false;
    std::size_t successfulGrabs = 0;
    std::size_t missedGrabs = 0;

    for (std::size_t image = 1; image <= imageCount; ++image)
    {
        Pose palm{0, 0, 0};
        int pressedInput = 0;
        std::cin >> palm.x >> palm.y >> palm.k >> pressedInput;

        const bool pressed = pressedInput != 0;
        const bool justPressed = pressed && !previousPressed;
        const bool justReleased = !pressed && previousPressed;
        bool missedThisImage = false;

        if (justReleased)
        {
            heldObject = noObject;
        }

        if (justPressed && heldObject == noObject)
        {
            bool foundObject = false;
            long double closestDistanceSquared = 22500.0L;
            std::size_t closestObject = noObject;

            for (std::size_t i = 0; i < objects.size(); ++i)
            {
                const long double dx =
                    static_cast<long double>(objects[i].x) -
                    static_cast<long double>(palm.x);
                const long double dy =
                    static_cast<long double>(objects[i].y) -
                    static_cast<long double>(palm.y);
                const long double distanceSquared = dx * dx + dy * dy;

                if (distanceSquared <= 22500.0L &&
                    (!foundObject || distanceSquared < closestDistanceSquared))
                {
                    foundObject = true;
                    closestDistanceSquared = distanceSquared;
                    closestObject = i;
                }
            }

            if (foundObject)
            {
                heldObject = closestObject;
                attachment = compose(inverse(palm), objects[heldObject]);
                ++successfulGrabs;
            }
            else
            {
                ++missedGrabs;
                missedThisImage = true;
            }
        }

        if (heldObject != noObject)
        {
            objects[heldObject] = compose(palm, attachment);
            std::cout << image << " TIENT " << heldObject + 1 << ' '
                      << objects[heldObject].x << ' '
                      << objects[heldObject].y << ' '
                      << objects[heldObject].k << '\n';
        }
        else if (missedThisImage)
        {
            std::cout << image << " RATE\n";
        }
        else
        {
            std::cout << image << " LIBRE\n";
        }

        previousPressed = pressed;
    }

    std::cout << "SAISIES " << successfulGrabs << '\n';
    std::cout << "RATEES " << missedGrabs << '\n';
    for (std::size_t i = 0; i < objects.size(); ++i)
    {
        std::cout << "OBJET " << i + 1 << ' ' << objects[i].x << ' '
                  << objects[i].y << ' ' << objects[i].k << '\n';
    }

    return 0;
}
