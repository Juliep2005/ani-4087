#include "NKImage/Core/NkImage.h"
#include "NKFileSystem/NkPath.h"
#include <cstdio>

using namespace nkentseu;

int main() {
    const NkString chemin = NkPath::LocateResource("murs/mur.jpg");
    if (chemin.Empty()) {                       // rend une chaine vide si introuvable
        std::printf("LocateResource : introuvable\n");
        return 1;
    }
    std::printf("chemin trouve : %s\n", chemin.CStr());
    NkImage img;
    const bool ok = img.Load(chemin.CStr());
    std::printf("charge : %d, largeur : %d, hauteur : %d\n", (int)ok, (int)img.Width(), (int)img.Height());
    return ok ? 0 : 1;
}
