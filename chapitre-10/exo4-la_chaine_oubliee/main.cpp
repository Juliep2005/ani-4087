#include <iostream>
#include <string>

int main()
{
    std::size_t chainSize = 0;
    std::size_t callCount = 0;
    if (!(std::cin >> chainSize >> callCount) || chainSize == 0 || chainSize > 4)
    {
        if (chainSize == 0 || chainSize > 4)
        {
            std::cout << "JOURNAL 0\nAVERTISSEMENTS 0\nSILENCIEUX 0\n";
        }
        return 0;
    }

    bool frameOpen = false;
    std::size_t leftIndex = 0;
    std::size_t rightIndex = 0;
    bool leftTaken = false;
    bool rightTaken = false;
    std::size_t journal = 0;
    std::size_t warnings = 0;
    std::size_t silent = 0;

    for (std::size_t callNumber = 1; callNumber <= callCount; ++callNumber)
    {
        std::string call;
        std::cin >> call;

        if (call == "wait")
        {
            if (frameOpen)
            {
                std::cout << callNumber << " wait REFUSE\n";
                ++journal;
            }
            else
            {
                std::cout << callNumber << " wait OK\n";
            }
        }
        else if (call == "begin")
        {
            if (frameOpen)
            {
                std::cout << callNumber << " begin REFUSE\n";
                ++journal;
            }
            else
            {
                frameOpen = true;
                std::cout << callNumber << " begin OK\n";
            }
        }
        else if (call == "acquire")
        {
            std::string eye;
            std::cin >> eye;
            if (eye == "G")
            {
                if (leftTaken)
                {
                    std::cout << callNumber << " acquire REFUSE\n";
                    ++silent;
                }
                else
                {
                    leftTaken = true;
                    std::cout << callNumber << " acquire OK " << leftIndex << '\n';
                }
            }
            else
            {
                if (rightTaken)
                {
                    std::cout << callNumber << " acquire REFUSE\n";
                    ++silent;
                }
                else
                {
                    rightTaken = true;
                    std::cout << callNumber << " acquire OK " << rightIndex << '\n';
                }
            }
        }
        else if (call == "release")
        {
            std::string eye;
            std::cin >> eye;
            if (eye == "G")
            {
                if (!leftTaken)
                {
                    std::cout << callNumber << " release REFUSE\n";
                    ++silent;
                }
                else
                {
                    leftTaken = false;
                    leftIndex = (leftIndex + 1) % chainSize;
                    std::cout << callNumber << " release OK\n";
                }
            }
            else
            {
                if (!rightTaken)
                {
                    std::cout << callNumber << " release REFUSE\n";
                    ++silent;
                }
                else
                {
                    rightTaken = false;
                    rightIndex = (rightIndex + 1) % chainSize;
                    std::cout << callNumber << " release OK\n";
                }
            }
        }
        else if (call == "end")
        {
            int leftInProjection = 0;
            int rightInProjection = 0;
            std::string projection;
            std::cin >> projection;
            if (projection != "-")
            {
                leftInProjection = projection == "1" ? 1 : 0;
                std::cin >> rightInProjection;
            }
            if (!frameOpen)
            {
                std::cout << callNumber << " end REFUSE AUCUNE\n";
                ++journal;
            }
            else if (leftInProjection == 1 && leftTaken)
            {
                frameOpen = false;
                std::cout << callNumber << " end REFUSE ACQUISE G\n";
                ++journal;
            }
            else if (rightInProjection == 1 && rightTaken)
            {
                frameOpen = false;
                std::cout << callNumber << " end REFUSE ACQUISE D\n";
                ++journal;
            }
            else
            {
                frameOpen = false;
                if (leftInProjection == 0 && rightInProjection == 0)
                {
                    std::cout << callNumber << " end OK\n";
                }
                else if (leftInProjection == 0)
                {
                    std::cout << callNumber << " end OK AVERT G\n";
                    ++warnings;
                }
                else if (rightInProjection == 0)
                {
                    std::cout << callNumber << " end OK AVERT D\n";
                    ++warnings;
                }
                else
                {
                    std::cout << callNumber << " end OK\n";
                }
            }
        }
    }

    std::cout << "JOURNAL " << journal << '\n';
    std::cout << "AVERTISSEMENTS " << warnings << '\n';
    std::cout << "SILENCIEUX " << silent << '\n';
    return 0;
}
