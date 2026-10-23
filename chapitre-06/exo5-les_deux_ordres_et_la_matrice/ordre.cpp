#include <algorithm>
#include <cstdio>

#include <NKEvent/NkEventSystem.h>
#include <NKEvent/NkKeyboardEvent.h>
#include <NKEvent/NkMouseEvent.h>
#include <NKEvent/NkWindowEvent.h>
#include <NKLogger/NkLog.h>
#include <NKMath/NKMath.h>
#include <NKPlatform/NkPlatformDetect.h>
#include <NKRenderer/Core/NkCamera.h>
#include <NKRenderer/Materials/NkMaterial.h>
#include <NKRenderer/Mesh/NkMeshSystem.h>
#include <NKRenderer/NkRenderer.h>
#include <NKRenderer/Tools/Render3D/NkRender3D.h>
#include <NKRHI/Core/NkDeviceFactory.h>
#include <NKWindow/Core/NkWindow.h>
#include <NKWindow/Core/NkWindowConfig.h>
#include <NKWindow/NKMain.h>

using namespace nkentseu;
using namespace nkentseu::renderer;

namespace {

constexpr float32 kPi = 3.14159265358979323846f;
constexpr float32 kSensitivity = 0.002f;
constexpr float32 kMaximumPitch = 89.0f * kPi / 180.0f;

void PrintDirection(const char* label, NkVec3f direction) {
    std::printf("%s: (%.4f, %.4f, %.4f)\n",
                label,
                static_cast<double>(direction.x),
                static_cast<double>(direction.y),
                static_cast<double>(direction.z));
}

void PrintOrientationComparison() {
    const NkAngle yaw(45.0f);
    const NkAngle pitch(30.0f);
    const NkVec3f forward = {0.0f, 0.0f, -1.0f};
    const NkQuatf yawRotation = NkQuatf::RotateY(yaw);
    const NkQuatf pitchRotation = NkQuatf::RotateX(pitch);

    const NkVec3f yawThenPitch = (yawRotation * pitchRotation) * forward;
    const NkVec3f pitchThenYaw = (pitchRotation * yawRotation) * forward;
    const NkMat4f matrixRotation =
        NkMat4f::RotationY(yaw) * NkMat4f::RotationX(pitch);
    const NkVec3f matrixDirection = matrixRotation * forward;

    std::printf("lacet = 45 deg, tangage = 30 deg, avant = (0.0, 0.0, -1.0)\n");
    PrintDirection("quaternion lacet * tangage", yawThenPitch);
    PrintDirection("quaternion tangage * lacet", pitchThenYaw);
    PrintDirection("matrice RotationY * RotationX", matrixDirection);
    std::printf("composante y (lacet * tangage) = %.4f\n",
                static_cast<double>(yawThenPitch.y));
    std::printf("composante y (tangage * lacet) = %.4f\n",
                static_cast<double>(pitchThenYaw.y));
    std::printf("composante y (matrice) = %.4f\n",
                static_cast<double>(matrixDirection.y));
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

    const NkVec3f frontWall = {0.67f, 0.65f, 0.62f};
    SubmitBox(scene, cube, {-1.55f, 1.25f, 2.0f}, {0.9f, 2.5f, 0.1f}, frontWall);
    SubmitBox(scene, cube, {1.55f, 1.25f, 2.0f}, {0.9f, 2.5f, 0.1f}, frontWall);
    SubmitBox(scene, cube, {0.0f, 2.25f, 2.0f}, {2.2f, 0.5f, 0.1f}, frontWall);
    SubmitBox(scene, cube, {0.0f, 1.0f, 1.93f}, {0.9f, 2.0f, 0.04f},
              {0.34f, 0.22f, 0.14f});
}

int RunRoom() {
    NkWindowConfig windowConfig;
    windowConfig.title = "Exercice 5 - ordre des rotations";
    windowConfig.width = 1280;
    windowConfig.height = 720;
    windowConfig.centered = true;
    windowConfig.resizable = true;

    NkWindow window(windowConfig);
    if (!window.IsValid()) {
        logger.Error("[Exercice 5] Creation de la fenetre impossible");
        return 1;
    }

    NkDeviceInitInfo deviceInfo{};
    deviceInfo.surface = window.GetSurfaceDesc();
    deviceInfo.width = static_cast<uint32>(window.GetSize().width);
    deviceInfo.height = static_cast<uint32>(window.GetSize().height);
    NkIDevice* device = NkDeviceFactory::CreateAutoDetect(deviceInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Exercice 5] Creation du device graphique impossible");
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 2;
    }

    NkRendererConfig rendererConfig =
        NkRendererConfig::ForGame(deviceInfo.api, deviceInfo.width, deviceInfo.height);
    NkRenderer* renderer = NkRenderer::Create(device, rendererConfig);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Exercice 5] Initialisation du renderer impossible");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkRender3D* scene = renderer->GetRender3D();
    const NkMeshHandle cube = renderer->GetMeshSystem()->GetCube();
    const NkMeshHandle plane = renderer->GetMeshSystem()->GetPlane();
    NkEventSystem& events = NkEvents();

    bool running = true;
    auto closeCallback = events.AddEventCallbackGuard<NkWindowCloseEvent>(
        [&](NkWindowCloseEvent*) { running = false; }, window.GetId());
    auto keyCallback = events.AddEventCallbackGuard<NkKeyPressEvent>(
        [&](NkKeyPressEvent* event) {
            if (event->GetKey() == NkKey::NK_ESCAPE) {
                running = false;
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
    int64 accumulatedY = 0;
    auto rawMouseCallback = events.AddEventCallbackGuard<NkMouseRawEvent>(
        [&](NkMouseRawEvent* event) {
            accumulatedX += event->GetDeltaX();
            accumulatedY += event->GetDeltaY();
        }, window.GetId());

    window.ShowMouse(false);
    window.CaptureMouse(true);
    window.ClipMouseToClient(true);

    float32 yaw = 0.0f;
    float32 pitch = 0.0f;
    uint32 width = deviceInfo.width;
    uint32 height = deviceInfo.height;
    while (running && window.IsOpen()) {
        events.PollEvents();
        if (!running) {
            break;
        }

        yaw += static_cast<float32>(accumulatedX) * kSensitivity;
        pitch = std::clamp(
            pitch + static_cast<float32>(accumulatedY) * kSensitivity,
            -kMaximumPitch, kMaximumPitch);
        accumulatedX = 0;
        accumulatedY = 0;

        if (!renderer->BeginFrame()) {
            continue;
        }

        NkCamera3DData cameraData;
        cameraData.position = {0.0f, 1.70f, 1.20f};
        cameraData.target = cameraData.position + NkVec3f(
            -std::sin(yaw) * std::cos(pitch),
            std::sin(pitch),
            -std::cos(yaw) * std::cos(pitch));
        cameraData.up = {0.0f, 1.0f, 0.0f};
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
        SubmitRoom(*scene, cube, plane);
        renderer->Present();
        renderer->EndFrame();

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
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}

}

int nkmain(const NkEntryState&) {
    PrintOrientationComparison();
    return RunRoom();
}
