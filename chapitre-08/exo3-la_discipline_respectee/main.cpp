#include <cstdint>
#include <iostream>
#include <string>

int main()
{
    std::uint64_t imageCount = 0;
    std::uint64_t orderCount = 0;
    if (!(std::cin >> imageCount >> orderCount))
    {
        return 0;
    }

    std::uint64_t nextImage = 0;
    std::uint64_t borrowedImage = 0;
    std::uint64_t submittedCount = 0;
    std::uint64_t refusalCount = 0;
    std::uint64_t outOfBorrowCount = 0;
    bool borrowed = false;
    bool drawn = false;
    bool releasedSinceSubmit = false;
    bool lastReleasedHadDraw = false;

    for (std::uint64_t order = 0; order < orderCount; ++order)
    {
        std::string command;
        std::cin >> command;

        if (command == "acquire")
        {
            if (borrowed)
            {
                ++refusalCount;
                std::cout << "ACQUIRE REFUSE\n";
            }
            else
            {
                borrowed = true;
                borrowedImage = nextImage;
                drawn = false;
                std::cout << "ACQUIRE " << borrowedImage << '\n';
            }
        }
        else if (command == "draw")
        {
            if (!borrowed)
            {
                ++outOfBorrowCount;
                std::cout << "DRAW HORS EMPRUNT\n";
            }
            else
            {
                drawn = true;
                std::cout << "DRAW " << borrowedImage << '\n';
            }
        }
        else if (command == "release")
        {
            if (!borrowed)
            {
                ++refusalCount;
                std::cout << "RELEASE REFUSE\n";
            }
            else
            {
                nextImage = (nextImage + 1) % imageCount;
                borrowed = false;
                releasedSinceSubmit = true;
                lastReleasedHadDraw = drawn;
                std::cout << "RELEASE OK\n";
            }
        }
        else if (command == "submit")
        {
            if (borrowed)
            {
                ++refusalCount;
                std::cout << "SUBMIT REFUSE EMPRUNTEE\n";
            }
            else if (!releasedSinceSubmit)
            {
                ++refusalCount;
                std::cout << "SUBMIT REFUSE RIEN\n";
            }
            else
            {
                const std::uint64_t submittedImage =
                    (nextImage + imageCount - 1) % imageCount;
                std::cout << "SUBMIT " << submittedImage;
                if (!lastReleasedHadDraw)
                {
                    std::cout << " VIDE";
                }
                std::cout << '\n';
                ++submittedCount;
                releasedSinceSubmit = false;
            }
        }
    }

    std::cout << "SOUMISES " << submittedCount << '\n';
    std::cout << "REFUS " << refusalCount << '\n';
    std::cout << "HORS EMPRUNT " << outOfBorrowCount << '\n';

    return 0;
}
