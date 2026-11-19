#include <iostream>
#include <string>

int main()
{
    std::size_t imageCount = 0;
    if (!(std::cin >> imageCount))
    {
        return 0;
    }

    std::size_t justes = 0;
    std::size_t fautives = 0;

    for (std::size_t image = 1; image <= imageCount; ++image)
    {
        std::size_t callCount = 0;
        std::cin >> callCount;

        bool waited = false;
        bool begun = false;
        bool views = false;
        bool leftTaken = false;
        bool rightTaken = false;
        bool ended = false;
        std::string fault;
        std::size_t faultPlace = 0;

        for (std::size_t place = 1; place <= callCount; ++place)
        {
            std::string call;
            std::cin >> call;
            if (!fault.empty())
            {
                continue;
            }

            if (ended)
            {
                fault = "APRES_END";
            }
            else if (call == "WAIT")
            {
                if (waited)
                {
                    fault = "DOUBLE_WAIT";
                }
                else
                {
                    waited = true;
                }
            }
            else if (call == "BEGIN")
            {
                if (!waited)
                {
                    fault = "BEGIN_SANS_WAIT";
                }
                else if (begun)
                {
                    fault = "DOUBLE_BEGIN";
                }
                else
                {
                    begun = true;
                }
            }
            else if (call == "VUES")
            {
                if (!waited)
                {
                    fault = "VUES_SANS_WAIT";
                }
                else
                {
                    views = true;
                }
            }
            else if (call == "PREND_G")
            {
                if (!begun)
                {
                    fault = "PREND_SANS_BEGIN";
                }
                else if (!views)
                {
                    fault = "PREND_SANS_VUES";
                }
                else if (leftTaken)
                {
                    fault = "DOUBLE_PREND";
                }
                else
                {
                    leftTaken = true;
                }
            }
            else if (call == "PREND_D")
            {
                if (!begun)
                {
                    fault = "PREND_SANS_BEGIN";
                }
                else if (!views)
                {
                    fault = "PREND_SANS_VUES";
                }
                else if (rightTaken)
                {
                    fault = "DOUBLE_PREND";
                }
                else
                {
                    rightTaken = true;
                }
            }
            else if (call == "LACHE_G")
            {
                if (!leftTaken)
                {
                    fault = "LACHE_SANS_PREND";
                }
                else
                {
                    leftTaken = false;
                }
            }
            else if (call == "LACHE_D")
            {
                if (!rightTaken)
                {
                    fault = "LACHE_SANS_PREND";
                }
                else
                {
                    rightTaken = false;
                }
            }
            else if (call == "END")
            {
                if (!begun)
                {
                    fault = "END_SANS_BEGIN";
                }
                else if (leftTaken || rightTaken)
                {
                    fault = "END_IMAGE_PRISE";
                }
                else
                {
                    ended = true;
                }
            }

            if (!fault.empty())
            {
                faultPlace = place;
            }
        }

        if (fault.empty() && !ended)
        {
            fault = "SANS_END";
            faultPlace = callCount + 1;
        }

        if (fault.empty())
        {
            ++justes;
            std::cout << image << " OK\n";
        }
        else
        {
            ++fautives;
            std::cout << image << " FAUTE " << faultPlace << ' ' << fault << '\n';
        }
    }

    if (imageCount == 0)
    {
        std::cout << "JUSTES 0\n";
        std::cout << "FAUTIVES 0\n";
    }
    else
    {
        std::cout << "JUSTES " << justes << '\n';
        std::cout << "FAUTIVES " << fautives << '\n';
    }
    return 0;
}
