#include <cstdio>
#include <fstream>

#include <NKWindow/NKMain.h>
#include <NKTime/NkChrono.h>

#include "../common/ExerciseMath.h"
#include "../common/RoomSupport.h"

using namespace nkentseu;
using namespace chapitre7;

static void ConfigureAppData(NkAppData& data) {
    data.appName = "Chapitre 7 - prédire et comparer";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState& state) {
    (void)state;
    double hz = 72.0;
    if (!ReadEnvDouble("MASALLE_HZ", 72.0, hz) || hz <= 0.0) {
        std::fprintf(stderr, "MASALLE_HZ doit être strictement positif.\n");
        return 1;
    }

    Room room;
    if (!room.Initialize("Chapitre 7 - prédire et comparer")) {
        return 1;
    }
    if (!room.RenderFrame()) {
        std::fprintf(stderr, "L'image de chauffe a échoué.\n");
        return 1;
    }

    constexpr float32 mouseRadiansPerCount = 0.0022f;
    const float32 predictionHorizon = static_cast<float32>(1.0 / hz);
    float32 yawRad = 0.0f;
    float32 pitchRad = 0.0f;
    bool hasPrevious = false;
    bool hasMouseMotion = false;
    NkQuatf previousOrientation;
    NkQuatf previousPrediction;
    uint64 previousTime = 0u;
    Statistics withPrediction;
    Statistics withoutPrediction;

    while (room.IsRunning() && withPrediction.Count() < 1000u) {
        const NkVec2f rawDelta = room.PollEvents();
        if (!room.IsRunning()) {
            break;
        }
        hasMouseMotion = hasMouseMotion ||
                         rawDelta.x != 0.0f || rawDelta.y != 0.0f;
        yawRad -= rawDelta.x * mouseRadiansPerCount;
        pitchRad -= rawDelta.y * mouseRadiansPerCount;

        const NkMat4f rotation =
            NkMat4f::RotationY(NkAngle::FromRad(yawRad)) *
            NkMat4f::RotationX(NkAngle::FromRad(pitchRad));
        const NkQuatf orientation = NkQuatf(rotation).Normalized();
        const uint64 now = static_cast<uint64>(NkChrono::Now().nanoseconds);

        if (hasPrevious) {
            const double dtSeconds =
                static_cast<double>(now - previousTime) / 1e9;
            const NkQuatf actual = orientation;
            withPrediction.Add(
                QuaternionErrorDegrees(previousPrediction, actual));
            withoutPrediction.Add(
                QuaternionErrorDegrees(previousOrientation, actual));
            const NkVec3f velocity = AngularVelocity(
                previousOrientation, orientation,
                static_cast<float32>(dtSeconds));
            previousPrediction =
                Extrapolate(orientation, velocity, predictionHorizon);
        } else {
            previousPrediction = orientation;
            hasPrevious = true;
        }

        if (!room.RenderFrame(orientation)) {
            std::fprintf(stderr, "Le rendu de la salle a échoué.\n");
            return 1;
        }
        previousOrientation = orientation;
        previousTime = now;
    }

    if (withPrediction.Count() != 1000u) {
        std::fprintf(stderr, "Mesure interrompue avant 1000 images.\n");
        return 1;
    }
    if (!hasMouseMotion) {
        std::fprintf(stderr,
                     "Aucun mouvement brut de souris; déplacez la souris et recommencez.\n");
        return 2;
    }

    std::printf("avec prediction : moyenne %.6f deg, max %.6f deg, sur %zu images\n",
                withPrediction.Mean(), withPrediction.Maximum(),
                withPrediction.Count());
    std::printf("sans prediction : moyenne %.6f deg, max %.6f deg, sur %zu images\n",
                withoutPrediction.Mean(), withoutPrediction.Maximum(),
                withoutPrediction.Count());
    std::ofstream output(ExerciseOutputPath(__FILE__, "erreurs.txt"),
                         std::ios::trunc);
    if (!output) {
        std::fprintf(stderr, "Impossible d'ouvrir erreurs.txt\n");
        return 1;
    }
    output.precision(3);
    output.setf(std::ios::fixed);
    output << "avec prediction : moyenne " << withPrediction.Mean()
           << " deg, max " << withPrediction.Maximum() << " deg, sur "
           << withPrediction.Count() << " images\n";
    output << "sans prediction : moyenne " << withoutPrediction.Mean()
           << " deg, max " << withoutPrediction.Maximum() << " deg, sur "
           << withoutPrediction.Count() << " images\n";
    return 0;
}
