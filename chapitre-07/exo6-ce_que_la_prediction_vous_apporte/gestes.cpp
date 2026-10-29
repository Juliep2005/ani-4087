#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>

#include <NKWindow/NKMain.h>

#include "../common/ExerciseMath.h"

using namespace nkentseu;
using namespace chapitre7;

static void ConfigureAppData(NkAppData& data) {
    data.appName = "Chapitre 7 - gestes";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

namespace {

struct GestureResult {
    Statistics withPrediction;
    Statistics withoutPrediction;
};

GestureResult MeasureGesture(double amplitude, double duration) {
    constexpr double hz = 72.0;
    constexpr double period = 1.0 / hz;
    const uint32 sampleCount =
        static_cast<uint32>(std::ceil(duration / period)) + 1u;
    GestureResult result;
    NkQuatf previous = NkQuatf::RotateY(NkAngle(0.0f));

    for (uint32 i = 0; i < sampleCount; ++i) {
        const double time = static_cast<double>(i) * period;
        if (time > duration) {
            break;
        }
        const double nextTime = time + period;
        const NkQuatf pose = NkQuatf::RotateY(
            NkAngle(static_cast<float32>(SmoothTurn(time, amplitude, duration))));
        const NkQuatf actual = NkQuatf::RotateY(NkAngle(
            static_cast<float32>(SmoothTurn(nextTime, amplitude, duration))));
        const NkVec3f velocity = i == 0u
            ? NkVec3f(0.0f, 0.0f, 0.0f)
            : AngularVelocity(previous, pose, static_cast<float32>(period));

        result.withPrediction.Add(QuaternionErrorDegrees(
            Extrapolate(pose, velocity, static_cast<float32>(period)), actual));
        result.withoutPrediction.Add(QuaternionErrorDegrees(pose, actual));
        previous = pose;
    }
    return result;
}

void WriteGesture(std::ofstream& file, const char* label, double amplitude,
                  double duration, const GestureResult& result) {
    const double ratio = result.withPrediction.Mean() > 0.0
        ? result.withoutPrediction.Mean() / result.withPrediction.Mean()
        : 0.0;
    file << label << " (" << amplitude << " deg en "
         << duration * 1000.0 << " ms) : avec "
         << result.withPrediction.Mean() << " deg, sans "
         << result.withoutPrediction.Mean() << " deg, rapport "
         << ratio << '\n';
    std::printf("%s : avec %.6f deg, sans %.6f deg, rapport %.4f\n",
                label, result.withPrediction.Mean(),
                result.withoutPrediction.Mean(), ratio);
}

} // namespace

int nkmain(const NkEntryState& state) {
    (void)state;
    std::ofstream output(ExerciseOutputPath(__FILE__, "gestes.txt"),
                         std::ios::trunc);
    if (!output) {
        std::fprintf(stderr, "Impossible d'ouvrir gestes.txt\n");
        return 1;
    }
    output.precision(3);
    output.setf(std::ios::fixed);
    std::printf("Echantillonnage simulé à 72 Hz; erreurs pendant les gestes.\n");

    const GestureResult rapid = MeasureGesture(90.0, 0.400);
    const GestureResult slow = MeasureGesture(45.0, 1.500);
    const GestureResult halfTurn = MeasureGesture(180.0, 0.250);
    WriteGesture(output, "rapide", 90.0, 0.400, rapid);
    WriteGesture(output, "lent", 45.0, 1.500, slow);
    WriteGesture(output, "demi-tour", 180.0, 0.250, halfTurn);
    return 0;
}
