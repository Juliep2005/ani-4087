#include <cmath>
#include <cstdio>
#include <fstream>

#include <NKWindow/NKMain.h>

#include "../common/ExerciseMath.h"

using namespace nkentseu;
using namespace chapitre7;

static void ConfigureAppData(NkAppData& data) {
    data.appName = "Chapitre 7 - modeles";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

namespace {

constexpr double kHz = 72.0;
constexpr double kPeriod = 1.0 / kHz;

struct Gesture {
    const char* label;
    double amplitude;
    double duration;
    bool abruptStop;
};

double GestureYaw(const Gesture& gesture, double time) {
    if (gesture.abruptStop) {
        const double speed = gesture.amplitude / gesture.duration;
        return time <= gesture.duration ? speed * time : gesture.amplitude;
    }
    return SmoothTurn(time, gesture.amplitude, gesture.duration);
}

struct ModelResult {
    Statistics velocity;
    Statistics acceleration;
};

ModelResult Measure(const Gesture& gesture) {
    const uint32 sampleCount =
        static_cast<uint32>(std::ceil((gesture.duration + 0.2) / kPeriod)) + 1u;
    ModelResult result;
    double previousPreviousYaw = 0.0;
    double previousYaw = 0.0;
    double currentYaw = 0.0;

    for (uint32 i = 0u; i < sampleCount; ++i) {
        const double time = static_cast<double>(i) * kPeriod;
        if (time > gesture.duration + 0.2) {
            break;
        }
        currentYaw = GestureYaw(gesture, time);
        const double futureTime = time + kPeriod;
        const double actual = GestureYaw(gesture, futureTime);
        const double velocity = i == 0u
            ? 0.0
            : (currentYaw - previousYaw) / kPeriod;
        const double previousVelocity = i < 2u
            ? velocity
            : (previousYaw - previousPreviousYaw) / kPeriod;
        const double acceleration = i < 2u
            ? 0.0
            : (velocity - previousVelocity) / kPeriod;
        const double predictionByVelocity = currentYaw + velocity * kPeriod;
        const double predictionByAcceleration =
            predictionByVelocity + 0.5 * acceleration * kPeriod * kPeriod;

        result.velocity.Add(std::abs(actual - predictionByVelocity));
        result.acceleration.Add(std::abs(actual - predictionByAcceleration));
        previousPreviousYaw = previousYaw;
        previousYaw = currentYaw;
    }
    return result;
}

void WriteResult(std::ofstream& output, const Gesture& gesture,
                 const ModelResult& result) {
    output << gesture.label << " : vitesse " << result.velocity.Mean()
           << " deg (max " << result.velocity.Maximum() << "), acceleration "
           << result.acceleration.Mean() << " deg (max "
           << result.acceleration.Maximum() << ")\n";
    std::printf("%s : vitesse %.6f deg (max %.6f), acceleration %.6f deg "
                "(max %.6f)\n",
                gesture.label, result.velocity.Mean(),
                result.velocity.Maximum(), result.acceleration.Mean(),
                result.acceleration.Maximum());
}

} // namespace

int nkmain(const NkEntryState& state) {
    (void)state;
    const Gesture gestures[] = {
        {"rapide (90 deg en 400 ms)", 90.0, 0.400, false},
        {"lent (45 deg en 1,5 s)", 45.0, 1.500, false},
        {"demi-tour (180 deg en 250 ms)", 180.0, 0.250, false},
        {"arret (180 deg/s pendant 400 ms)", 72.0, 0.400, true}
    };

    std::ofstream output(ExerciseOutputPath(__FILE__, "modeles.txt"),
                         std::ios::trunc);
    if (!output) {
        std::fprintf(stderr, "Impossible d'ouvrir modeles.txt\n");
        return 1;
    }
    output.precision(3);
    output.setf(std::ios::fixed);
    std::printf("Echantillonnage simulé à 72 Hz; erreurs pendant le geste "
                "et les 200 ms suivants.\n");
    for (const Gesture& gesture : gestures) {
        WriteResult(output, gesture, Measure(gesture));
    }
    return 0;
}
