#include <cstdio>
#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKTime/NkTime.h"
#include "NKLogger/NkLog.h"
#include "NKImage/Core/NkImage.h"

#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Materials/NkMaterial.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "Salle";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    NkImage img;
    bool c = img.Load("murs/nexistepas.png");
    std::printf("charge : %d, valide : %d, largeur : %d, hauteur : %d\n",
                (int)c, (int)img.IsValid(), (int)img.Width(), (int)img.Height());

    // Partie 2 : la salle.
    NkWindowConfig winCfg;
    winCfg.title = "Salle";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = true;
    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[Salle] Creation fenetre KO");
        return 1;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;
    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Salle] Creation device KO");
        window.Close();
        return 2;
    }
    logger.Info("[Salle] Backend : {0}", NkGraphicsApiName(device->GetApi()));

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Salle] Init renderer KO");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }
    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();
    NkMeshHandle meshPlane = renderer->GetMeshSystem()->GetPlane();

    // La texture inexistante, puis le materiau du mur.
    auto *texLib = renderer->GetTextures();
    NkTexHandle tex = texLib->Load(NkString("murs/nexistepas.png"));
    const bool estTextureErreur = tex.id == texLib->GetError().id; std::printf("texture valide : %d, texture d'erreur : %d\n", tex.IsValid() ? 1 : 0, estTextureErreur ? 1 : 0);
    auto wallMat = NkMaterial::Create(renderer->GetMaterials(), NkMaterialType::NK_PBR_METALLIC);
    if (wallMat && wallMat->IsValid()) {
        wallMat->SetAlbedoMap(tex)->SetMetallic(0.0f)->SetRoughness(0.85f);
    }

    bool running = true;
    uint32 W = devInfo.width;
    uint32 H = devInfo.height;
    NkEventSystem &events = NkEvents();
    events.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { running = false; });
    events.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE) { running = false; }
    });
    events.AddEventCallback<NkWindowResizeEvent>([&](NkWindowResizeEvent *e) {
        const uint32 w = (uint32)e->GetWidth(), h = (uint32)e->GetHeight();
        if (w > 0 && h > 0 && (w != W || h != H)) {
            W = w;
            H = h;
            renderer->OnResize(w, h);
        }
    });

    NkClock clock;
    float32 total = 0.f;
    while (running && window.IsOpen()) {
        events.PollEvents();
        if (!running) { break; }
        total += clock.Tick().delta;
        if (!renderer->BeginFrame()) { continue; }

        NkCamera3DData camData;
        camData.up = {0.f, 1.f, 0.f};
        camData.fovY = 60.f;
        camData.aspect = (float32)W / (float32)H;
        camData.nearPlane = 0.05f;      // 5 cm
        camData.farPlane = 100.f;
        NkCamera3D cam(camData);
        cam.SetPosition({0.f, 1.7f, 1.0f});   // hauteur des yeux
        cam.SetTarget({0.f, 1.4f, -2.f});

        NkSceneContext sctx;
        sctx.camera = cam;
        sctx.time = total;
        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.4f, -1.f, -0.3f};
        sun.color = {1.f, 0.95f, 0.85f};
        sun.intensity = 3.f;
        sun.castShadow = false;
        sctx.lights.PushBack(sun);
        r3d->BeginScene(sctx);

        {   // sol : plan de 4 m x 4 m (le plan unitaire fait 2 m de cote)
            NkDrawCall3D dc;
            dc.mesh = meshPlane;
            dc.transform = NkMat4f::Scale({2.f, 1.f, 2.f});
            dc.aabb = {{-2.f, 0.f, -2.f}, {2.f, 0.f, 2.f}};
            dc.castShadow = false;
            dc.tint = {0.14f, 0.14f, 0.16f};
            dc.metallic = 0.f;
            dc.roughness = 0.9f;
            r3d->Submit(dc);
        }
        {   // mur du fond : translation APRES l'echelle, dans le bon ordre
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate({0.f, 1.25f, -2.f}) * NkMat4f::Scale({4.f, 2.5f, 0.1f});
            dc.aabb = {{-2.f, 0.f, -2.05f}, {2.f, 2.5f, -1.95f}};
            dc.tint = {0.72f, 0.70f, 0.66f};
            if (wallMat) { dc.material = wallMat->GetInstHandle(); }
            r3d->Submit(dc);
        }

        renderer->Present();
        renderer->EndFrame();
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}


