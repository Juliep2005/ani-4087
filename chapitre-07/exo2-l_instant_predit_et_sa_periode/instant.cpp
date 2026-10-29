#include <cstdio>
#include <fstream>

#include <NKWindow/NKMain.h>
#include <NKTime/NkChrono.h>

#include "../common/ExerciseMath.h"
#include "../common/RoomSupport.h"

using namespace nkentseu;
using namespace chapitre7;

static void ConfigureAppData(NkAppData& data) {
    data.appName = "Chapitre 7 - instant prédit";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState& state) {
    (void)state;
    double hz = 72.0;
    if (!ReadEnvDouble("MASALLE_HZ", 72.0, hz) || hz <= 0.0) {
        std::fprintf(stderr, "MASALLE_HZ doit être strictement positif.\n");
        return 1;
    }
    const double periodNs = 1e9 / hz;
    const uint64 autoExit = ReadEnvU64("MASALLE_EXIT", 700u);

    Room room;
    if (!room.Initialize("Chapitre 7 - instant prédit")) {
        return 1;
    }

    const uint64 start = static_cast<uint64>(NkChrono::Now().nanoseconds);
    uint64 previousPredicted = 0u;
    uint32 intervalsSeen = 0u;
    uint32 frame = 0u;
    Statistics predictionOffsets;
    const auto outputMode = hz == 72.0 ? std::ios::trunc : std::ios::app;
    std::ofstream output(ExerciseOutputPath(__FILE__, "instants.txt"),
                         outputMode);
    if (!output) {
        std::fprintf(stderr, "Impossible d'ouvrir instants.txt\n");
        return 1;
    }
    output.precision(3);
    output.setf(std::ios::fixed);

    while (room.IsRunning() && predictionOffsets.Count() < 600u &&
           (autoExit == 0u || frame < autoExit)) {
        room.PollEvents();
        if (!room.IsRunning()) {
            break;
        }
        const uint64 now = static_cast<uint64>(NkChrono::Now().nanoseconds);
        const uint64 predicted = now + static_cast<uint64>(periodNs);
        if (previousPredicted != 0u) {
            const double offsetMs =
                (static_cast<double>(now) - static_cast<double>(previousPredicted)) /
                1e6;
            if (intervalsSeen >= 30u) {
                predictionOffsets.Add(offsetMs);
            }
            ++intervalsSeen;
        }

        if (hz == 72.0 && frame < 20u) {
            const double nowMs = static_cast<double>(now - start) / 1e6;
            const double predictedMs =
                static_cast<double>(predicted - start) / 1e6;
            std::printf("maintenant = %.3f ms, predit = %.3f ms, ecart = %.3f ms\n",
                        nowMs, predictedMs, predictedMs - nowMs);
            output << "72 Hz: maintenant = " << nowMs
                   << " ms, predit = " << predictedMs
                   << " ms, ecart = " << predictedMs - nowMs << " ms\n";
        }

        previousPredicted = predicted;
        if (!room.RenderFrame()) {
            std::fprintf(stderr, "Le rendu de la salle a échoué.\n");
            return 1;
        }
        ++frame;
    }

    if (predictionOffsets.Count() != 600u) {
        std::fprintf(stderr, "Mesure incomplète: %zu écarts sur 600.\n",
                     predictionOffsets.Count());
        return 1;
    }
    const double periodMs = 1000.0 / hz;
    std::printf("hz = %.0f, periode = %.3f ms, reel - predit = %.3f ms\n",
                hz, periodMs, predictionOffsets.Mean());
    output << "hz = " << hz << ", periode = " << periodMs
           << " ms, reel - predit = " << predictionOffsets.Mean() << " ms\n";
    return 0;
}
