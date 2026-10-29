#pragma once

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <limits>
#include <string>
#include <vector>

#include <NKMath/NKMath.h>

namespace chapitre7 {

using namespace nkentseu;
using namespace nkentseu::math;

inline bool ReadEnvDouble(const char* name, double fallback, double& value) {
    const char* text = std::getenv(name);
    if (text == nullptr || *text == '\0') {
        value = fallback;
        return true;
    }

    char* end = nullptr;
    value = std::strtod(text, &end);
    if (end == text || *end != '\0' || !std::isfinite(value)) {
        std::fprintf(stderr, "Valeur invalide pour %s: %s\n", name, text);
        return false;
    }
    return true;
}

inline uint64 ReadEnvU64(const char* name, uint64 fallback) {
    const char* text = std::getenv(name);
    if (text == nullptr || *text == '\0') {
        return fallback;
    }

    char* end = nullptr;
    const unsigned long long parsed = std::strtoull(text, &end, 10);
    if (end == text || *end != '\0') {
        std::fprintf(stderr, "Valeur invalide pour %s: %s\n", name, text);
        return fallback;
    }
    return static_cast<uint64>(parsed);
}

inline bool EnvFlag(const char* name) {
    const char* value = std::getenv(name);
    return value != nullptr && value[0] != '\0' && value[0] != '0';
}

inline std::string ExerciseOutputPath(const char* sourceFile,
                                      const char* outputFile) {
    return (std::filesystem::absolute(sourceFile).parent_path() /
            outputFile).string();
}

class Statistics {
public:
    void Add(double value) {
        mValues.push_back(value);
        mSum += value;
        mMaximum = std::max(mMaximum, value);
    }

    size_t Count() const {
        return mValues.size();
    }

    double Mean() const {
        return mValues.empty() ? 0.0 : mSum / static_cast<double>(mValues.size());
    }

    double Maximum() const {
        return mValues.empty() ? 0.0 : mMaximum;
    }

    size_t CountAbove(double threshold) const {
        return static_cast<size_t>(std::count_if(
            mValues.begin(), mValues.end(),
            [threshold](double value) { return value > threshold; }));
    }

    double Median() const {
        if (mValues.empty()) {
            return 0.0;
        }
        std::vector<double> sorted = mValues;
        const size_t middle = sorted.size() / 2u;
        std::nth_element(sorted.begin(), sorted.begin() + middle, sorted.end());
        if ((sorted.size() & 1u) != 0u) {
            return sorted[middle];
        }
        const double upper = sorted[middle];
        const double lower = *std::max_element(sorted.begin(), sorted.begin() + middle);
        return (lower + upper) * 0.5;
    }

    double StandardDeviation() const {
        if (mValues.empty()) {
            return 0.0;
        }
        const double mean = Mean();
        double squared = 0.0;
        for (double value : mValues) {
            const double difference = value - mean;
            squared += difference * difference;
        }
        return std::sqrt(squared / static_cast<double>(mValues.size()));
    }

private:
    std::vector<double> mValues;
    double mSum = 0.0;
    double mMaximum = -std::numeric_limits<double>::infinity();
};

inline NkVec3f AngularVelocity(const NkQuatf& from, const NkQuatf& to,
                               float32 dtSeconds, bool shortestPath = true) {
    if (dtSeconds <= 0.0f) {
        return {0.0f, 0.0f, 0.0f};
    }

    NkQuatf delta = (to * from.Conjugate()).Normalized();
    if (shortestPath && delta.w < 0.0f) {
        delta = NkQuatf(-delta.x, -delta.y, -delta.z, -delta.w);
    }
    const float32 angleRad = delta.Angle().Rad();
    if (angleRad <= 1e-6f) {
        return {0.0f, 0.0f, 0.0f};
    }
    return delta.Axis() * (angleRad / dtSeconds);
}

inline NkQuatf Extrapolate(const NkQuatf& orientation, const NkVec3f& velocity,
                           float32 dtSeconds) {
    const float32 speedRad = velocity.Len();
    if (speedRad <= 1e-6f) {
        return orientation;
    }
    const NkVec3f axis = velocity * (1.0f / speedRad);
    const NkQuatf delta(NkAngle::FromRad(speedRad * dtSeconds), axis);
    return (delta * orientation).Normalized();
}

inline double SmoothTurn(double timeSeconds, double amplitudeDegrees,
                         double durationSeconds) {
    constexpr double pi = 3.14159265358979323846;
    if (timeSeconds <= 0.0) {
        return 0.0;
    }
    if (timeSeconds >= durationSeconds) {
        return amplitudeDegrees;
    }
    return amplitudeDegrees * 0.5 *
           (1.0 - std::cos(pi * timeSeconds / durationSeconds));
}

inline double QuaternionErrorDegrees(const NkQuatf& estimate,
                                     const NkQuatf& actual) {
    return (actual * estimate.Conjugate()).Normalized().Angle().Deg();
}

} // namespace chapitre7
