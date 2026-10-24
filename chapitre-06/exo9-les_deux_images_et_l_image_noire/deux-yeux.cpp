#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>

#include <NKEvent/NkEventSystem.h>
#include <NKEvent/NkKeyboardEvent.h>
#include <NKEvent/NkWindowEvent.h>
#include <NKImage/Core/NkImage.h>
#include <NKLogger/NkLog.h>
#include <NKMath/NKMath.h>
#include <NKPlatform/NkPlatformDetect.h>
#include <NKRenderer/Core/NkCamera.h>
#include <NKRenderer/Materials/NkMaterial.h>
#include <NKRenderer/Mesh/NkMeshSystem.h>
#include <NKRenderer/NkRenderer.h>
#include <NKRenderer/Tools/Offscreen/NkOffscreenTarget.h>
#include <NKRenderer/Tools/Render2D/NkRender2D.h>
#include <NKRenderer/Tools/Render3D/NkRender3D.h>
#include <NKRHI/Core/NkDeviceFactory.h>
#include <NKWindow/Core/NkWindow.h>
#include <NKWindow/Core/NkWindowConfig.h>
#include <NKWindow/NKMain.h>

using namespace nkentseu;
using namespace nkentseu::renderer;

namespace {

constexpr uint32 kEyeWidth = 640;
constexpr uint32 kEyeHeight = 720;
constexpr uint32 kCaptureFrame = 30;
constexpr float32 kHalfIpd = 0.0315f;

void SubmitBox(NkRender3D& scene, NkMeshHandle cube, NkVec3f center,
              NkVec3f size, NkVec3f tint) {
    NkDrawCall3D draw;
    draw.mesh = cube;
    draw.transform = NkMat4f::Translate(center) * NkMat4f::Scale(size);
    draw.aabb = {
        {center.x - size.x * 0.5f, center.y - size.y * 0.5f, center.z - size.z * 0.5f},
        {center.x + size.x * 0.5f, center.y + size.y * 0.5f, center.z + size.z * 0.5f}
    };
    draw.tint = tint;
    draw.metallic = 0.0f;
    draw.roughness = 0.9f;
    scene.Submit(draw);
}

void SubmitRoom(NkRender3D& scene, NkMeshHandle cube, NkMeshHandle plane) {
    NkDrawCall3D floor;
    floor.mesh = plane;
    floor.transform = NkMat4f::Scale({2.0f, 1.0f, 2.0f});
    floor.aabb = {{-2.0f, 0.0f, -2.0f}, {2.0f, 0.0f, 2.0f}};
    floor.castShadow = false;
    floor.tint = {0.22f, 0.24f, 0.27f};
    floor.metallic = 0.0f;
    floor.roughness = 0.9f;
    scene.Submit(floor);

    const NkVec3f wall = {0.72f, 0.70f, 0.66f};
    SubmitBox(scene, cube, {0.0f, 1.25f, -2.0f}, {4.0f, 2.5f, 0.1f}, wall);
    SubmitBox(scene, cube, {-2.0f, 1.25f, 0.0f}, {0.1f, 2.5f, 4.0f}, wall);
    SubmitBox(scene, cube, {2.0f, 1.25f, 0.0f}, {0.1f, 2.5f, 4.0f}, wall);
    SubmitBox(scene, cube, {-1.55f, 1.25f, 2.0f}, {0.9f, 2.5f, 0.1f}, wall);
    SubmitBox(scene, cube, {1.55f, 1.25f, 2.0f}, {0.9f, 2.5f, 0.1f}, wall);
    SubmitBox(scene, cube, {0.0f, 2.25f, 2.0f}, {2.2f, 0.5f, 0.1f}, wall);
    SubmitBox(scene, cube, {0.0f, 1.0f, 1.93f}, {0.9f, 2.0f, 0.04f},
              {0.34f, 0.22f, 0.14f});
}

NkSceneContext MakeSceneContext(NkVec3f eyePosition) {
    NkCamera3DData cameraData;
    cameraData.position = eyePosition;
    cameraData.target = eyePosition + NkVec3f{0.0f, 0.0f, -1.0f};
    cameraData.up = {0.0f, 1.0f, 0.0f};
    cameraData.fovY = 70.0f;
    cameraData.aspect =
        static_cast<float32>(kEyeWidth) / static_cast<float32>(kEyeHeight);
    cameraData.nearPlane = 0.05f;
    cameraData.farPlane = 100.0f;

    NkSceneContext context;
    context.camera = NkCamera3D(cameraData);
    NkLightDesc light;
    light.type = NkLightType::NK_DIRECTIONAL;
    light.direction = {-0.4f, -1.0f, -0.3f};
    light.color = {1.0f, 0.95f, 0.85f};
    light.intensity = 3.0f;
    light.castShadow = false;
    context.lights.PushBack(light);
    return context;
}

bool RenderEye(NkRenderer& renderer, NkVec3f eyePosition) {
    renderer.FlushGraphRebuilds();
    if (!renderer.BeginFrame()) {
        return false;
    }

    NkRender3D* scene = renderer.GetRender3D();
    scene->ResetFrame();
    scene->BeginScene(MakeSceneContext(eyePosition));
    SubmitRoom(*scene, renderer.GetMeshSystem()->GetCube(),
               renderer.GetMeshSystem()->GetPlane());
    renderer.Present();
    renderer.EndFrame();
    return true;
}

struct DifferenceStats {
    uint32 pixelsOverOne = 0;
    uint8 maximum = 0;
};

bool CreateDifference(const char* firstPath, const char* secondPath,
                      const char* outputPath, DifferenceStats& stats) {
    NkImage first;
    NkImage second;
    if (!first.Load(firstPath) || !second.Load(secondPath)) {
        logger.Error("[Exercice 9] Chargement des captures impossible");
        return false;
    }
    if (first.Width() != static_cast<int32>(kEyeWidth) ||
        first.Height() != static_cast<int32>(kEyeHeight) ||
        second.Width() != first.Width() || second.Height() != first.Height()) {
        logger.Error("[Exercice 9] Les captures n'ont pas les dimensions attendues");
        return false;
    }

    NkImage difference =
        NkImage::Create(kEyeWidth, kEyeHeight, 4, 0x000000FFu);
    if (!difference.IsValid()) {
        logger.Error("[Exercice 9] Allocation de l'image difference impossible");
        return false;
    }

    stats = {};
    for (int32 y = 0; y < first.Height(); ++y) {
        for (int32 x = 0; x < first.Width(); ++x) {
            const NkColor left = first.GetPixel(x, y);
            const NkColor right = second.GetPixel(x, y);
            const uint8 pixelDifference = std::max({
                static_cast<uint8>(std::abs(static_cast<int32>(left.r) - right.r)),
                static_cast<uint8>(std::abs(static_cast<int32>(left.g) - right.g)),
                static_cast<uint8>(std::abs(static_cast<int32>(left.b) - right.b))
            });

            if (pixelDifference > 1) {
                ++stats.pixelsOverOne;
            }
            stats.maximum = std::max(stats.maximum, pixelDifference);

            const uint8 amplified = static_cast<uint8>(
                std::min<uint32>(static_cast<uint32>(pixelDifference) * 4u, 255u));
            difference.SetPixel(x, y, NkColor(amplified, amplified, amplified, 255));
        }
    }

    if (!difference.SavePNG(outputPath)) {
        logger.Error("[Exercice 9] Ecriture de l'image difference impossible");
        return false;
    }
    return true;
}

bool WriteDifferenceStats(const DifferenceStats& stereo,
                          const DifferenceStats& forgottenView) {
    std::ofstream output("difference.txt", std::ios::binary);
    if (!output) {
        logger.Error("[Exercice 9] Creation de difference.txt impossible");
        return false;
    }

    const uint32 totalPixels = kEyeWidth * kEyeHeight;
    output << "deux yeux : " << stereo.pixelsOverOne << " pixels sur "
           << totalPixels << ", ecart max " << static_cast<uint32>(stereo.maximum) << '\n';
    output << "vue oubliee : " << forgottenView.pixelsOverOne << " pixels sur "
           << totalPixels << ", ecart max "
           << static_cast<uint32>(forgottenView.maximum) << '\n';
    return static_cast<bool>(output);
}

bool CaptureForgottenView(NkRenderer& leftRenderer,
                          NkRenderer& rightRenderer,
                          NkOffscreenTarget& leftTarget,
                          NkOffscreenTarget& rightTarget,
                          NkVec3f headPosition,
                          DifferenceStats& forgottenView) {
    for (uint32 frame = 1; frame <= kCaptureFrame; ++frame) {
        if (!RenderEye(leftRenderer, headPosition) ||
            !RenderEye(rightRenderer, headPosition)) {
            logger.Error("[Exercice 9] Echec du rendu avec la vue oubliee");
            return false;
        }
    }

    constexpr const char* leftTemporary = "vue-oubliee-gauche-temp.png";
    constexpr const char* rightTemporary = "vue-oubliee-droite-temp.png";
    const bool leftCaptured = leftTarget.Capture(leftTemporary);
    const bool rightCaptured = rightTarget.Capture(rightTemporary);
    const bool differenceCreated =
        leftCaptured && rightCaptured &&
        CreateDifference(leftTemporary, rightTemporary,
                         "difference-vue-oubliee.png", forgottenView);
    const bool leftRemoved = std::remove(leftTemporary) == 0;
    const bool rightRemoved = std::remove(rightTemporary) == 0;
    if (!leftCaptured || !rightCaptured || !differenceCreated ||
        !leftRemoved || !rightRemoved) {
        logger.Error("[Exercice 9] Echec de la comparaison ou du nettoyage "
                     "des captures temporaires");
        return false;
    }
    return true;
}

bool ShowCapturedEyes(NkRenderer& renderer) {
    renderer.SetFinalColorTargetMirror(NkTextureHandle{}, false);
    renderer.SetRenderSizeOverride(0, 0);
    const NkTexHandle left = renderer.GetTextures()->Load("oeil-gauche.png");
    const NkTexHandle right = renderer.GetTextures()->Load("oeil-droit.png");
    if (!left.IsValid() || !right.IsValid()) {
        logger.Error("[Exercice 9] Rechargement des captures pour affichage impossible");
        return false;
    }

    NkEventSystem& events = NkEvents();
    bool running = true;
    auto closeCallback = events.AddEventCallbackGuard<NkWindowCloseEvent>(
        [&](NkWindowCloseEvent*) { running = false; });
    auto keyCallback = events.AddEventCallbackGuard<NkKeyPressEvent>(
        [&](NkKeyPressEvent* event) {
            if (event->GetKey() == NkKey::NK_ESCAPE) {
                running = false;
            }
        });

    std::puts("Captures affichees cote a cote. Appuyez sur Echap pour quitter.");
    while (running) {
        events.PollEvents();
        if (!running) {
            break;
        }
        if (!renderer.BeginFrame()) {
            continue;
        }

        NkRender2D* render2D = renderer.GetRender2D();
        render2D->DrawSprite({0.0f, 0.0f, 640.0f, 720.0f}, left);
        render2D->DrawSprite({640.0f, 0.0f, 640.0f, 720.0f}, right);
        renderer.Present();
        renderer.EndFrame();
    }
    return true;
}

int RunExercise() {
    NkWindowConfig windowConfig;
    windowConfig.title = "Exercice 9 - les deux images";
    windowConfig.width = 1280;
    windowConfig.height = 720;
    windowConfig.centered = true;
    windowConfig.resizable = false;

    NkWindow window(windowConfig);
    if (!window.IsValid()) {
        logger.Error("[Exercice 9] Creation de la fenetre impossible");
        return 1;
    }

    NkDeviceInitInfo deviceInfo{};
    deviceInfo.surface = window.GetSurfaceDesc();
    deviceInfo.width = static_cast<uint32>(window.GetSize().width);
    deviceInfo.height = static_cast<uint32>(window.GetSize().height);
    NkIDevice* device = NkDeviceFactory::CreateAutoDetect(deviceInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Exercice 9] Creation du device graphique impossible");
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 2;
    }

    const NkRendererConfig rendererConfig =
        NkRendererConfig::ForGame(deviceInfo.api, deviceInfo.width, deviceInfo.height);
    NkRenderer* leftRenderer = NkRenderer::Create(device, rendererConfig);
    NkRenderer* rightRenderer = NkRenderer::Create(device, rendererConfig);
    if (!leftRenderer || !rightRenderer ||
        !leftRenderer->Initialize() || !rightRenderer->Initialize()) {
        logger.Error("[Exercice 9] Initialisation des renderers impossible");
        NkRenderer::Destroy(rightRenderer);
        NkRenderer::Destroy(leftRenderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkOffscreenDesc targetDescription;
    targetDescription.width = kEyeWidth;
    targetDescription.height = kEyeHeight;
    targetDescription.colorFmt = NkGPUFormat::NK_RGBA8_SRGB;
    targetDescription.readback = true;
    NkOffscreenTarget* leftTarget = leftRenderer->CreateOffscreen(targetDescription);
    NkOffscreenTarget* rightTarget = rightRenderer->CreateOffscreen(targetDescription);
    if (!leftTarget || !rightTarget ||
        !leftTarget->IsValid() || !rightTarget->IsValid()) {
        logger.Error("[Exercice 9] Creation des cibles hors ecran impossible");
        if (leftTarget) {
            leftRenderer->DestroyOffscreen(leftTarget);
        }
        if (rightTarget) {
            rightRenderer->DestroyOffscreen(rightTarget);
        }
        NkRenderer::Destroy(rightRenderer);
        NkRenderer::Destroy(leftRenderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 4;
    }

    leftRenderer->SetRenderSizeOverride(kEyeWidth, kEyeHeight);
    rightRenderer->SetRenderSizeOverride(kEyeWidth, kEyeHeight);
    leftRenderer->SetFinalColorTargetMirror(
        leftRenderer->GetTextures()->GetRHIHandle(leftTarget->GetColorResult()), true);
    rightRenderer->SetFinalColorTargetMirror(
        rightRenderer->GetTextures()->GetRHIHandle(rightTarget->GetColorResult()), true);

    const NkVec3f headPosition = {0.30f, 1.70f, 1.20f};
    const NkVec3f leftEye = headPosition - NkVec3f{1.0f, 0.0f, 0.0f} * kHalfIpd;
    const NkVec3f rightEye = headPosition + NkVec3f{1.0f, 0.0f, 0.0f} * kHalfIpd;
    bool captureSucceeded = true;
    DifferenceStats stereo;
    for (uint32 frame = 1; frame <= kCaptureFrame; ++frame) {
        if (!RenderEye(*leftRenderer, leftEye) ||
            !RenderEye(*rightRenderer, rightEye)) {
            logger.Error("[Exercice 9] Echec du rendu stereo");
            captureSucceeded = false;
            break;
        }
        if (frame == kCaptureFrame) {
            captureSucceeded = leftTarget->Capture("oeil-gauche.png") &&
                               rightTarget->Capture("oeil-droit.png") &&
                               CreateDifference("oeil-gauche.png", "oeil-droit.png",
                                               "difference.png", stereo);
            if (!captureSucceeded) {
                logger.Error("[Exercice 9] Capture et comparaison stereo impossibles");
            }
        }
    }

    DifferenceStats forgottenView;
    if (captureSucceeded) {
        captureSucceeded = CaptureForgottenView(
            *leftRenderer, *rightRenderer, *leftTarget, *rightTarget,
            headPosition, forgottenView);
    }
    if (captureSucceeded) {
        captureSucceeded = WriteDifferenceStats(stereo, forgottenView);
    }
    if (captureSucceeded) {
        std::printf("deux yeux : %u pixels sur %u, ecart max %u\n",
                    stereo.pixelsOverOne, kEyeWidth * kEyeHeight,
                    static_cast<uint32>(stereo.maximum));
        std::printf("vue oubliee : %u pixels sur %u, ecart max %u\n",
                    forgottenView.pixelsOverOne, kEyeWidth * kEyeHeight,
                    static_cast<uint32>(forgottenView.maximum));
    }

    device->WaitIdle();
    const char* captureOnly = std::getenv("MASALLE_CAPTURE_ONLY");
    if (captureSucceeded &&
        !(captureOnly && captureOnly[0] == '1' && captureOnly[1] == '\0')) {
        captureSucceeded = ShowCapturedEyes(*leftRenderer);
    }

    leftRenderer->SetFinalColorTargetMirror(NkTextureHandle{}, false);
    rightRenderer->SetFinalColorTargetMirror(NkTextureHandle{}, false);
    leftRenderer->SetRenderSizeOverride(0, 0);
    rightRenderer->SetRenderSizeOverride(0, 0);
    device->WaitIdle();
    leftRenderer->DestroyOffscreen(leftTarget);
    rightRenderer->DestroyOffscreen(rightTarget);
    NkRenderer::Destroy(rightRenderer);
    NkRenderer::Destroy(leftRenderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return captureSucceeded ? 0 : 5;
}

}

int nkmain(const NkEntryState&) {
    return RunExercise();
}
