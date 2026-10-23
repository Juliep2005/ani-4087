#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <NKEvent/NkEventSystem.h>
#include <NKEvent/NkKeyboardEvent.h>
#include <NKEvent/NkMouseEvent.h>
#include <NKEvent/NkWindowEvent.h>
#include <NKLogger/NkLog.h>
#include <NKMath/NKMath.h>
#include <NKPlatform/NkPlatformDetect.h>
#include <NKRenderer/Core/NkCamera.h>
#include <NKRenderer/Core/NkTextureLibrary.h>
#include <NKRenderer/Materials/NkMaterial.h>
#include <NKRenderer/Mesh/NkMeshSystem.h>
#include <NKRenderer/NkRenderer.h>
#include <NKRenderer/Tools/Render3D/NkRender3D.h>
#include <NKRenderer/Tools/Offscreen/NkOffscreenTarget.h>
#include <NKRHI/Core/NkDeviceFactory.h>
#include <NKWindow/Core/NkWindow.h>
#include <NKWindow/Core/NkWindowConfig.h>
#include <NKWindow/NKMain.h>

using namespace nkentseu;
using namespace nkentseu::renderer;

namespace {

constexpr float32 kSensitivity = 0.002f;
constexpr float32 kPitchDegrees = 30.0f;
constexpr float32 kRadiansToDegrees = 57.29577951308232f;

enum class RotationOrder {
    YawThenPitch,
    PitchThenYaw
};

struct CaptureConfig {
    uint32 frame = 0;
    const char* path = nullptr;
};

const char* HeightOrderName(RotationOrder order) {
    return order == RotationOrder::YawThenPitch
        ? "lacet*tangage"
        : "tangage*lacet";
}

NkQuatf MakeOrientation(float32 yawDegrees, float32 pitchDegrees,
                        RotationOrder order) {
    const NkQuatf yaw = NkQuatf::RotateY(NkAngle(yawDegrees));
    const NkQuatf pitch = NkQuatf::RotateX(NkAngle(pitchDegrees));
    return (order == RotationOrder::YawThenPitch ? yaw * pitch : pitch * yaw)
        .Normalized();
}

const char* OrderName(RotationOrder order) {
    return order == RotationOrder::YawThenPitch
        ? "lacet * tangage"
        : "tangage * lacet";
}

void PrintDirection(float32 yawDegrees, float32 pitchDegrees,
                    RotationOrder order, const NkVec3f& direction) {
    std::printf("regard: ordre=%s, lacet=%.2f deg, tangage=%.2f deg, "
                "direction=(%.4f, %.4f, %.4f)\n",
                OrderName(order),
                static_cast<double>(yawDegrees),
                static_cast<double>(pitchDegrees),
                static_cast<double>(direction.x),
                static_cast<double>(direction.y),
                static_cast<double>(direction.z));
    std::fflush(stdout);
}

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

void SubmitRoom(NkRender3D& scene, NkMeshHandle cube, NkMeshHandle plane,
                NkMeshHandle cone) {
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
    const NkVec3f ceiling = {0.48f, 0.51f, 0.57f};
    const NkVec3f doorColor = {0.20f, 0.38f, 0.58f};
    SubmitBox(scene, cube, {0.0f, 2.45f, 0.0f}, {4.0f, 0.1f, 4.0f}, ceiling);
    SubmitBox(scene, cube, {0.0f, 1.25f, -2.0f}, {4.0f, 2.5f, 0.1f}, wall);
    SubmitBox(scene, cube, {-2.0f, 1.25f, -1.25f}, {0.1f, 2.5f, 1.5f}, wall);
    SubmitBox(scene, cube, {-2.0f, 1.25f, 1.25f}, {0.1f, 2.5f, 1.5f}, wall);
    SubmitBox(scene, cube, {-2.0f, 2.3f, 0.0f}, {0.1f, 0.4f, 1.0f}, wall);
    SubmitBox(scene, cube, {-1.93f, 1.05f, 0.0f}, {0.05f, 2.1f, 0.9f}, doorColor);
    SubmitBox(scene, cube, {-1.88f, 1.05f, 0.30f}, {0.04f, 0.08f, 0.04f},
              {0.92f, 0.70f, 0.24f});
    SubmitBox(scene, cube, {2.0f, 1.25f, 0.0f}, {0.1f, 2.5f, 4.0f}, wall);
    SubmitBox(scene, cube, {-1.55f, 1.25f, 2.0f}, {0.9f, 2.5f, 0.1f}, wall);
    SubmitBox(scene, cube, {1.55f, 1.25f, 2.0f}, {0.9f, 2.5f, 0.1f}, wall);
    SubmitBox(scene, cube, {0.0f, 2.25f, 2.0f}, {2.2f, 0.5f, 0.1f}, wall);

    NkDrawCall3D marker;
    marker.mesh = cone;
    marker.transform =
        NkMat4f::Translate({0.0f, 0.55f, -1.25f}) * NkMat4f::Scale({0.45f, 0.9f, 0.45f});
    marker.aabb = {{-0.5f, 0.0f, -1.75f}, {0.5f, 1.1f, -0.75f}};
    marker.tint = {0.90f, 0.28f, 0.12f};
    marker.metallic = 0.0f;
    marker.roughness = 0.8f;
    scene.Submit(marker);
}

bool ReadFiniteFloat(const char* text, float32& value) {
    if (!text || !*text) {
        return false;
    }

    char* end = nullptr;
    errno = 0;
    const float parsed = std::strtof(text, &end);
    if (errno == ERANGE || end == text || *end != '\0' || !std::isfinite(parsed)) {
        return false;
    }

    value = static_cast<float32>(parsed);
    return true;
}

bool ReadCaptureConfig(CaptureConfig& capture) {
    const char* frameText = std::getenv("MASALLE_SHOT");
    const char* path = std::getenv("MASALLE_SHOT_PATH");
    if (!frameText && !path) {
        return true;
    }
    if (!frameText || !path || !*path) {
        logger.Error("[Exercice 6] MASALLE_SHOT et MASALLE_SHOT_PATH doivent etre definis ensemble");
        return false;
    }

    char* end = nullptr;
    errno = 0;
    const unsigned long long frame = std::strtoull(frameText, &end, 10);
    if (errno == ERANGE || end == frameText || *end != '\0' ||
        frame == 0 || frame > 0xffffffffULL) {
        logger.Error("[Exercice 6] MASALLE_SHOT doit etre un numero d'image positif");
        return false;
    }

    capture.frame = static_cast<uint32>(frame);
    capture.path = path;
    return true;
}

void PrintHeights() {
    constexpr float32 yawAngles[] = {0.0f, 45.0f, 90.0f, 135.0f, 180.0f};
    constexpr RotationOrder orders[] = {
        RotationOrder::YawThenPitch,
        RotationOrder::PitchThenYaw
    };
    const NkVec3f forward = {0.0f, 0.0f, -1.0f};

    for (RotationOrder order : orders) {
        for (float32 yawDegrees : yawAngles) {
            const NkVec3f direction =
                MakeOrientation(yawDegrees, kPitchDegrees, order) * forward;
            const float32 horizontal =
                std::sqrt(direction.x * direction.x + direction.z * direction.z);
            const float32 heightDegrees =
                std::atan2(direction.y, horizontal) * kRadiansToDegrees;
            std::printf("%s, lacet %.0f : hauteur %.4f deg\n",
                        HeightOrderName(order),
                        static_cast<double>(yawDegrees),
                        static_cast<double>(heightDegrees));
        }
    }
    std::fflush(stdout);
}

int RunRoom(float32 initialYawDegrees, bool fixedYaw, RotationOrder initialOrder,
            const CaptureConfig& capture) {
    NkWindowConfig windowConfig;
    windowConfig.title = "Exercice 6 - ordre des rotations";
    windowConfig.width = 1280;
    windowConfig.height = 720;
    windowConfig.centered = true;
    windowConfig.resizable = true;

    NkWindow window(windowConfig);
    if (!window.IsValid()) {
        logger.Error("[Exercice 6] Creation de la fenetre impossible");
        return 1;
    }

    NkDeviceInitInfo deviceInfo{};
    deviceInfo.surface = window.GetSurfaceDesc();
    deviceInfo.width = static_cast<uint32>(window.GetSize().width);
    deviceInfo.height = static_cast<uint32>(window.GetSize().height);
    NkIDevice* device = NkDeviceFactory::CreateAutoDetect(deviceInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Exercice 6] Creation du device graphique impossible");
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 2;
    }

    NkRendererConfig rendererConfig =
        NkRendererConfig::ForGame(deviceInfo.api, deviceInfo.width, deviceInfo.height);
    NkRenderer* renderer = NkRenderer::Create(device, rendererConfig);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Exercice 6] Initialisation du renderer impossible");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkRender3D* scene = renderer->GetRender3D();
    const NkMeshHandle cube = renderer->GetMeshSystem()->GetCube();
    const NkMeshHandle plane = renderer->GetMeshSystem()->GetPlane();
    const     NkMeshHandle cone = renderer->GetMeshSystem()->GetCone();
    NkEventSystem& events = NkEvents();

    bool running = true;
    RotationOrder order = initialOrder;
    float32 yawDegrees = initialYawDegrees;
    auto closeCallback = events.AddEventCallbackGuard<NkWindowCloseEvent>(
        [&](NkWindowCloseEvent*) { running = false; }, window.GetId());
    auto keyCallback = events.AddEventCallbackGuard<NkKeyPressEvent>(
        [&](NkKeyPressEvent* event) {
            if (event->GetKey() == NkKey::NK_ESCAPE) {
                running = false;
            } else if (event->GetKey() == NkKey::NK_Q) {
                order = order == RotationOrder::YawThenPitch
                    ? RotationOrder::PitchThenYaw
                    : RotationOrder::YawThenPitch;
                const NkVec3f direction =
                    MakeOrientation(yawDegrees, kPitchDegrees, order) *
                    NkVec3f{0.0f, 0.0f, -1.0f};
                PrintDirection(yawDegrees, kPitchDegrees, order, direction);
            } else if (event->GetKey() == NkKey::NK_P) {
                const NkVec3f direction =
                    MakeOrientation(yawDegrees, kPitchDegrees, order) *
                    NkVec3f{0.0f, 0.0f, -1.0f};
                PrintDirection(yawDegrees, kPitchDegrees, order, direction);
            }
        }, window.GetId());
    auto resizeCallback = events.AddEventCallbackGuard<NkWindowResizeEvent>(
        [&](NkWindowResizeEvent* event) {
            const uint32 width = static_cast<uint32>(event->GetWidth());
            const uint32 height = static_cast<uint32>(event->GetHeight());
            if (width > 0 && height > 0) {
                renderer->OnResize(width, height);
            }
        }, window.GetId());

    int64 accumulatedX = 0;
    auto rawMouseCallback = events.AddEventCallbackGuard<NkMouseRawEvent>(
        [&](NkMouseRawEvent* event) {
            if (!fixedYaw) {
                accumulatedX += event->GetDeltaX();
            }
        }, window.GetId());

    std::printf("Souris: tourner la tete | Q: changer l'ordre | "
                "P: noter le regard | Echap: quitter\n");
    std::printf("Ordre initial: %s\n", OrderName(order));
    std::fflush(stdout);
    window.ShowMouse(fixedYaw);
    if (!fixedYaw) {
        window.CaptureMouse(true);
        window.ClipMouseToClient(true);
    }

    NkOffscreenTarget* captureTarget = nullptr;
    if (capture.frame > 0) {
        NkOffscreenDesc captureDesc;
        captureDesc.width = deviceInfo.width;
        captureDesc.height = deviceInfo.height;
        captureDesc.colorFmt = NkGPUFormat::NK_RGBA8_SRGB;
        captureDesc.readback = true;
        captureTarget = renderer->CreateOffscreen(captureDesc);
        if (!captureTarget) {
            logger.Error("[Exercice 6] Creation de la cible de capture impossible");
            window.ClipMouseToClient(false);
            window.CaptureMouse(false);
            window.ShowMouse(true);
            NkRenderer::Destroy(renderer);
            NkDeviceFactory::Destroy(device);
            window.Close();
            return 4;
        }
        renderer->SetFinalColorTargetMirror(
            renderer->GetTextures()->GetRHIHandle(captureTarget->GetColorResult()), true);
    }

    uint32 width = deviceInfo.width;
    uint32 height = deviceInfo.height;
    uint32 renderedFrames = 0;
    bool captureSucceeded = false;
    while (running && window.IsOpen()) {
        events.PollEvents();
        if (!running) {
            break;
        }

        if (!fixedYaw) {
            yawDegrees += static_cast<float32>(accumulatedX) * kSensitivity;
        }
        accumulatedX = 0;

        if (!renderer->BeginFrame()) {
            continue;
        }

        const NkQuatf orientation =
            MakeOrientation(yawDegrees, kPitchDegrees, order);
        const NkVec3f forward = orientation * NkVec3f{0.0f, 0.0f, -1.0f};
        NkCamera3DData cameraData;
        cameraData.position = {0.0f, 1.70f, 1.20f};
        cameraData.target = cameraData.position + forward;
        cameraData.up = orientation * NkVec3f{0.0f, 1.0f, 0.0f};
        cameraData.fovY = 70.0f;
        cameraData.aspect = static_cast<float32>(width) / static_cast<float32>(height);
        cameraData.nearPlane = 0.05f;
        cameraData.farPlane = 100.0f;

        NkSceneContext sceneContext;
        sceneContext.camera = NkCamera3D(cameraData);
        NkLightDesc light;
        light.type = NkLightType::NK_DIRECTIONAL;
        light.direction = {-0.4f, -1.0f, -0.3f};
        light.color = {1.0f, 0.95f, 0.85f};
        light.intensity = 3.0f;
        light.castShadow = false;
        sceneContext.lights.PushBack(light);

        scene->BeginScene(sceneContext);
        SubmitRoom(*scene, cube, plane, cone);
        renderer->Present();
        renderer->EndFrame();
        ++renderedFrames;

        if (capture.frame > 0 && renderedFrames == capture.frame) {
            captureSucceeded = captureTarget->Capture(capture.path);
            if (!captureSucceeded) {
                logger.Error("[Exercice 6] Echec de capture vers {0}", capture.path);
            } else {
                logger.Info("[Exercice 6] Capture de l'image {0} : {1}",
                            renderedFrames, capture.path);
            }
            running = false;
        }

        const NkVec2u clientSize = window.GetSize();
        if (clientSize.x > 0 && clientSize.y > 0 &&
            (clientSize.x != width || clientSize.y != height)) {
            width = clientSize.x;
            height = clientSize.y;
            renderer->OnResize(width, height);
        }
    }

    window.ClipMouseToClient(false);
    window.CaptureMouse(false);
    window.ShowMouse(true);
    device->WaitIdle();
    if (captureTarget) {
        renderer->SetFinalColorTargetMirror(NkTextureHandle{}, false);
        renderer->DestroyOffscreen(captureTarget);
    }
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    if (capture.frame > 0 && !captureSucceeded) {
        return 5;
    }
    return 0;
}

}

int nkmain(const NkEntryState&) {
    std::puts("Exercice 6: comparer les ordres lacet * tangage et tangage * lacet.");
    PrintHeights();

    float32 initialYawDegrees = 0.0f;
    bool fixedYaw = false;
    RotationOrder initialOrder = RotationOrder::YawThenPitch;
    if (const char* fixedYawText = std::getenv("MASALLE_LACET")) {
        if (!ReadFiniteFloat(fixedYawText, initialYawDegrees)) {
            logger.Error("[Exercice 6] MASALLE_LACET doit etre un nombre fini en degres");
            return 1;
        }
        fixedYaw = true;
    }

    if (const char* orderText = std::getenv("MASALLE_ORDRE")) {
        if (std::strcmp(orderText, "bon") == 0) {
            initialOrder = RotationOrder::YawThenPitch;
        } else if (std::strcmp(orderText, "mauvais") == 0) {
            initialOrder = RotationOrder::PitchThenYaw;
        } else {
            logger.Error("[Exercice 6] MASALLE_ORDRE doit valoir bon ou mauvais");
            return 1;
        }
    }

    CaptureConfig capture;
    if (!ReadCaptureConfig(capture)) {
        return 1;
    }
    if (capture.frame > 0 && !fixedYaw) {
        logger.Error("[Exercice 6] MASALLE_LACET est requis pour une capture deterministe");
        return 1;
    }

    return RunRoom(initialYawDegrees, fixedYaw, initialOrder, capture);
}
