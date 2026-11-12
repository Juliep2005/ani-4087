#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Action
{
    std::string name;
    std::string type;
    std::uint32_t usage;
};

int main()
{
    const std::unordered_map<std::string, std::uint32_t> usageNumbers{
        {"SELECT", 0},
        {"GRAB", 1},
        {"MENU", 2},
        {"MOVE", 3},
        {"AIM_POSE", 4},
        {"GRIP_POSE", 5},
        {"HAPTIC", 6},
        {"SELECT_LEFT", 7},
        {"GRAB_LEFT", 8},
        {"AIM_POSE_LEFT", 9},
        {"GRIP_POSE_LEFT", 10},
        {"HAPTIC_LEFT", 11},
        {"MOVE_RIGHT", 12},
    };

    std::size_t orderCount = 0;
    if (!(std::cin >> orderCount))
    {
        return 0;
    }

    std::vector<Action> actions;
    std::unordered_set<std::string> names;
    std::size_t duplicateNames = 0;
    std::size_t attachedCount = 0;
    bool attached = false;

    for (std::size_t order = 0; order < orderCount; ++order)
    {
        std::string command;
        std::cin >> command;

        if (command == "cree")
        {
            std::string name;
            std::string type;
            std::string usage;
            std::cin >> name >> type >> usage;

            if (!names.insert(name).second)
            {
                ++duplicateNames;
            }

            actions.push_back({name, type, usageNumbers.at(usage)});
            std::cout << "ACTION " << actions.size() << ' ' << name << ' '
                      << type << ' ' << actions.back().usage << '\n';
        }
        else if (command == "attache")
        {
            if (attached)
            {
                std::cout << "ATTACHE REFUSE\n";
            }
            else
            {
                attached = true;
                attachedCount = actions.size();
                std::cout << "ATTACHE " << attachedCount << '\n';
            }
        }
        else if (command == "lit")
        {
            std::uint64_t handle = 0;
            std::cin >> handle;

            if (handle == 0 || handle > actions.size())
            {
                std::cout << "LIT " << handle << " INVALIDE\n";
            }
            else if (!attached || handle > attachedCount)
            {
                std::cout << "LIT " << handle << " INCONNUE\n";
            }
            else
            {
                std::cout << "LIT " << handle << ' '
                          << actions[static_cast<std::size_t>(handle - 1)].name
                          << '\n';
            }
        }
    }

    std::cout << "ACTIONS " << actions.size() << '\n';
    std::cout << "ATTACHEES " << attachedCount << '\n';
    std::cout << "NOMS EN DOUBLE " << duplicateNames << '\n';

    return 0;
}
