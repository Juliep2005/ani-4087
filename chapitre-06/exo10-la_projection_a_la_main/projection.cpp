#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include <NKMath/NKMath.h>
#include <NKRenderer/Core/NkCamera.h>

using nkentseu::math::NkAngle;
using nkentseu::math::NkMat4f;
using nkentseu::math::NkTan;
using nkentseu::math::NkAtan;
using nkentseu::renderer::NkCamera3D;

namespace {

constexpr float kVerticalFovDegrees = 70.0f;
constexpr float kAspect = 1280.0f / 720.0f;
constexpr float kNearPlane = 0.05f;
constexpr float kFarPlane = 100.0f;
constexpr float kPi = 3.14159265358979323846f;

struct Fov {
    float left;
    float right;
    float up;
    float down;
};

NkMat4f Projection(float left, float right, float up, float down,
                   float nearPlane, float farPlane) {
    const float tanLeft = NkTan(left);
    const float tanRight = NkTan(right);
    const float tanUp = NkTan(up);
    const float tanDown = NkTan(down);
    const float tanWidth = tanRight - tanLeft;
    const float tanHeight = tanUp - tanDown;

    NkMat4f result = NkMat4f::Zero();
    result.mat[0][0] = 2.0f / tanWidth;
    result.mat[1][1] = 2.0f / tanHeight;
    result.mat[2][0] = (tanRight + tanLeft) / tanWidth;
    result.mat[2][1] = (tanUp + tanDown) / tanHeight;
    result.mat[2][2] = (farPlane + nearPlane) / (nearPlane - farPlane);
    result.mat[2][3] = -1.0f;
    result.mat[3][2] =
        2.0f * farPlane * nearPlane / (nearPlane - farPlane);
    return result;
}

Fov SymmetricFov() {
    const float halfVertical = NkAngle(kVerticalFovDegrees).Rad() * 0.5f;
    const float halfHorizontal = NkAtan(NkTan(halfVertical) * kAspect);
    return {-halfHorizontal, halfHorizontal, halfVertical, -halfVertical};
}

bool ParseAngle(const char* text, float& angle) {
    errno = 0;
    char* end = nullptr;
    const float value = std::strtof(text, &end);
    if (end == text || *end != '\0' || errno == ERANGE ||
        !std::isfinite(value)) {
        return false;
    }
    angle = value;
    return true;
}

bool IsValidAsymmetricFov(const Fov& fov) {
    const float halfPi = kPi * 0.5f;
    return fov.left < 0.0f && fov.right > 0.0f && fov.up > 0.0f &&
           fov.down < 0.0f && fov.left > -halfPi && fov.right < halfPi &&
           fov.up < halfPi && fov.down > -halfPi;
}

float WriteDifferences(std::ostream& output, const char* label,
                       const NkMat4f& manual, const NkMat4f& reference) {
    float maximum = 0.0f;
    output << std::setprecision(9);
    for (int row = 0; row < 4; ++row) {
        output << label << ", ligne " << row << " :";
        for (int column = 0; column < 4; ++column) {
            // NKMath stores coefficients as mat[colonne][ligne].
            const float difference =
                std::fabs(manual.mat[column][row] -
                          reference.mat[column][row]);
            maximum = std::max(maximum, difference);
            output << ' ' << difference;
        }
        output << '\n';
    }
    output << label << " : plus grand ecart " << maximum << '\n';
    return maximum;
}

int RunSymmetricOnly() {
    const Fov fov = SymmetricFov();
    const NkMat4f manual =
        Projection(fov.left, fov.right, fov.up, fov.down,
                   kNearPlane, kFarPlane);
    const NkMat4f reference = NkMat4f::Perspective(
        NkAngle(kVerticalFovDegrees), kAspect, kNearPlane, kFarPlane);
    WriteDifferences(std::cout, "symetrique", manual, reference);
    return 0;
}

int RunBoth(const Fov& questFov) {
    const Fov symmetricFov = SymmetricFov();
    const NkMat4f symmetricManual =
        Projection(symmetricFov.left, symmetricFov.right,
                   symmetricFov.up, symmetricFov.down,
                   kNearPlane, kFarPlane);
    const NkMat4f symmetricReference = NkMat4f::Perspective(
        NkAngle(kVerticalFovDegrees), kAspect, kNearPlane, kFarPlane);

    const NkMat4f questManual =
        Projection(questFov.left, questFov.right,
                   questFov.up, questFov.down,
                   kNearPlane, kFarPlane);
    NkCamera3D questCamera;
    questCamera.SetNearFar(kNearPlane, kFarPlane);
    questCamera.SetFovAsym(questFov.left, questFov.right,
                           questFov.up, questFov.down);

    std::ofstream output("ecarts.txt", std::ios::trunc);
    if (!output) {
        std::fprintf(stderr, "Impossible de creer ecarts.txt.\n");
        return 3;
    }
    WriteDifferences(output, "symetrique", symmetricManual,
                     symmetricReference);
    WriteDifferences(output, "quest", questManual, questCamera.GetProj());
    output.close();
    if (!output) {
        std::fprintf(stderr, "Echec de l'ecriture de ecarts.txt.\n");
        return 4;
    }

    std::ifstream results("ecarts.txt");
    if (!results) {
        std::fprintf(stderr, "Impossible de relire ecarts.txt.\n");
        return 5;
    }
    std::cout << results.rdbuf();
    if (results.bad()) {
        std::fprintf(stderr, "Echec de lecture de ecarts.txt.\n");
        return 5;
    }
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc == 2 && std::string(argv[1]) == "--symmetric-only") {
        return RunSymmetricOnly();
    }
    if (argc != 5) {
        std::fprintf(
            stderr,
            "Usage: projection.exe <gauche_rad> <droite_rad> "
            "<haut_rad> <bas_rad>\n"
            "Fournissez les quatre angles Quest en radians; "
            "les angles gauche et bas sont negatifs.\n"
            "Pour verifier seulement l'epreuve symetrique: "
            "projection.exe --symmetric-only\n");
        return 2;
    }

    Fov questFov{};
    if (!ParseAngle(argv[1], questFov.left) ||
        !ParseAngle(argv[2], questFov.right) ||
        !ParseAngle(argv[3], questFov.up) ||
        !ParseAngle(argv[4], questFov.down) ||
        !IsValidAsymmetricFov(questFov)) {
        std::fprintf(
            stderr,
            "Angles invalides: fournissez quatre radians finis dans "
            "l'ordre gauche, droite, haut, bas; gauche et bas negatifs, "
            "droite et haut positifs, tous entre -pi/2 et pi/2.\n");
        return 2;
    }

    return RunBoth(questFov);
}
