# Exercice2 —Deux configurations

## Objectif

L'objectif de cet exercice est de construire le projet en deux configurations différentes, **Debug** et **Release**, puis de comparer la taille des exécutables produits et le temps nécessaire à leur construction.

## Construction en Debug

La construction en mode Debug a été réalisée avec la commande :

```bash
jenga build --config Debug
```

### Résultats

* **Taille de l'exécutable Debug :** `73067 o`
* **Temps de construction Debug :** `0.29s`

## Construction en Release

La construction en mode Release a été réalisée avec la commande :

```bash
jenga build --config Release
```

### Résultats

* **Taille de l'exécutable Release :** `73067 o`
* **Temps de construction Release :** `0.25s`

## Comparaison

| Configuration | Taille de l'exécutable | Temps de construction |
| ------------- | ---------------------: | --------------------: |
| Debug         |                73067 o |                 0.29s |
| Release       |                73067 o |                 0.25s |


## Observation

Dans cette expérience, les deux exécutables ont exactement la même taille : 73067 octets.

Le temps de construction est également très proche. La construction en Debug a pris 0.29 s, tandis que la construction en Release a pris 0.25 s, soit une différence de 0.04 s.

Pour ce projet très simple, composé d'un seul fichier source main.cpp, la différence entre les deux configurations est donc faible au niveau de la taille de l'exécutable et du temps de construction.

Les quatre nombres demandés
- Debug — Taille : 73067 octets
- Debug — Temps : 0.29 s
- Release — Taille : 73067 octets
- Release — Temps : 0.25 s