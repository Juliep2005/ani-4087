#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Action
{
    std::string name;
    std::string usage;
};

std::vector<std::string> readTable(
    std::size_t tableNumber,
    const std::vector<Action>& actions)
{
    std::size_t bindingCount = 0;
    std::cin >> bindingCount;

    std::unordered_map<std::string, std::string> bindings;
    for (std::size_t i = 0; i < bindingCount; ++i)
    {
        std::string usage;
        std::string command;
        std::cin >> usage >> command;
        bindings.emplace(usage, command);
    }

    std::cout << "TABLE " << tableNumber << '\n';
    for (const Action& action : actions)
    {
        if (bindings.count(action.usage) == 0)
        {
            std::cout << "INACTIVE " << action.name << '\n';
        }
    }

    std::size_t imageCount = 0;
    std::cin >> imageCount;
    std::vector<bool> previouslyPressed(actions.size(), false);
    std::vector<std::string> imageLines;
    imageLines.reserve(imageCount);

    for (std::size_t image = 1; image <= imageCount; ++image)
    {
        std::size_t commandCount = 0;
        std::cin >> commandCount;

        std::unordered_set<std::string> heldCommands;
        for (std::size_t i = 0; i < commandCount; ++i)
        {
            std::string command;
            std::cin >> command;
            heldCommands.insert(command);
        }

        std::ostringstream line;
        line << image;
        bool anyPressed = false;
        for (std::size_t i = 0; i < actions.size(); ++i)
        {
            const auto binding = bindings.find(actions[i].usage);
            const bool pressed =
                binding != bindings.end() &&
                heldCommands.count(binding->second) != 0;

            if (pressed && !previouslyPressed[i])
            {
                line << ' ' << actions[i].name;
                anyPressed = true;
            }
            previouslyPressed[i] = pressed;
        }

        if (!anyPressed)
        {
            line << " -";
        }

        imageLines.push_back(line.str());
        std::cout << imageLines.back() << '\n';
    }

    return imageLines;
}

int main()
{
    std::size_t actionCount = 0;
    if (!(std::cin >> actionCount))
    {
        return 0;
    }

    std::vector<Action> actions(actionCount);
    for (Action& action : actions)
    {
        std::cin >> action.name >> action.usage;
    }

    const std::vector<std::string> firstTable = readTable(1, actions);
    const std::vector<std::string> secondTable = readTable(2, actions);

    const bool identical = firstTable == secondTable;
    std::cout << "IDENTIQUES " << (identical ? "OUI" : "NON") << '\n';

    return 0;
}
