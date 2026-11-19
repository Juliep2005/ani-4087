#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <algorithm>

int main()
{
    std::int64_t speed = 0;
    std::size_t imageCount = 0;
    if (!(std::cin >> speed >> imageCount))
    {
        return 0;
    }

    std::size_t fausses = 0;
    std::int64_t pireCasque = 0;
    std::int64_t pireSimulateur = 0;

    for (std::size_t image = 1; image <= imageCount; ++image)
    {
        std::int64_t predicted = 0;
        std::int64_t submitted = 0;
        std::int64_t displayed = 0;
        char mode = 'R';
        std::cin >> predicted >> submitted >> displayed >> mode;

        const std::int64_t predictedPose = speed * predicted;
        const std::int64_t submittedPose = (mode == 'R') ? predictedPose : speed * submitted;
        const std::int64_t casque = submittedPose - predictedPose;
        const std::int64_t simulateur = speed * displayed - predictedPose;

        std::cout << image << " CASQUE " << casque
                  << " SIMULATEUR " << simulateur << '\n';

        if (casque != 0)
        {
            ++fausses;
        }
        pireCasque = std::max(pireCasque, std::llabs(casque));
        pireSimulateur = std::max(pireSimulateur, std::llabs(simulateur));
    }

    if (imageCount == 0)
    {
        fausses = 0;
        pireCasque = 0;
        pireSimulateur = 0;
    }

    std::cout << "FAUSSES " << fausses << '\n';
    std::cout << "PIRE CASQUE " << pireCasque << '\n';
    std::cout << "PIRE SIMULATEUR " << pireSimulateur << '\n';
    return 0;
}
