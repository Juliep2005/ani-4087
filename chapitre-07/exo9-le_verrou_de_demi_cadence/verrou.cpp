#include <cstdio>
#include <fstream>

#include <NKWindow/NKMain.h>
#include <NKTime/NkChrono.h>

#include "../common/ExerciseMath.h"
#include "../common/RoomSupport.h"

using namespace nkentseu;
using namespace chapitre7;

static void ConfigureAppData(NkAppData& data) {
    data.appName = "Chapitre 7 - verrou de demi-cadence";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

namespace {

bool RenderMeasuredFrame(Room& room, uint32 copies, uint64& start) {
    room.PollEvents();
    if (!room.IsRunning()) {
        return false;
    }
    start = static_cast<uint64>(NkChrono::Now().nanoseconds);
    if (!room.RenderFrame(NkQuatf(), copies)) {
        std::fprintf(stderr, "Le rendu de la salle a échoué.\n");
        return false;
    }
    return true;
}

bool CalibrateCopies(Room& room, uint32& copies, double frameBudgetMs) {
    constexpr uint32 maxCopies = 512u;
    while (copies <= maxCopies) {
        Statistics trial;
        uint64 previous = 0u;
        for (uint32 frame = 0u; frame < 21u; ++frame) {
            uint64 start = 0u;
            if (!RenderMeasuredFrame(room, copies, start)) {
                return false;
            }
            if (previous != 0u) {
                trial.Add(static_cast<double>(start - previous) / 1e6);
            }
            previous = start;
        }
        std::printf("calibration : copies = %u, moyenne %.3f ms\n",
                    copies, trial.Mean());
        if (trial.Mean() > frameBudgetMs || copies == maxCopies) {
            return true;
        }
        copies = copies > maxCopies / 2u ? maxCopies : copies * 2u;
    }
    return true;
}

bool Measure(Room& room, uint32 copies, bool locked, uint64 periodNs,
             Statistics& intervals) {
    uint64 previous = 0u;
    for (uint32 frame = 0u; frame < 1001u; ++frame) {
        uint64 start = 0u;
        if (!RenderMeasuredFrame(room, copies, start)) {
            return false;
        }
        if (previous != 0u) {
            intervals.Add(static_cast<double>(start - previous) / 1e6);
        }
        previous = start;
        if (locked) {
            const uint64 deadline = start + 2u * periodNs;
            for (;;) {
                const uint64 now =
                    static_cast<uint64>(NkChrono::Now().nanoseconds);
                if (now >= deadline) {
                    break;
                }
                const uint64 remainingNs = deadline - now;
                if (remainingNs > 2'000'000ull) {
                    const int64 sleepMs =
                        static_cast<int64>(remainingNs / 1'000'000ull) - 1;
                    NkChrono::SleepMilliseconds(sleepMs);
                }
            }
        }
    }
    return intervals.Count() == 1000u;
}

} // namespace

int nkmain(const NkEntryState& state) {
    (void)state;
    double hz = 72.0;
    if (!ReadEnvDouble("MASALLE_HZ", 72.0, hz) || hz <= 0.0) {
        std::fprintf(stderr, "MASALLE_HZ doit être strictement positif.\n");
        return 1;
    }
    uint64 initialCopies = ReadEnvU64("MASALLE_COPIES", 2u);
    if (initialCopies == 0u || initialCopies > 512u) {
        std::fprintf(stderr, "MASALLE_COPIES doit être compris entre 1 et 512.\n");
        return 1;
    }

    Room room;
    if (!room.Initialize("Chapitre 7 - verrou de demi-cadence")) {
        return 1;
    }

    for (uint32 frame = 0u; frame < 30u; ++frame) {
        uint64 start = 0u;
        if (!RenderMeasuredFrame(room, 1u, start)) {
            std::fprintf(stderr, "La chauffe de la salle a échoué.\n");
            return 1;
        }
    }

    uint32 copies = static_cast<uint32>(initialCopies);
    const double periodMs = 1000.0 / hz;
    if (!CalibrateCopies(room, copies, periodMs)) {
        std::fprintf(stderr, "Calibration interrompue.\n");
        return 1;
    }
    if (copies == 512u) {
        std::fprintf(stderr,
                     "Limite de 512 copies atteinte; vérifiez que la période est dépassée.\n");
    }

    Statistics withoutLock;
    if (!Measure(room, copies, false,
                 static_cast<uint64>(1e9 / hz), withoutLock)) {
        std::fprintf(stderr, "Mesure sans verrou interrompue.\n");
        return 1;
    }

    NkChrono::BeginPreciseTiming();
    Statistics withLock;
    const bool lockedMeasurement = Measure(
        room, copies, true, static_cast<uint64>(1e9 / hz), withLock);
    NkChrono::EndPreciseTiming();
    if (!lockedMeasurement) {
        std::fprintf(stderr, "Mesure avec verrou interrompue.\n");
        return 1;
    }

    std::printf("copies de la salle : %u\n", copies);
    std::printf("sans verrou : moyenne %.3f ms, ecart type %.3f ms, max %.3f ms\n",
                withoutLock.Mean(), withoutLock.StandardDeviation(),
                withoutLock.Maximum());
    std::printf("avec verrou : moyenne %.3f ms, ecart type %.3f ms, max %.3f ms\n",
                withLock.Mean(), withLock.StandardDeviation(),
                withLock.Maximum());

    std::ofstream output(ExerciseOutputPath(__FILE__, "verrou.txt"),
                         std::ios::trunc);
    if (!output) {
        std::fprintf(stderr, "Impossible d'ouvrir verrou.txt\n");
        return 1;
    }
    output.precision(3);
    output.setf(std::ios::fixed);
    output << "copies de la salle : " << copies << '\n';
    output << "sans verrou : moyenne " << withoutLock.Mean()
           << " ms, ecart type " << withoutLock.StandardDeviation()
           << " ms, max " << withoutLock.Maximum() << " ms\n";
    output << "avec verrou : moyenne " << withLock.Mean()
           << " ms, ecart type " << withLock.StandardDeviation()
           << " ms, max " << withLock.Maximum() << " ms\n";
    return 0;
}
