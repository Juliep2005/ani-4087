#include <cmath>
#include <cstdio>
#include <fstream>

#include <NKWindow/NKMain.h>
#include <NKTime/NkChrono.h>

#include "../common/ExerciseMath.h"
#include "../common/RoomSupport.h"

using namespace nkentseu;
using namespace chapitre7;

static void ConfigureAppData(NkAppData& data) {
    data.appName = "Chapitre 7 - vitesse";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState& state) {
    (void)state;
    double programmedTurn = 0.0;
    if (!ReadEnvDouble("MASALLE_TOUR", 0.0, programmedTurn)) {
        return 1;
    }
    const bool isProgrammed = programmedTurn != 0.0;

    Room room;
    if (!room.Initialize("Chapitre 7 - mesurer une vitesse")) {
        return 1;
    }
    if (!room.RenderFrame()) {
        std::fprintf(stderr, "L'image de chauffe a échoué.\n");
        return 1;
    }

    const char* series = isProgrammed ? "serie programme" : "serie souris";
    std::ofstream output(ExerciseOutputPath(__FILE__, "vitesses.txt"),
                         isProgrammed ? std::ios::trunc : std::ios::app);
    if (!output) {
        std::fprintf(stderr, "Impossible d'ouvrir vitesses.txt\n");
        return 1;
    }
    output.precision(3);
    output.setf(std::ios::fixed);
    output << series << '\n';
    std::printf("%s\n", series);

    constexpr float32 mouseRadiansPerCount = 0.0022f;
    float32 yawRad = 0.0f;
    float32 pitchRad = 0.0f;
    NkQuatf previousOrientation = NkQuatf::RotateY(NkAngle(0.0f));
    if (!room.RenderFrame(previousOrientation)) {
        std::fprintf(stderr, "L'image de référence a échoué.\n");
        return 1;
    }
    const uint64 started = static_cast<uint64>(NkChrono::Now().nanoseconds);
    uint64 previousTime = started;

    uint32 frame = 0u;
    uint32 recorded = 0u;
    for (; recorded < 20u && room.IsRunning(); ++frame) {
        const NkVec2f rawDelta = room.PollEvents();
        if (!room.IsRunning()) {
            break;
        }
        if (isProgrammed) {
            const uint64 now =
                static_cast<uint64>(NkChrono::Now().nanoseconds);
            const double elapsed =
                static_cast<double>(now - started) / 1e9;
            yawRad = static_cast<float32>(
                elapsed * programmedTurn * 3.14159265358979323846 / 180.0);
        } else {
            yawRad -= rawDelta.x * mouseRadiansPerCount;
            pitchRad -= rawDelta.y * mouseRadiansPerCount;
        }

        const NkMat4f rotation =
            NkMat4f::RotationY(NkAngle::FromRad(yawRad)) *
            NkMat4f::RotationX(NkAngle::FromRad(pitchRad));
        const NkQuatf orientation = NkQuatf(rotation).Normalized();
        const uint64 now = static_cast<uint64>(NkChrono::Now().nanoseconds);
        const float32 dtSeconds =
            static_cast<float32>(static_cast<double>(now - previousTime) / 1e9);
        const NkVec3f velocity =
            AngularVelocity(previousOrientation, orientation, dtSeconds);
        const double dtMs = static_cast<double>(now - previousTime) / 1e6;
        const float32 radiansToDegrees = 180.0f / 3.14159265358979323846f;
        if (frame != 0u) {
            const uint32 image = recorded + 1u;
            std::printf(
                "image %u : dt = %.3f ms, vitesse = (%.3f, %.3f, %.3f) deg/s\n",
                image, dtMs, velocity.x * radiansToDegrees,
                velocity.y * radiansToDegrees,
                velocity.z * radiansToDegrees);
            output << "image " << image << " : dt = " << dtMs
                   << " ms, vitesse = ("
                   << velocity.x * radiansToDegrees << ", "
                   << velocity.y * radiansToDegrees << ", "
                   << velocity.z * radiansToDegrees << ") deg/s\n";
            ++recorded;
        }

        if (!room.RenderFrame(orientation)) {
            std::fprintf(stderr, "Le rendu de la salle a échoué.\n");
            return 1;
        }
        previousTime = now;
        previousOrientation = orientation;
    }

    if (recorded != 20u) {
        std::fprintf(stderr, "La série est incomplète.\n");
        return 1;
    }
    return 0;
}
