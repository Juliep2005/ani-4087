#include "NKImage/Core/NkImage.h"
#include "NKTime/NkChrono.h"
#include <cstdio>
#include <utility>

using namespace nkentseu;

// Nombre de pixels qui different sur au moins un canal.
static long long Differences(const NkImage& a, const NkImage& b) {
    long long n = 0;
    for (int y = 0; y < 256; ++y) {
        for (int x = 0; x < 256; ++x) {
            const auto p = a.GetPixel(x, y);
            const auto q = b.GetPixel(x, y);
            if (p.r != q.r || p.g != q.g || p.b != q.b || p.a != q.a) { ++n; }
        }
    }
    return n;
}

int main() {
    NkImage photo;
    if (!photo.Load("photo.jpg") || !photo.IsValid()) {
        std::printf("Impossible de charger photo.jpg\n");
        return 1;
    }
    std::printf("photo : %d x %d\n", (int)photo.Width(), (int)photo.Height());

    // Source commune aux quatre filtres : 2048 x 2048.
    NkImage source = photo.Resize(2048, 2048, NkResizeFilter::NK_BILINEAR);
    std::printf("source : %d x %d\n", (int)source.Width(), (int)source.Height());

    const char* noms[4] = {"NK_NEAREST", "NK_BILINEAR", "NK_BICUBIC", "NK_LANCZOS3"};
    const char* fichiers[4] = {"nearest.png", "bilinear.png", "bicubic.png", "lanczos3.png"};
    const NkResizeFilter filtres[4] = {
        NkResizeFilter::NK_NEAREST, NkResizeFilter::NK_BILINEAR,
        NkResizeFilter::NK_BICUBIC, NkResizeFilter::NK_LANCZOS3
    };

    NkImage resultats[4];
    long long moyennes[4] = {0, 0, 0, 0};

    for (int i = 0; i < 4; ++i) {
        // Appel a froid, non chronometre : la premiere reduction paie une initialisation.
        NkImage chauffe = source.Resize(256, 256, filtres[i]);
        (void)chauffe;

        NkChrono chrono;
        chrono.Reset();
        NkImage derniere;
        for (int k = 0; k < 20; ++k) {
            derniere = source.Resize(256, 256, filtres[i]);
        }
        const double totalUs = (double)chrono.Elapsed().ToMicroseconds();
        moyennes[i] = (long long)(totalUs / 20.0);

        resultats[i] = std::move(derniere);
        resultats[i].SavePNG(fichiers[i]);
        std::printf("%s : %d x %d\n", noms[i], (int)resultats[i].Width(), (int)resultats[i].Height());
    }

    // Comparaison pixel par pixel au bilineaire (indice 1).
    FILE* sortie = std::fopen("mesures.txt", "w");
    if (!sortie) { return 1; }
    for (int i = 0; i < 4; ++i) {
        std::fprintf(sortie, "%s : %lld us, differences : %lld\n",
                     noms[i], moyennes[i], Differences(resultats[i], resultats[1]));
    }
    std::fclose(sortie);
    return 0;
}
