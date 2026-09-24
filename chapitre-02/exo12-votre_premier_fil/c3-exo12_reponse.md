# Exercice12

## Objectif

Le fichier `salle.jenga` a été préparé pour les différents chapitres à venir du livre.

Pour chaque chapitre, une ligne ou un bloc de configuration a été ajouté **sous forme de commentaire**. Ces lignes ne sont donc pas encore exécutées.

## Organisation

Le fichier contient des commentaires indiquant ce qu'il faudra ajouter au projet au fur et à mesure des chapitres, notamment :

* les configurations du projet ;
* les filtres par système d'exploitation ;
* les dépendances entre projets ;
* les bibliothèques ;
* les répertoires d'inclusion ;
* les définitions du préprocesseur ;
* les optimisations Debug/Release ;
* les symboles de débogage ;
* les configurations Windows, Linux et macOS ;
* l'organisation des fichiers générés ;
* les éléments liés au moteur XR et à Vulkan ;
* les dépendances explicites du linker.

## Méthode prévue

Les lignes sont volontairement laissées commentées. Lors de l'étude de chaque chapitre, la configuration correspondante pourra être **décommentée**, puis le projet pourra être construit afin de vérifier son fonctionnement.

Cette méthode permet de faire évoluer progressivement le fichier de projet sans activer à l'avance les fonctionnalités des chapitres qui n'ont pas encore été étudiés.

## Fichier rendu

Le fichier principal rendu est :

[Voir le fichier `salle.jenga`](salle.jenga)

Il contient les configurations futures sous forme de commentaires, prêtes à être activées chapitre par chapitre.
