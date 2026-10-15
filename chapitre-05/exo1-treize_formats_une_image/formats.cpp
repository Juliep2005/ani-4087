#include "NKImage/Core/NkImage.h"
#include <cstdio>

using namespace nkentseu;

// Taille du fichier sur le disque, en octets (0 s'il n'existe pas).
static long TailleFichier(const char* chemin) {
    FILE* f = std::fopen(chemin, "rb");
    if (!f) { return 0; }
    std::fseek(f, 0, SEEK_END);
    long n = std::ftell(f);
    std::fclose(f);
    return n;
}

int main() {
    NkImage photo;
    if (!photo.Load("photo.jpg") || !photo.IsValid()) {
        std::printf("Impossible de charger photo.jpg (dossier de lancement ?)\n");
        return 1;
    }
    const int largeur0 = photo.Width();
    const int hauteur0 = photo.Height();
    std::printf("photo : %d x %d\n", largeur0, hauteur0);

    // 1. Le moteur écrit la photo dans cinq formats.
    photo.SavePNG("sortie.png");
    photo.SaveJPEG("sortie.jpg");
    photo.SaveBMP("sortie.bmp");
    photo.SaveTGA("sortie.tga");
    photo.SaveQOI("sortie.qoi");

    struct Format { const char* nom; const char* fichier; };
    const Format formats[5] = {
        {"png", "sortie.png"}, {"jpeg", "sortie.jpg"}, {"bmp", "sortie.bmp"},
        {"tga", "sortie.tga"}, {"qoi", "sortie.qoi"}
    };

    FILE* sortie = std::fopen("formats.txt", "w");
    if (!sortie) { return 1; }

    // 2. On relit chaque fichier : la ligne vient de la relecture, pas du nom.
    for (int i = 0; i < 5; ++i) {
        const long octets = TailleFichier(formats[i].fichier);
        NkImage relue;
        if (octets > 0 && relue.Load(formats[i].fichier) && relue.IsValid()) {
            std::fprintf(sortie, "%s : %ld octets, %d x %d\n",
                         formats[i].nom, octets, relue.Width(), relue.Height());
            const bool memes = relue.Width() == largeur0 && relue.Height() == hauteur0;
            std::printf("%s : dimensions %s la photo\n",
                        formats[i].nom, memes ? "identiques a" : "DIFFERENTES de");
        } else {
            std::fprintf(sortie, "%s : %ld octets, illisible\n", formats[i].nom, octets);
        }
    }
    std::fclose(sortie);
    return 0;
}