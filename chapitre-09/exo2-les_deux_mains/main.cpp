#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

struct Action
{
    std::string name;
    std::string usage;
    std::string hand;
    std::string command;
};

int heldValue(const std::unordered_set<std::string>& held, const std::string& key)
{
    return held.count(key) != 0 ? 1 : 0;
}

std::string vectorValue(
    const std::unordered_set<std::string>& held,
    const std::string& usage)
{
    int x = 0;
    int y = 0;

    if (usage == "MOVE")
    {
        x = heldValue(held, "F_DROITE") - heldValue(held, "F_GAUCHE");
        y = heldValue(held, "F_HAUT") - heldValue(held, "F_BAS");
    }
    else
    {
        x = heldValue(held, "L") - heldValue(held, "J");
        y = heldValue(held, "I") - heldValue(held, "K");
    }

    return std::to_string(x) + ',' + std::to_string(y);
}

int main()
{
    const std::unordered_map<std::string, std::pair<std::string, std::string>>
        bindings{
            {"SELECT", {"DROITE", "SOURIS_G"}},
            {"GRAB", {"DROITE", "SOURIS_D"}},
            {"MENU", {"GAUCHE", "TAB"}},
            {"MOVE", {"GAUCHE", "FLECHES"}},
            {"AIM_POSE", {"DROITE", "AUCUNE"}},
            {"GRIP_POSE", {"DROITE", "AUCUNE"}},
            {"HAPTIC", {"DROITE", "AUCUNE"}},
            {"SELECT_LEFT", {"GAUCHE", "SOURIS_M"}},
            {"GRAB_LEFT", {"GAUCHE", "MAJ_G"}},
            {"AIM_POSE_LEFT", {"GAUCHE", "AUCUNE"}},
            {"GRIP_POSE_LEFT", {"GAUCHE", "AUCUNE"}},
            {"HAPTIC_LEFT", {"GAUCHE", "AUCUNE"}},
            {"MOVE_RIGHT", {"DROITE", "IJKL"}},
        };

    std::size_t actionCount = 0;
    std::size_t imageCount = 0;
    if (!(std::cin >> actionCount >> imageCount))
    {
        return 0;
    }

    std::vector<Action> actions;
    actions.reserve(actionCount);
    std::size_t rightActions = 0;
    std::size_t leftActions = 0;

    for (std::size_t i = 0; i < actionCount; ++i)
    {
        std::string name;
        std::string usage;
        std::cin >> name >> usage;

        const auto& binding = bindings.at(usage);
        actions.push_back({name, usage, binding.first, binding.second});
        if (binding.first == "DROITE")
        {
            ++rightActions;
        }
        else
        {
            ++leftActions;
        }
    }

    for (const Action& action : actions)
    {
        std::cout << action.name << ' ' << action.hand << ' '
                  << action.command << '\n';
    }

    std::size_t headImages = 0;
    for (std::size_t image = 1; image <= imageCount; ++image)
    {
        std::size_t commandCount = 0;
        std::cin >> commandCount;

        std::unordered_set<std::string> held;
        for (std::size_t i = 0; i < commandCount; ++i)
        {
            std::string command;
            std::cin >> command;
            held.insert(command);
        }

        if (held.count("Z") != 0 || held.count("Q") != 0 ||
            held.count("S") != 0 || held.count("D") != 0 ||
            held.count("W") != 0 || held.count("A") != 0)
        {
            ++headImages;
        }

        std::cout << image;
        for (const Action& action : actions)
        {
            std::cout << ' ';
            if (action.command == "AUCUNE")
            {
                std::cout << '-';
            }
            else if (action.command == "FLECHES" || action.command == "IJKL")
            {
                std::cout << vectorValue(held, action.usage);
            }
            else
            {
                std::cout << heldValue(held, action.command);
            }
        }
        std::cout << '\n';
    }

    std::cout << "DROITE " << rightActions << '\n';
    std::cout << "GAUCHE " << leftActions << '\n';
    std::cout << "TETE " << headImages << '\n';

    return 0;
}
