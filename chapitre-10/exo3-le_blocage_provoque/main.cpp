#include <iostream>
#include <string>

int main()
{
    std::string policy;
    std::size_t imageCount = 0;
    if (!(std::cin >> policy >> imageCount))
    {
        return 0;
    }

    std::string signs;
    if (imageCount != 0)
    {
        std::cin >> signs;
    }

    bool frameOpen = false;
    std::size_t rendues = 0;
    std::size_t perdues = 0;
    std::size_t erreurs = 0;
    std::size_t premierRefus = 0;

    for (std::size_t image = 1; image <= imageCount; ++image)
    {
        bool render = false;
        std::string waitResult = "OK";
        std::string beginResult = "OK";
        std::string endResult = "-";
        bool beginAccepted = false;

        if (frameOpen)
        {
            waitResult = "REFUSE";
            ++erreurs;
            if (premierRefus == 0)
            {
                premierRefus = image;
            }
        }
        else
        {
            render = signs[image - 1] == '1';
        }

        if (frameOpen)
        {
            beginResult = "REFUSE";
            ++erreurs;
            if (premierRefus == 0)
            {
                premierRefus = image;
            }
        }
        else
        {
            frameOpen = true;
            beginAccepted = true;
        }

        if (signs[image - 1] == '1' && !beginAccepted)
        {
            ++perdues;
        }
        else if (render)
        {
            ++rendues;
        }

        const std::string renderText = render ? "OUI" : "NON";
        if (policy == "TOUJOURS")
        {
            if (!frameOpen)
            {
                endResult = "REFUSE";
                ++erreurs;
            }
            else
            {
                endResult = "OK";
                frameOpen = false;
            }
        }
        else if (render)
        {
            if (!frameOpen)
            {
                endResult = "REFUSE";
                ++erreurs;
            }
            else
            {
                endResult = "OK";
                frameOpen = false;
            }
        }

        std::cout << image << ' ' << waitResult << ' ' << beginResult << ' '
                  << renderText << ' ' << endResult << '\n';
    }

    std::cout << "RENDUES " << rendues << '\n';
    std::cout << "PERDUES " << perdues << '\n';
    std::cout << "ERREURS " << erreurs << '\n';
    std::cout << "PREMIER REFUS " << premierRefus << '\n';
    return 0;
}
