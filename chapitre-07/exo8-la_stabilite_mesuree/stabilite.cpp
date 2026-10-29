#include <cstdio>
#include <fstream>

#include <NKWindow/NKMain.h>
#include <NKTime/NkChrono.h>

#include "../common/ExerciseMath.h"
#include "../common/RoomSupport.h"

using namespace nkentseu;
using namespace chapitre7;

static void ConfigureAppData(NkAppData& data) {
    data.appName = "Chapitre 7 - stabilité";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState& state) {
    (void)state;
    Room room;
    if (!room.Initialize("Chapitre 7 - stabilité")) {
        return 1;
    }
    if (EnvFlag("NK_VK_MAILBOX") && !room.UsesVulkan()) {
        std::fprintf(stderr,
                     "NK_VK_MAILBOX=1 est demandé, mais l'API active n'est pas Vulkan.\n");
    }

    uint64 previous = 0u;
    uint32 warmupIntervals = 0u;

    while (room.IsRunning() && warmupIntervals < 30u) {
        room.PollEvents();
        if (!room.IsRunning()) {
            break;
        }
        const uint64 now = static_cast<uint64>(NkChrono::Now().nanoseconds);
        if (previous != 0u) {
            ++warmupIntervals;
        }
        previous = now;
        if (!room.RenderFrame()) {
            std::fprintf(stderr, "Le rendu de la salle a échoué.\n");
            return 1;
        }
    }

    const uint64 start = static_cast<uint64>(NkChrono::Now().nanoseconds);
    const uint64 deadline = start + 60'000'000'000ull;
    previous = 0u;
    Statistics intervals;
    while (room.IsRunning()) {
        room.PollEvents();
        if (!room.IsRunning()) {
            break;
        }
        const uint64 now = static_cast<uint64>(NkChrono::Now().nanoseconds);
        if (now >= deadline) {
            break;
        }
        if (previous != 0u) {
            intervals.Add(static_cast<double>(now - previous) / 1e6);
        }
        previous = now;
        if (!room.RenderFrame()) {
            std::fprintf(stderr, "Le rendu de la salle a échoué.\n");
            return 1;
        }
    }

    if (intervals.Count() == 0u) {
        std::fprintf(stderr, "Aucun intervalle mesuré pendant la minute.\n");
        return 1;
    }
    constexpr double budget90Hz = 1000.0 / 90.0;
    const size_t overBudget = intervals.CountAbove(budget90Hz);
    std::printf("Mesuré sur %zu intervalles après 30 intervalles de chauffe.\n",
                intervals.Count());
    std::printf("%s : %zu images, moyenne %.3f ms, ecart type %.3f ms, "
                "max %.3f ms, hors budget %llu\n",
                EnvFlag("NK_VK_MAILBOX") ? "sans attente" : "ecran",
                intervals.Count(), intervals.Mean(),
                intervals.StandardDeviation(), intervals.Maximum(),
                static_cast<unsigned long long>(overBudget));

    std::ofstream output(ExerciseOutputPath(__FILE__, "stabilite.txt"),
                         EnvFlag("NK_VK_MAILBOX") ? std::ios::app
                                                  : std::ios::trunc);
    if (!output) {
        std::fprintf(stderr, "Impossible d'ouvrir stabilite.txt\n");
        return 1;
    }
    output.precision(3);
    output.setf(std::ios::fixed);
    output << (EnvFlag("NK_VK_MAILBOX") ? "sans attente" : "ecran")
           << " : " << intervals.Count() << " images, moyenne "
           << intervals.Mean() << " ms, ecart type "
           << intervals.StandardDeviation() << " ms, max "
           << intervals.Maximum() << " ms, hors budget " << overBudget
           << '\n';
    return 0;
}
