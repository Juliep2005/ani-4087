#include <cstdio>
#include <fstream>
#include <vector>

#include <NKWindow/NKMain.h>
#include <NKTime/NkChrono.h>

#include "../common/ExerciseMath.h"
#include "../common/RoomSupport.h"

using namespace nkentseu;
using namespace chapitre7;

static void ConfigureAppData(NkAppData& data) {
    data.appName = "Chapitre 7 - budget";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState& state) {
    (void)state;
    for (double hz : {72.0, 90.0, 120.0}) {
        std::printf("budget %.0f Hz : %.3f ms\n", hz, 1000.0 / hz);
    }

    Room room;
    if (!room.Initialize("Chapitre 7 - budget")) {
        return 1;
    }
    if (EnvFlag("NK_VK_MAILBOX") && !room.UsesVulkan()) {
        std::fprintf(stderr,
                     "NK_VK_MAILBOX=1 est demandé, mais l'API active n'est pas Vulkan.\n");
    }

    Statistics intervals;
    uint64 previous = 0u;
    uint32 intervalsSeen = 0u;
    uint32 frames = 0u;
    const uint64 autoExit = ReadEnvU64("MASALLE_EXIT", 1040u);
    while (room.IsRunning() && intervals.Count() < 1000u &&
           (autoExit == 0u || frames < autoExit)) {
        room.PollEvents();
        if (!room.IsRunning()) {
            break;
        }
        const uint64 now = static_cast<uint64>(NkChrono::Now().nanoseconds);
        if (previous != 0u) {
            const double milliseconds =
                static_cast<double>(now - previous) / 1e6;
            if (intervalsSeen >= 30u) {
                intervals.Add(milliseconds);
            }
            ++intervalsSeen;
        }
        previous = now;
        if (!room.RenderFrame()) {
            std::fprintf(stderr, "Le rendu de la salle a échoué.\n");
            return 1;
        }
        ++frames;
    }

    if (intervals.Count() != 1000u) {
        std::fprintf(stderr, "Mesure incomplète: %zu intervalles sur 1000.\n",
                     intervals.Count());
        return 1;
    }

    const char* label = EnvFlag("NK_VK_MAILBOX") ? "sans attente" : "ecran";
    std::printf("%s : moyenne %.3f ms, mediane %.3f ms, max %.3f ms\n",
                label, intervals.Mean(), intervals.Median(), intervals.Maximum());
    std::ofstream output(ExerciseOutputPath(__FILE__, "budget.txt"),
                         std::ios::app);
    if (!output) {
        std::fprintf(stderr, "Impossible d'ouvrir budget.txt\n");
        return 1;
    }
    output.precision(3);
    output.setf(std::ios::fixed);
    output << label << " : moyenne " << intervals.Mean()
           << " ms, mediane " << intervals.Median()
           << " ms, max " << intervals.Maximum() << " ms\n";
    return 0;
}
