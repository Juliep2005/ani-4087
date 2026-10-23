#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <NKEvent/NkEventSystem.h>
#include <NKEvent/NkKeyboardEvent.h>
#include <NKEvent/NkWindowEvent.h>
#include <NKLogger/NkLog.h>
#include <NKMath/NKMath.h>
#include <NKPlatform/NkPlatformDetect.h>
#include <NKRenderer/Core/NkCamera.h>
#include <NKRenderer/Materials/NkMaterial.h>
#include <NKRenderer/Mesh/NkMeshSystem.h>
#include <NKRenderer/NkRenderer.h>
#include <NKRenderer/Tools/Render3D/NkRender3D.h>
#include <NKTime/NkChrono.h>
#include <NKRHI/Core/NkDeviceFactory.h>
#include <NKWindow/Core/NkWindow.h>
#include <NKWindow/Core/NkWindowConfig.h>
#include <NKWindow/NKMain.h>

using namespace nkentseu;
using namespace nkentseu::renderer;

namespace {

constexpr uint32 kIterations = 1'000'000;

NkQuatf MatrixOrientation(float32 yawDegrees, float32 pitchDegrees) {
    const NkMat4f rotation =
        NkMat4f::RotationY(NkAngle(yawDegrees)) *
        NkMat4f::RotationX(NkAngle(pitchDegrees));
    return NkQuatf(rotation).Normalized();
}

NkQuatf QuaternionOrientation(float32 yawDegrees, float32 pitchDegrees) {
    const NkQuatf yaw = NkQuatf::RotateY(NkAngle(yawDegrees));
    const NkQuatf pitch = NkQuatf::RotateX(NkAngle(pitchDegrees));
    return (yaw * pitch).Normalized();
}

bool VerifyOrientations() {
    const NkQuatf matrix = MatrixOrientation(45.0f, 30.0f);
    const NkQuatf quaternion = QuaternionOrientation(45.0f, 30.0f);
    const float32 differences[] = {
        std::abs(matrix.x - quaternion.x),
        std::abs(matrix.y - quaternion.y),
        std::abs(matrix.z - quaternion.z),
        std::abs(matrix.w - quaternion.w)
    };
    const float32 maximumDifference =
        *std::max_element(std::begin(differences), std::end(differences));
    std::printf("Verification lacet=45 deg, tangage=30 deg : "
                "matrice=(%.6f, %.6f, %.6f, %.6f), "
                "quaternion=(%.6f, %.6f, %.6f, %.6f), "
                "ecart max=%.8f\n",
                static_cast<double>(matrix.x),
                static_cast<double>(matrix.y),
                static_cast<double>(matrix.z),
                static_cast<double>(matrix.w),
                static_cast<double>(quaternion.x),
                static_cast<double>(quaternion.y),
                static_cast<double>(quaternion.z),
                static_cast<double>(quaternion.w),
                static_cast<double>(maximumDifference));
    return maximumDifference <= 0.0001f;
}

template <bool UseMatrix>
void RunMeasurements(const char* configuration, const char* method) {
    for (uint32 repetition = 1; repetition <= 3; ++repetition) {
        double sums[4] = {};
        NkChrono chrono;

        for (uint32 i = 0; i < kIterations; ++i) {
            const float32 yawDegrees =
                static_cast<float32>(i % 3600u) * 0.1f;
            const float32 pitchDegrees =
                static_cast<float32>((i * 7u) % 900u) * 0.1f - 45.0f;
            const NkQuatf orientation = UseMatrix
                ? MatrixOrientation(yawDegrees, pitchDegrees)
                : QuaternionOrientation(yawDegrees, pitchDegrees);
            sums[0] += orientation.x;
            sums[1] += orientation.y;
            sums[2] += orientation.z;
            sums[3] += orientation.w;
        }

        const float64 elapsedMilliseconds =
            chrono.Elapsed().ToMilliseconds();
        std::printf("%s, %s : %.6f ms, somme=(%.6f, %.6f, %.6f, %.6f)\n",
                    configuration, method,
                    static_cast<double>(elapsedMilliseconds),
                    sums[0], sums[1], sums[2], sums[3]);
    }
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
    SubmitBox(scene, cube, {0.0f, 2.45f, 0.0f}, {4.0f, 0.1f, 4.0f},
              {0.48f, 0.51f, 0.57f});
    SubmitBox(scene, cube, {0.0f, 1.25f, -2.0f}, {4.0f, 2.5f, 0.1f}, wall);
    SubmitBox(scene, cube, {-2.0f, 1.25f, -1.25f}, {0.1f, 2.5f, 1.5f}, wall);
    SubmitBox(scene, cube, {-2.0f, 1.25f, 1.25f}, {0.1f, 2.5f, 1.5f}, wall);
    SubmitBox(scene, cube, {-2.0f, 2.3f, 0.0f}, {0.1f, 0.4f, 1.0f}, wall);
    SubmitBox(scene, cube, {-1.93f, 1.05f, 0.0f}, {0.05f, 2.1f, 0.9f},
              {0.20f, 0.38f, 0.58f});
    SubmitBox(scene, cube, {-1.88f, 1.05f, 0.30f}, {0.04f, 0.08f, 0.04f},
              {0.92f, 0.70f, 0.24f});
    SubmitBox(scene, cube, {2.0f, 1.25f, 0.0f}, {0.1f, 2.5f, 4.0f}, wall);
    SubmitBox(scene, cube, {-1.55f, 1.25f, 2.0f}, {0.9f, 2.5f, 0.1f}, wall);
    SubmitBox(scene, cube, {1.55f, 1.25f, 2.0f}, {0.9f, 2.5f, 0.1f}, wall);
    SubmitBox(scene, cube, {0.0f, 2.25f, 2.0f}, {2.2f, 0.5f, 0.1f}, wall);

    NkDrawCall3D marker;
    marker.mesh = cone;
    marker.transform =
        NkMat4f::Translate({0.0f, 0.55f, -1.25f}) *
        NkMat4f::Scale({0.45f, 0.9f, 0.45f});
    marker.aabb = {{-0.5f, 0.0f, -1.75f}, {0.5f, 1.1f, -0.75f}};
    marker.tint = {0.90f, 0.28f, 0.12f};
    marker.metallic = 0.0f;
    marker.roughness = 0.8f;
    scene.Submit(marker);
}

int RunRoom() {
    NkWindowConfig windowConfig;
    windowConfig.title = "Exercice 7 - cout mesure";
    windowConfig.width = 1280;
    windowConfig.height = 720;
    windowConfig.centered = true;

    NkWindow window(windowConfig);
    if (!window.IsValid()) {
        logger.Error("[Exercice 7] Creation de la fenetre impossible");
        return 1;
    }

    NkDeviceInitInfo deviceInfo{};
    deviceInfo.surface = window.GetSurfaceDesc();
    deviceInfo.width = static_cast<uint32>(window.GetSize().width);
    deviceInfo.height = static_cast<uint32>(window.GetSize().height);
    NkIDevice* device = NkDeviceFactory::CreateAutoDetect(deviceInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Exercice 7] Creation du device graphique impossible");
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 2;
    }

    NkRendererConfig rendererConfig =
        NkRendererConfig::ForGame(deviceInfo.api, deviceInfo.width, deviceInfo.height);
    NkRenderer* renderer = NkRenderer::Create(device, rendererConfig);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Exercice 7] Initialisation du renderer impossible");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkRender3D* scene = renderer->GetRender3D();
    const NkMeshHandle cube = renderer->GetMeshSystem()->GetCube();
    const NkMeshHandle plane = renderer->GetMeshSystem()->GetPlane();
    const NkMeshHandle cone = renderer->GetMeshSystem()->GetCone();
    NkEventSystem& events = NkEvents();
    bool running = true;
    uint32 width = deviceInfo.width;
    uint32 height = deviceInfo.height;
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
            const uint32 resizedWidth = static_cast<uint32>(event->GetWidth());
            const uint32 resizedHeight = static_cast<uint32>(event->GetHeight());
            if (resizedWidth > 0 && resizedHeight > 0) {
                renderer->OnResize(resizedWidth, resizedHeight);
            }
        }, window.GetId());

    while (running && window.IsOpen()) {
        events.PollEvents();
        if (!running) {
            break;
        }
        if (!renderer->BeginFrame()) {
            continue;
        }

        const NkQuatf orientation = QuaternionOrientation(0.0f, 30.0f);
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

        const NkVec2u clientSize = window.GetSize();
        if (clientSize.x > 0 && clientSize.y > 0 &&
            (clientSize.x != width || clientSize.y != height)) {
            width = clientSize.x;
            height = clientSize.y;
            renderer->OnResize(width, height);
        }
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}

}

int nkmain(const NkEntryState&) {
    if (!VerifyOrientations()) {
        logger.Error("[Exercice 7] Les constructions ne donnent pas le meme quaternion");
        return 1;
    }

#if defined(MASALLE_RELEASE)
    constexpr const char* configuration = "Release";
#else
    constexpr const char* configuration = "Debug";
#endif
    RunMeasurements<true>(configuration, "matrices");
    RunMeasurements<false>(configuration, "quaternions");

    if (const char* benchOnly = std::getenv("MASALLE_BENCH_ONLY")) {
        if (std::strcmp(benchOnly, "1") != 0) {
            logger.Error("[Exercice 7] MASALLE_BENCH_ONLY doit valoir 1");
            return 1;
        }
        return 0;
    }
    return RunRoom();
}
