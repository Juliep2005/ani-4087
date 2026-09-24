# Exercice11

## 1. Ce que construit le projet

Le projet construit une application graphique fenêtrée nommée **`NKXRDemo`** en C++17.

La démonstration met en œuvre une scène `NKRenderer` en **stéréo simulée** grâce au module `NKXR`. Elle utilise un simulateur desktop permettant de contrôler la tête avec la souris et les déplacements avec ZQSD/WASD. Le rendu stéréo est effectué côte à côte.

Le projet compile les fichiers :

```text
src/**.cpp
```

Les fichiers générés sont placés dans `Build/Obj` pour les objets et `Build/Bin` pour l'exécutable.

## 2. Ce dont le projet dépend

Le projet dépend de 23 modules du moteur :

```text
NKXR
NKRenderer
NKRHI
NKSL
NKGLSlang
NKSPIRVCross
NKSerialization
NKReflection
NKFileSystem
NKFont
NKImage
NKGlad
NKEvent
NKWindow
NKMath
NKTime
NKLogger
NKStream
NKContainers
NKMemory
NKCore
NKPlatform
NKThreading
```

Il ajoute également les en-têtes Vulkan, car `NkVulkanDevice.h`, utilisé pour la liaison OpenXR, dépend de `vulkan.h`.

## 3. Ce qui change selon le système

### Windows

Le projet utilise le toolchain `TC_WINDOWS`, active plusieurs définitions Windows et lie notamment :

```text
user32, gdi32, opengl32, dwmapi, shell32, advapi32,
d3d11, d3d12, dxgi, dxguid, d3dcompiler, uuid, ole32
```

### Linux

La version Linux utilise `clang-native`, force le système de fenêtrage XLib et lie :

```text
pthread, X11, Xext, GL
```

Le backend Linux peut être sélectionné avec l'option `linux-backend=xlib`, ou utilisé par défaut lorsqu'il ne s'agit ni d'un autre backend ni d'un mode headless.

### macOS

La version macOS utilise `clang-native` et les frameworks :

```text
Cocoa, QuartzCore, OpenGL
```

### Debug / Release

En Debug, les optimisations sont désactivées et les symboles de débogage sont activés.

En Release, l'optimisation est réglée sur `Speed`, les symboles sont désactivés et `NDEBUG` est défini.

## 4. Les trois pièges documentés

### Piège 1 — Dépendances explicites de `NKGLSlang` et `NKSPIRVCross`

Le fichier précise que le linker d'un exécutable utilisant `NKSL` et `NKRHI` ne récupère pas automatiquement certaines dépendances transitives.

La ligne :

```python
nkentseudependson([...])
```

déclare donc explicitement les 23 dépendances.

**Sans cette ligne :** certaines bibliothèques nécessaires au linkage ne seraient pas récupérées automatiquement et l'édition de liens pourrait échouer avec des symboles non résolus.

### Piège 2 — `NK_RHI_VK_ENABLED`

Le fichier indique que `NK_RHI_VK_ENABLED` est une définition locale de `NKRHI` et qu'elle n'est pas propagée.

La ligne :

```python
defines(["NK_RHI_VK_ENABLED"])
```

est donc nécessaire.

**Sans cette ligne :** `NkVulkanDevice.h` utiliserait sa classe *stub* au lieu de l'implémentation Vulkan complète, ce qui empêcherait la compilation de la liaison OpenXR.

### Piège 3 — Bibliothèque `advapi32` sous Windows

Le commentaire Windows précise que `advapi32` fournit notamment `RegGetValueA`, utilisé lors de la découverte du runtime OpenXR actif.

La bibliothèque apparaît dans :

```python
links(["user32", "gdi32", "opengl32", "dwmapi", "shell32", "advapi32",
       "d3d11", "d3d12", "dxgi", "dxguid", "d3dcompiler", "uuid", "ole32"])
```

**Sans `advapi32` :** la fonction `RegGetValueA` nécessaire à la découverte du runtime OpenXR ne serait pas disponible lors de l'édition de liens, ce qui pourrait provoquer une erreur de symbole non résolu.

## Conclusion

`DemoXR.jenga` configure une application XR multiplateforme en adaptant ses bibliothèques, toolchains et frameworks à Windows, Linux et macOS. Les commentaires du fichier mettent particulièrement en évidence trois points nécessaires au bon fonctionnement : les dépendances explicites du moteur, l'activation de Vulkan avec `NK_RHI_VK_ENABLED` et la présence de `advapi32` sous Windows.
