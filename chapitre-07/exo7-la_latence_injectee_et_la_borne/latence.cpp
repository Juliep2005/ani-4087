#include <array>
#include <cmath>
#include <cstdio>
#include <fstream>

#include <NKWindow/NKMain.h>

#include "../common/ExerciseMath.h"

using namespace nkentseu;
using namespace chapitre7;

static void ConfigureAppData(NkAppData& data) {
    data.appName = "Chapitre 7 - latence";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

namespace {

constexpr double kHz = 72.0;
constexpr double kPeriod = 1.0 / kHz;
constexpr double kGestureStart = 1.0;
constexpr double kGestureDuration = 0.400;
constexpr size_t kHistorySize = 64u;

struct TimedPose {
    double time = 0.0;
    NkQuatf pose;
    NkVec3f velocity{0.0f, 0.0f, 0.0f};
};

class PoseHistory {
public:
    void Add(const TimedPose& sample) {
        mSamples[mHead] = sample;
        mHead = (mHead + 1u) % kHistorySize;
        if (mCount < kHistorySize) {
            ++mCount;
        }
    }

    const TimedPose& SampleAt(double targetTime) const {
        const size_t oldest = (mHead + kHistorySize - mCount) % kHistorySize;
        const TimedPose* selected = &mSamples[oldest];
        for (size_t i = 0u; i < mCount; ++i) {
            const TimedPose& sample = mSamples[(oldest + i) % kHistorySize];
            if (sample.time > targetTime) {
                break;
            }
            selected = &sample;
        }
        return *selected;
    }

private:
    std::array<TimedPose, kHistorySize> mSamples{};
    size_t mHead = 0u;
    size_t mCount = 0u;
};

double YawDegrees(const NkQuatf& pose) {
    return 2.0 * std::atan2(static_cast<double>(pose.y),
                            static_cast<double>(pose.w)) *
           180.0 / 3.14159265358979323846;
}

NkQuatf PoseAt(double time) {
    const double gestureTime = time - kGestureStart;
    return NkQuatf::RotateY(NkAngle(static_cast<float32>(
        SmoothTurn(gestureTime, 90.0, kGestureDuration))));
}

std::vector<TimedPose> BuildSamples() {
    std::vector<TimedPose> samples;
    const uint32 sampleCount = static_cast<uint32>(3.0 * kHz) + 1u;
    NkQuatf previous = PoseAt(0.0);
    for (uint32 i = 0u; i < sampleCount; ++i) {
        const double time = static_cast<double>(i) * kPeriod;
        const NkQuatf pose = PoseAt(time);
        const NkVec3f velocity = i == 0u
            ? NkVec3f(0.0f, 0.0f, 0.0f)
            : AngularVelocity(previous, pose, static_cast<float32>(kPeriod));
        samples.push_back({time, pose, velocity});
        previous = pose;
    }
    return samples;
}

struct ErrorResult {
    Statistics withPrediction;
    Statistics withoutPrediction;
};

ErrorResult MeasureLatency(const std::vector<TimedPose>& samples, double latency,
                           bool clampPrediction) {
    ErrorResult result;
    PoseHistory history;
    size_t nextSample = 0u;
    const uint32 frameCount =
        static_cast<uint32>(std::floor(0.9 / kPeriod)) + 1u;
    for (uint32 i = 0u; i < frameCount; ++i) {
        const double now = kGestureStart + static_cast<double>(i) * kPeriod;
        while (nextSample < samples.size() &&
               samples[nextSample].time <= now + 1e-12) {
            history.Add(samples[nextSample]);
            ++nextSample;
        }
        const double displayTime = now + kPeriod;
        const TimedPose& sample = history.SampleAt(now - latency);
        float32 horizon = static_cast<float32>(displayTime - sample.time);
        if (clampPrediction) {
            horizon = math::NkClamp(horizon, 0.0f, 0.1f);
        }
        const NkQuatf predicted = Extrapolate(sample.pose, sample.velocity, horizon);
        const NkQuatf actual = PoseAt(displayTime);
        result.withPrediction.Add(QuaternionErrorDegrees(predicted, actual));
        result.withoutPrediction.Add(QuaternionErrorDegrees(sample.pose, actual));
    }
    return result;
}

struct BoundSample {
    double sampleTime = 0.0;
    double horizonMs = 0.0;
    double predictedYaw = 0.0;
    double actualYaw = 0.0;
};

BoundSample MeasureOneSecondLatency(const std::vector<TimedPose>& samples,
                                    bool clampPrediction) {
    const uint32 frameCount = static_cast<uint32>(2.0 * kHz) + 1u;
    PoseHistory history;
    size_t nextSample = 0u;
    const TimedPose* closest = nullptr;
    double closestDisplayTime = 0.0;
    double closestDistance = 1e9;
    for (uint32 i = 0u; i < frameCount; ++i) {
        const double now = static_cast<double>(i) * kPeriod;
        while (nextSample < samples.size() &&
               samples[nextSample].time <= now + 1e-12) {
            history.Add(samples[nextSample]);
            ++nextSample;
        }
        const TimedPose& sample = history.SampleAt(now - 1.0);
        const double distance = std::abs(sample.time - 1.2);
        if (distance < closestDistance) {
            closest = &sample;
            closestDisplayTime = now + kPeriod;
            closestDistance = distance;
        }
    }

    double horizon = closestDisplayTime - closest->time;
    if (clampPrediction) {
        horizon = math::NkClamp(horizon, 0.0, 0.1);
    }
    const NkQuatf predicted = Extrapolate(
        closest->pose, closest->velocity, static_cast<float32>(horizon));
    return {closest->time, horizon * 1000.0, YawDegrees(predicted),
            YawDegrees(PoseAt(closestDisplayTime))};
}

} // namespace

int nkmain(const NkEntryState& state) {
    (void)state;
    const std::vector<TimedPose> samples = BuildSamples();
    const double latenciesMs[] = {0.0, 10.0, 20.0, 40.0, 80.0};
    std::ofstream output(ExerciseOutputPath(__FILE__, "latences.txt"),
                         std::ios::trunc);
    if (!output) {
        std::fprintf(stderr, "Impossible d'ouvrir latences.txt\n");
        return 1;
    }
    output.precision(3);
    output.setf(std::ios::fixed);

    for (double latencyMs : latenciesMs) {
        const ErrorResult result =
            MeasureLatency(samples, latencyMs / 1000.0, true);
        output << "latence " << latencyMs << " ms : avec "
               << result.withPrediction.Mean() << " deg (max "
               << result.withPrediction.Maximum() << "), sans "
               << result.withoutPrediction.Mean() << " deg (max "
               << result.withoutPrediction.Maximum() << ")\n";
        std::printf("latence %.0f ms : avec %.6f deg (max %.6f), "
                    "sans %.6f deg (max %.6f)\n",
                    latencyMs, result.withPrediction.Mean(),
                    result.withPrediction.Maximum(),
                    result.withoutPrediction.Mean(),
                    result.withoutPrediction.Maximum());
    }

    const BoundSample unbounded = MeasureOneSecondLatency(samples, false);
    const BoundSample bounded = MeasureOneSecondLatency(samples, true);
    output << "sans borne : echantillon a " << unbounded.sampleTime
           << " s, horizon " << unbounded.horizonMs << " ms, predit "
           << unbounded.predictedYaw << " deg, vrai "
           << unbounded.actualYaw << " deg\n";
    output << "avec borne : echantillon a " << bounded.sampleTime
           << " s, horizon " << bounded.horizonMs << " ms, predit "
           << bounded.predictedYaw << " deg, vrai "
           << bounded.actualYaw << " deg\n";
    std::printf("sans borne : echantillon a %.6f s, horizon %.3f ms, "
                "predit %.6f deg, vrai %.6f deg\n",
                unbounded.sampleTime, unbounded.horizonMs,
                unbounded.predictedYaw, unbounded.actualYaw);
    std::printf("avec borne : echantillon a %.6f s, horizon %.3f ms, "
                "predit %.6f deg, vrai %.6f deg\n",
                bounded.sampleTime, bounded.horizonMs,
                bounded.predictedYaw, bounded.actualYaw);
    return 0;
}
