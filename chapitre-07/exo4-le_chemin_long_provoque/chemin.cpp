#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include <NKWindow/NKMain.h>
#include <NKTime/NkChrono.h>

#include "../common/ExerciseMath.h"
#include "../common/RoomSupport.h"

using namespace nkentseu;
using namespace chapitre7;

static void ConfigureAppData(NkAppData& data) {
    data.appName = "Chapitre 7 - chemin long";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

namespace {

std::string FormatImage(uint64 image, double yaw, const NkVec3f& velocity,
                        float32 deltaW) {
    constexpr double radiansToDegrees = 180.0 / 3.14159265358979323846;
    char line[256];
    std::snprintf(line, sizeof(line),
                  "image %llu : lacet = %.3f deg, vitesse = (%.3f, %.3f, %.3f) "
                  "deg/s, delta.w = %.6f",
                  static_cast<unsigned long long>(image), yaw,
                  velocity.x * radiansToDegrees,
                  velocity.y * radiansToDegrees,
                  velocity.z * radiansToDegrees, deltaW);
    return line;
}

} // namespace

int nkmain(const NkEntryState& state) {
    (void)state;
    constexpr double turnRate = 20.0;
    constexpr double turnDuration = 360.0 / turnRate;
    constexpr double radiansToDegrees = 180.0 / 3.14159265358979323846;

    Room room;
    if (!room.Initialize("Chapitre 7 - chemin long")) {
        return 1;
    }
    if (!room.RenderFrame()) {
        std::fprintf(stderr, "L'image de chauffe a échoué.\n");
        return 1;
    }

    std::vector<std::string> longPath;
    std::vector<std::string> shortestPath;
    NkQuatf previousOrientation = NkQuatf::RotateY(NkAngle(0.0f));
    uint64 previousTime =
        static_cast<uint64>(NkChrono::Now().nanoseconds);
    uint64 image = 0u;
    uint64 faultyImage = 0u;
    const uint64 started = previousTime;

    while (room.IsRunning()) {
        room.PollEvents();
        if (!room.IsRunning()) {
            break;
        }
        const uint64 now =
            static_cast<uint64>(NkChrono::Now().nanoseconds);
        const double elapsed = static_cast<double>(now - started) / 1e9;
        if (elapsed >= turnDuration) {
            break;
        }
        const double yawDegrees = turnRate * elapsed;
        const float32 yawRadians = static_cast<float32>(
            yawDegrees / radiansToDegrees);
        const NkMat4f rotation =
            NkMat4f::RotationY(NkAngle::FromRad(yawRadians));
        NkQuatf orientation = NkQuatf(rotation).Normalized();
        // The local matrix conversion stays continuous; normalize the chosen
        // hemisphere explicitly to reproduce a pose-source representation flip.
        if (orientation.w < 0.0f) {
            orientation = NkQuatf(-orientation.x, -orientation.y,
                                  -orientation.z, -orientation.w);
        }
        const float32 dt =
            static_cast<float32>(static_cast<double>(now - previousTime) / 1e9);
        const NkQuatf rawDelta =
            (orientation * previousOrientation.Conjugate()).Normalized();
        const NkVec3f faultyVelocity =
            AngularVelocity(previousOrientation, orientation, dt, false);
        const NkVec3f correctedVelocity =
            AngularVelocity(previousOrientation, orientation, dt, true);
        ++image;

        const std::string faulty = FormatImage(
            image, yawDegrees, faultyVelocity, rawDelta.w);
        const std::string corrected = FormatImage(
            image, yawDegrees, correctedVelocity, rawDelta.w);
        std::printf("%s\n", faulty.c_str());
        longPath.push_back(faulty);
        shortestPath.push_back(corrected);

        if (faultyImage == 0u && rawDelta.w < 0.0f) {
            faultyImage = image;
        }
        if (!room.RenderFrame(orientation)) {
            std::fprintf(stderr, "Le rendu de la salle a échoué.\n");
            return 1;
        }
        previousTime = now;
        previousOrientation = orientation;
    }

    std::ofstream output(ExerciseOutputPath(__FILE__, "chemin.txt"),
                         std::ios::trunc);
    if (!output) {
        std::fprintf(stderr, "Impossible d'ouvrir chemin.txt\n");
        return 1;
    }
    if (faultyImage == 0u || longPath.empty()) {
        output << "Aucune image fautive observee pendant ce tour.\n";
        std::fprintf(stderr,
                     "Aucune vitesse anormale détectée; mesure à examiner.\n");
        return 2;
    }

    const size_t center = static_cast<size_t>(faultyImage - 1u);
    const size_t first = center == 0u ? 0u : center - 1u;
    const size_t last = std::min(center + 1u, longPath.size() - 1u);
    output << "sans le test delta.w < 0 :\n";
    for (size_t i = first; i <= last; ++i) {
        output << longPath[i] << '\n';
    }
    output << "avec le test delta.w < 0 :\n";
    for (size_t i = first; i <= last; ++i) {
        output << shortestPath[i] << '\n';
    }
    return 0;
}
