#  Exercice7

## Objectif

Classer les 23 dépendances de la démonstration du moteur en fonction de ce que leur nom permet de comprendre.

## 1. Nom suffisant pour deviner le rôle

* `NKRenderer` : rendu graphique.
* `NKSerialization` : sérialisation des données.
* `NKReflection` : réflexion et inspection des objets/types.
* `NKFileSystem` : gestion du système de fichiers.
* `NKFont` : gestion des polices.
* `NKImage` : gestion des images.
* `NKEvent` : gestion des événements.
* `NKWindow` : gestion des fenêtres.
* `NKMath` : fonctions mathématiques.
* `NKTime` : gestion du temps.
* `NKLogger` : gestion des journaux et messages.
* `NKStream` : gestion des flux de données.
* `NKContainers` : gestion des conteneurs de données.
* `NKMemory` : gestion de la mémoire.
* `NKThreading` : gestion des threads.

## 2. Nom donnant une idée sans certitude

* `NKRHI` : probablement une interface d'abstraction du rendu matériel.
* `NKSL` : probablement lié aux langages ou aux shaders.
* `NKGLSlang` : probablement lié à GLSL et aux shaders.
* `NKSPIRVCross` : probablement lié à la conversion ou traduction de SPIR-V.
* `NKGlad` : probablement lié au chargement des fonctions OpenGL.
* `NKPlatform` : probablement lié à l'abstraction des plateformes.

## 3. Nom insuffisant pour connaître précisément le rôle

* `NKXR` : le sigle suggère un lien avec la réalité étendue, mais le rôle exact n'est pas déductible uniquement du nom.
* `NKCore` : le terme « Core » est trop général pour déterminer précisément les fonctionnalités du module.

## Résultat

Le classement montre que la majorité des dépendances ont un nom suffisamment explicite pour donner une première idée de leur fonction. Pour les modules plus génériques ou abrégés, la consultation de leur en-tête principal est nécessaire pour déterminer précisément leur rôle.
