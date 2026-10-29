#pragma once

#include <cstdio>
#include <memory>

#include <NKEvent/NkEventSystem.h>
#include <NKEvent/NkMouseEvent.h>
#include <NKEvent/NkWindowEvent.h>
#include <NKMath/NKMath.h>
#include <NKRenderer/Core/NkCamera.h>
#include <NKRenderer/Core/NkRenderGraph.h>
#include <NKRenderer/Core/NkTextureLibrary.h>
#include <NKRenderer/Materials/NkMaterialCollection.h>
#include <NKRenderer/Mesh/NkMeshSystem.h>
#include <NKRenderer/NkRenderer.h>
#include <NKRenderer/Tools/Offscreen/NkOffscreenTarget.h>
#include <NKRenderer/Tools/Render2D/NkRender2D.h>
#include <NKRenderer/Tools/Render3D/NkRender3D.h>
#include <NKRHI/Core/NkDeviceFactory.h>
#include <NKWindow/Core/NkWindow.h>
#include <NKWindow/Core/NkWindowConfig.h>

namespace chapitre7 {

using namespace nkentseu;
using namespace nkentseu::renderer;

inline void SubmitBox(NkRender3D* scene, NkMeshSystem* meshes,
                      const NkVec3f& center, const NkVec3f& size,
                      const NkVec3f& tint, float32 roughness = 1.0f) {
    NkDrawCall3D draw;
    draw.mesh = meshes->GetCube();
    draw.transform = NkMat4f::Translate(center) * NkMat4f::Scale(size);
    draw.aabb = {
        {center.x - size.x * 0.5f, center.y - size.y * 0.5f,
         center.z - size.z * 0.5f},
        {center.x + size.x * 0.5f, center.y + size.y * 0.5f,
         center.z + size.z * 0.5f}
    };
    draw.tint = tint;
    draw.metallic = 0.0f;
    draw.roughness = roughness;
    scene->Submit(draw);
}

inline void SubmitRoom(NkRender3D* scene, NkMeshSystem* meshes) {
    constexpr float32 roomSize = 4.0f;
    constexpr float32 halfRoom = roomSize * 0.5f;
    constexpr float32 wallHeight = 2.5f;
    constexpr float32 wallThickness = 0.1f;
    const NkVec3f wallColor{0.72f, 0.70f, 0.66f};
    const NkVec3f floorColor{0.38f, 0.36f, 0.34f};
    const NkVec3f woodColor{0.55f, 0.38f, 0.22f};
    const NkVec3f glassColor{0.75f, 0.85f, 0.92f};

    NkDrawCall3D floor;
    floor.mesh = meshes->GetPlane();
    floor.transform = NkMat4f::Scale({roomSize, 1.0f, roomSize});
    floor.aabb = {{-halfRoom, 0.0f, -halfRoom}, {halfRoom, 0.0f, halfRoom}};
    floor.castShadow = false;
    floor.tint = floorColor;
    floor.metallic = 0.0f;
    floor.roughness = 1.0f;
    scene->Submit(floor);

    const float32 wallY = wallHeight * 0.5f;
    const float32 wallEdge = halfRoom + wallThickness * 0.5f;
    SubmitBox(scene, meshes, {0.0f, wallY, -wallEdge},
              {roomSize, wallHeight, wallThickness}, wallColor);
    SubmitBox(scene, meshes, {0.0f, wallY, wallEdge},
              {roomSize, wallHeight, wallThickness}, wallColor);
    SubmitBox(scene, meshes, {-wallEdge, wallY, 0.0f},
              {wallThickness, wallHeight, roomSize}, wallColor);
    SubmitBox(scene, meshes, {wallEdge, wallY, 0.0f},
              {wallThickness, wallHeight, roomSize}, wallColor);
    SubmitBox(scene, meshes, {0.0f, wallHeight + wallThickness * 0.5f, 0.0f},
              {roomSize, wallThickness, roomSize}, wallColor);
    SubmitBox(scene, meshes, {-0.70f, 1.0f, -halfRoom + 0.02f},
              {0.90f, 2.0f, 0.04f}, woodColor, 0.6f);
    SubmitBox(scene, meshes, {-halfRoom + 0.02f, 1.40f, -0.90f},
              {0.04f, 1.0f, 1.20f}, glassColor, 0.15f);
    SubmitBox(scene, meshes, {-halfRoom + 0.02f, 1.40f, 0.90f},
              {0.04f, 1.0f, 1.20f}, glassColor, 0.15f);

    const float32 tableX = 0.60f;
    const float32 tableZ = 0.70f;
    const float32 tableLength = 1.20f;
    const float32 tableDepth = 0.80f;
    const float32 tabletopThickness = 0.05f;
    const float32 legSize = 0.06f;
    const float32 legHeight = 0.75f - tabletopThickness;
    SubmitBox(scene, meshes,
              {tableX, 0.75f - tabletopThickness * 0.5f, tableZ},
              {tableLength, tabletopThickness, tableDepth}, woodColor, 0.5f);
    for (int i = 0; i < 4; ++i) {
        const float32 dx = ((i & 1) ? 1.0f : -1.0f) *
                           (tableLength * 0.5f - legSize);
        const float32 dz = ((i & 2) ? 1.0f : -1.0f) *
                           (tableDepth * 0.5f - legSize);
        SubmitBox(scene, meshes,
                  {tableX + dx, legHeight * 0.5f, tableZ + dz},
                  {legSize, legHeight, legSize}, woodColor, 0.6f);
    }
    SubmitBox(scene, meshes, {-1.10f, 0.50f, 1.20f},
              {1.0f, 1.0f, 1.0f}, {0.85f, 0.80f, 0.30f}, 0.8f);
}

class Room {
public:
    Room() = default;
    Room(const Room&) = delete;
    Room& operator=(const Room&) = delete;

    ~Room() {
        Shutdown();
    }

    bool Initialize(const char* title) {
        NkWindowConfig windowConfig;
        windowConfig.title = title;
        windowConfig.width = 1280;
        windowConfig.height = 720;
        windowConfig.centered = true;
        windowConfig.resizable = true;
        windowConfig.vsync = true;
        mWindow = std::make_unique<NkWindow>(windowConfig);
        if (!mWindow->IsValid()) {
            std::fprintf(stderr, "[MaSalle] creation de la fenetre impossible\n");
            return false;
        }

        NkDeviceInitInfo deviceInfo{};
        deviceInfo.surface = mWindow->GetSurfaceDesc();
        deviceInfo.width = static_cast<uint32>(mWindow->GetSize().x);
        deviceInfo.height = static_cast<uint32>(mWindow->GetSize().y);
        mDevice = NkDeviceFactory::CreateAutoDetect(deviceInfo);
        if (mDevice == nullptr || !mDevice->IsValid()) {
            std::fprintf(stderr, "[MaSalle] creation du device graphique impossible\n");
            return false;
        }
        std::printf("[MaSalle] API : %s\n", NkGraphicsApiName(mDevice->GetApi()));

        NkRendererConfig screenConfig =
            NkRendererConfig::For2D(deviceInfo.api, deviceInfo.width, deviceInfo.height);
        mScreenRenderer = NkRenderer::Create(mDevice, screenConfig);
        if (mScreenRenderer == nullptr || !mScreenRenderer->Initialize()) {
            std::fprintf(stderr, "[MaSalle] initialisation du renderer d'ecran impossible\n");
            return false;
        }

        NkOffscreenDesc offscreenDesc;
        offscreenDesc.width = deviceInfo.width;
        offscreenDesc.height = deviceInfo.height;
        offscreenDesc.hasDepth = true;
        offscreenDesc.hdr = false;
        offscreenDesc.colorFmt = NkGPUFormat::NK_RGBA8_UNORM;
        offscreenDesc.readable = true;
        offscreenDesc.readback = true;
        offscreenDesc.name = "Chapitre7_Salle";
        mOffscreen = mScreenRenderer->CreateOffscreen(offscreenDesc);
        if (mOffscreen == nullptr) {
            std::fprintf(stderr, "[MaSalle] creation de la cible hors ecran impossible\n");
            return false;
        }

        NkRendererConfig sceneConfig =
            NkRendererConfig::ForGame(deviceInfo.api, deviceInfo.width, deviceInfo.height);
        sceneConfig.Disable(NK_SS_RENDER2D);
        sceneConfig.Disable(NK_SS_TEXT);
        sceneConfig.Disable(NK_SS_OVERLAY);
        sceneConfig.Enable(NK_SS_OFFSCREEN);
        mSceneRenderer = NkRenderer::Create(mDevice, sceneConfig);
        if (mSceneRenderer == nullptr || !mSceneRenderer->Initialize()) {
            std::fprintf(stderr, "[MaSalle] initialisation du renderer de la salle impossible\n");
            return false;
        }
        mSceneRenderer->SetFinalColorTarget(
            mScreenRenderer->GetTextures()->GetRHIHandle(mOffscreen->GetColorResult()));
        mWidth = deviceInfo.width;
        mHeight = deviceInfo.height;
        return true;
    }

    NkVec2f PollEvents() {
        NkVec2f rawDelta{0.0f, 0.0f};
        NkEvent* event = nullptr;
        while (NkEvents().PollEvent(event)) {
            if (event != nullptr && event->As<NkWindowCloseEvent>() != nullptr) {
                mRunning = false;
            }
            if (event != nullptr) {
                if (auto* raw = event->As<NkMouseRawEvent>()) {
                    rawDelta.x += static_cast<float32>(raw->GetDeltaX());
                    rawDelta.y += static_cast<float32>(raw->GetDeltaY());
                }
            }
        }
        return rawDelta;
    }

    bool IsRunning() const {
        return mRunning && mWindow != nullptr && mWindow->IsOpen();
    }

    bool RenderFrame(const NkQuatf& headOrientation = NkQuatf(),
                     uint32 roomCopies = 1u) {
        if (!IsRunning() || mScreenRenderer == nullptr || mSceneRenderer == nullptr) {
            return false;
        }
        if (!mScreenRenderer->BeginFrame()) {
            return false;
        }

        NkICommandBuffer* commandBuffer = mScreenRenderer->GetCmd();
        NkRender3D* scene = mSceneRenderer->GetRender3D();
        if (commandBuffer == nullptr || scene == nullptr) {
            std::fprintf(stderr, "[MaSalle] ressources de rendu indisponibles\n");
            mScreenRenderer->EndFrame();
            return false;
        }

        mSceneRenderer->FlushGraphRebuilds();
        scene->ResetFrame();
        if (auto* materials = mSceneRenderer->GetMaterialCollection()) {
            materials->Upload();
        }

        NkCamera3DData camera;
        camera.position = {0.30f, 1.70f, 1.20f};
        camera.target = camera.position +
                        headOrientation * NkVec3f(0.0f, 0.0f, -1.0f);
        camera.up = headOrientation * NkVec3f(0.0f, 1.0f, 0.0f);
        camera.fovY = 70.0f;
        camera.aspect = static_cast<float32>(mWidth) /
                        static_cast<float32>(mHeight);
        camera.nearPlane = 0.05f;
        camera.farPlane = 100.0f;

        NkSceneContext sceneContext;
        sceneContext.camera = NkCamera3D(camera);
        sceneContext.time = static_cast<float32>(mFrameCount) / 60.0f;
        scene->SetIBLStrength(0.35f);

        NkLightDesc light;
        light.type = NkLightType::NK_DIRECTIONAL;
        light.direction = {-0.5f, -1.0f, -0.35f};
        light.color = {1.0f, 0.96f, 0.88f};
        light.intensity = 3.0f;
        light.castShadow = false;
        sceneContext.lights.PushBack(light);
        scene->BeginScene(sceneContext);

        const uint32 copies = roomCopies == 0u ? 1u : roomCopies;
        for (uint32 i = 0; i < copies; ++i) {
            SubmitRoom(scene, mSceneRenderer->GetMeshSystem());
        }

        NkRenderGraph* graph = mSceneRenderer->GetRenderGraph();
        if (graph == nullptr) {
            std::fprintf(stderr, "[MaSalle] graphe de rendu indisponible\n");
            mScreenRenderer->EndFrame();
            return false;
        }
        graph->Execute(commandBuffer);
        if (auto* renderer2D = mScreenRenderer->GetRender2D()) {
            renderer2D->DrawImage(
                mOffscreen->GetColorResult(),
                {0.0f, 0.0f, static_cast<float32>(mWidth),
                 static_cast<float32>(mHeight)});
        }

        mScreenRenderer->Present();
        mScreenRenderer->EndFrame();
        ++mFrameCount;
        return true;
    }

    uint64 FrameCount() const {
        return mFrameCount;
    }

    bool UsesVulkan() const {
        if (mDevice == nullptr) {
            return false;
        }
        const char* api = NkGraphicsApiName(mDevice->GetApi());
        return api != nullptr &&
               (api[0] == 'V' || api[0] == 'v') &&
               (api[1] == 'u' || api[1] == 'U');
    }

private:
    void Shutdown() {
        if (mDevice != nullptr) {
            mDevice->WaitIdle();
        }
        if (mSceneRenderer != nullptr) {
            NkRenderer::Destroy(mSceneRenderer);
            mSceneRenderer = nullptr;
        }
        if (mScreenRenderer != nullptr) {
            NkRenderer::Destroy(mScreenRenderer);
            mScreenRenderer = nullptr;
        }
        mOffscreen = nullptr;
        if (mDevice != nullptr) {
            NkDeviceFactory::Destroy(mDevice);
            mDevice = nullptr;
        }
        if (mWindow != nullptr) {
            mWindow->Close();
            mWindow.reset();
        }
    }

    std::unique_ptr<NkWindow> mWindow;
    NkIDevice* mDevice = nullptr;
    NkRenderer* mScreenRenderer = nullptr;
    NkRenderer* mSceneRenderer = nullptr;
    NkOffscreenTarget* mOffscreen = nullptr;
    uint32 mWidth = 1280u;
    uint32 mHeight = 720u;
    uint64 mFrameCount = 0u;
    bool mRunning = true;
};

} // namespace chapitre7
