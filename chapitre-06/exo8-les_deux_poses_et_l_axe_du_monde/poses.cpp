#include <cmath>
#include <cstdio>

#include <NKMath/NkAngle.h>
#include <NKMath/NkQuat.h>
#include <NKMath/NkVec.h>

using nkentseu::math::NkAngle;
using nkentseu::math::NkQuatf;
using nkentseu::math::NkVec3f;

namespace {

constexpr float kHalfIpdMeters = 0.0315f;
constexpr float kDisplayZeroThreshold = 0.00005f;

struct HeadPose {
    const char* label;
    NkQuatf orientation;
};

float DisplayValue(float value) {
    return std::fabs(value) < kDisplayZeroThreshold ? 0.0f : value;
}

NkVec3f EyePosition(const NkVec3f& headPosition,
                    const NkVec3f& rightAxis,
                    float offset) {
    return headPosition + rightAxis * offset;
}

float Distance(const NkVec3f& first, const NkVec3f& second) {
    const float dx = first.x - second.x;
    const float dy = first.y - second.y;
    const float dz = first.z - second.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

void PrintEyePositions(const char* poseLabel,
                       const char* methodLabel,
                       const NkVec3f& leftEye,
                       const NkVec3f& rightEye) {
    std::printf(
        "%s, %s : G (%.4f, %.4f, %.4f) D (%.4f, %.4f, %.4f)\n",
        poseLabel,
        methodLabel,
        DisplayValue(leftEye.x),
        DisplayValue(leftEye.y),
        DisplayValue(leftEye.z),
        DisplayValue(rightEye.x),
        DisplayValue(rightEye.y),
        DisplayValue(rightEye.z));
}

}  // namespace

int main() {
    constexpr int kPoseCount = 3;
    const NkVec3f headPosition(0.30f, 1.70f, 1.20f);
    const NkVec3f worldRight(1.0f, 0.0f, 0.0f);
    const HeadPose poses[kPoseCount] = {
        {"neutre", NkQuatf::Identity()},
        {"roulis 30", NkQuatf::RotateZ(NkAngle(30.0f))},
        {"lacet 90", NkQuatf::RotateY(NkAngle(90.0f))}
    };
    float gapsMillimeters[kPoseCount] = {};

    for (int i = 0; i < kPoseCount; ++i) {
        const NkVec3f headRight = poses[i].orientation * worldRight;
        const NkVec3f leftEyeAlongHead =
            EyePosition(headPosition, headRight, -kHalfIpdMeters);
        const NkVec3f rightEyeAlongHead =
            EyePosition(headPosition, headRight, kHalfIpdMeters);
        const NkVec3f leftEyeAlongWorld =
            EyePosition(headPosition, worldRight, -kHalfIpdMeters);
        const NkVec3f rightEyeAlongWorld =
            EyePosition(headPosition, worldRight, kHalfIpdMeters);

        PrintEyePositions(
            poses[i].label, "tete", leftEyeAlongHead, rightEyeAlongHead);
        PrintEyePositions(
            poses[i].label, "monde", leftEyeAlongWorld, rightEyeAlongWorld);

        gapsMillimeters[i] =
            Distance(leftEyeAlongHead, leftEyeAlongWorld) * 1000.0f;
    }

    for (int i = 0; i < kPoseCount; ++i) {
        std::printf("%s : ecart %.1f mm\n",
                    poses[i].label,
                    gapsMillimeters[i]);
    }

    return 0;
}
