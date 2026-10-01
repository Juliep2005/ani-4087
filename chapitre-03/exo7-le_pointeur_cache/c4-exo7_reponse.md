# Exercice 7 : Le pointeur caché

## Objectif 

L'objectif de cet exercice est d'étudier le comportement de la souris lorsque son curseur est caché et confiné à la fenêtre.

Il faut :

* cacher le curseur ;
* confiner le curseur à la fenêtre ;
* afficher à chaque image sa position `x` et `y` ;
* afficher également son `rawDelta` ;
* déplacer la souris jusqu'à atteindre un bord ;
* comparer les deux séries de valeurs ;
* déterminer laquelle continue à évoluer lorsque la position du curseur est bloquée ;
* expliquer pourquoi cette valeur est adaptée au contrôle d'une caméra.

---

## Mise en place

Deux fonctions de `NkWindow` sont utilisées pour gérer le curseur :

```cpp
fenetre.ShowMouse(false);
fenetre.ClipMouseToClient(true);
```

La première permet de cacher le curseur.

La seconde confine le curseur à la zone cliente de la fenêtre.

Dans la boucle principale, les informations relatives à la souris sont récupérées avec :

```cpp
const nkentseu::int32 x =
    nkentseu::NkInput.MouseX();

const nkentseu::int32 y =
    nkentseu::NkInput.MouseY();

const nkentseu::int32 rdx =
    nkentseu::NkInput.MouseRawDeltaX();

const nkentseu::int32 rdy =
    nkentseu::NkInput.MouseRawDeltaY();
```

Les valeurs sont affichées uniquement lorsqu'un déplacement est détecté :

```cpp
if (rdx != 0 || rdy != 0){

    std::cout << "img " << images
              << " : x=" << x
              << " y=" << y
              << " | rawDelta=("
              << rdx << ","
              << rdy << ")"
              << std::endl;
}
```

À la fin du programme, le confinement est désactivé et le curseur est rendu visible :

```cpp
fenetre.ClipMouseToClient(false);
fenetre.ShowMouse(true);
```

Cette étape permet de laisser l'environnement dans son état normal après l'exécution.

---

## Première observation : la position et le `rawDelta` évoluent ensemble

Au cours du déplacement, les deux informations évoluent simultanément.

Par exemple :

```text
img 86319 : x=1153 y=1 | rawDelta=(7,1)
img 86320 : x=1153 y=1 | rawDelta=(7,1)
img 86321 : x=1153 y=1 | rawDelta=(7,1)
img 86322 : x=1153 y=1 | rawDelta=(5,0)
img 86323 : x=1153 y=1 | rawDelta=(5,0)
```

Le `rawDelta` indique ici que la souris continue à recevoir un déplacement horizontal, avec par exemple `5` ou `7` pixels.

Lorsque le curseur n'est pas encore bloqué par une limite, la position et le déplacement permettent tous les deux de suivre le mouvement de la souris.

---

## Observation au bord de la fenêtre

Le phénomène devient particulièrement intéressant lorsque le curseur atteint une limite.

On observe par exemple :

```text
img 86394 : x=1272 y=0 | rawDelta=(7,-5)
img 86395 : x=1272 y=0 | rawDelta=(7,-5)
img 86396 : x=1272 y=0 | rawDelta=(9,-6)
img 86397 : x=1272 y=0 | rawDelta=(8,-6)
img 86398 : x=1272 y=0 | rawDelta=(8,-6)
img 86399 : x=1272 y=0 | rawDelta=(8,-6)
img 86400 : x=1272 y=0 | rawDelta=(8,-6)
```

La position reste alors :

```text
x = 1272
y = 0
```

alors que le `rawDelta` continue à changer.

On observe également :

```text
img 86957 : x=1272 y=0 | rawDelta=(-6,-1)
img 86968 : x=1272 y=0 | rawDelta=(-7,0)
img 86978 : x=1272 y=0 | rawDelta=(-6,0)
img 86989 : x=1272 y=0 | rawDelta=(-5,1)
img 87000 : x=1272 y=0 | rawDelta=(-5,2)
```

La souris continue donc bien à produire des déplacements alors que sa position reste fixée au bord.

---

## Comparaison des deux séries

| Information                             | Comportement observé                                                        |
| --------------------------------------- | --------------------------------------------------------------------------- |
| `MouseX()` / `MouseY()`                 | La position évolue puis se bloque lorsqu'une limite est atteinte            |
| `MouseRawDeltaX()` / `MouseRawDeltaY()` | Le déplacement continue à être détecté même lorsque la position est bloquée |

Le résultat principal de l'expérience est donc :

> **La position `x,y` finit par être figée au bord tandis que le `rawDelta` continue de varier.**

Dans mes mesures, la position est notamment restée à :

```text
x = 1272
y = 0
```

pendant que les valeurs de `rawDelta` continuaient à changer, par exemple :

```text
(7,-5)
(9,-6)
(8,-6)
(6,-4)
(3,-1)
(1,-1)
(-6,0)
(-5,2)
```

---

## Quelle série continue de bouger ?

C'est **le `rawDelta`** qui continue de bouger.

La différence vient de la nature des deux informations.

La position `x,y` indique **où se trouve le curseur dans la fenêtre**. Comme le curseur est confiné, cette position possède nécessairement des limites. Une fois une limite atteinte, elle ne peut plus continuer dans cette direction.

Le `rawDelta`, au contraire, indique **le déplacement détecté de la souris**. Il ne décrit pas une position fixe dans la fenêtre. Il peut donc continuer à signaler un mouvement même lorsque le curseur ne peut plus avancer.

---

## Pourquoi utiliser le `rawDelta` pour une caméra ?

Le `rawDelta` est particulièrement adapté au contrôle d'une caméra ou d'une vue à la première personne.

### La caméra ne doit pas s'arrêter au bord

Si la caméra était contrôlée directement par la position `x,y`, elle finirait par ne plus tourner lorsque le curseur atteindrait une limite.

Dans les résultats obtenus, on observe par exemple :

```text
x=1272 y=0 | rawDelta=(8,-6)
```

La position ne change plus, mais la souris continue à être déplacée.

Une caméra utilisant uniquement `x,y` ne pourrait donc plus interpréter correctement ce mouvement.

Avec `rawDelta`, elle peut continuer à tourner.

### Le déplacement réel reste disponible

Les valeurs :

```text
rawDelta=(9,-6)
rawDelta=(8,-6)
rawDelta=(7,-5)
rawDelta=(6,-4)
```

montrent que le mouvement de la souris continue d'être détecté.

Le programme peut donc utiliser directement ces variations pour modifier l'orientation de la caméra.

### La position dépend des limites de la fenêtre

La position du curseur est liée à la zone dans laquelle il est autorisé à se déplacer.

Le `rawDelta` est différent : il décrit le déplacement de la souris plutôt que sa position absolue dans la fenêtre.

Pour une caméra, on s'intéresse justement au mouvement :

```text
déplacement horizontal → rotation gauche/droite
déplacement vertical → rotation haut/bas
```

Le `rawDelta` correspond donc directement à cette utilisation.

---

## Résultat de l'exécution

Le programme s'est terminé normalement après :

```text
Images : 90410
```

La durée totale observée lors de cette exécution était :

```text
31.71 s
```

Une partie importante des images montre que le `rawDelta` continue à évoluer alors que la position reste bloquée à :

```text
x=1272
y=0
```

Exemple représentatif :

```text
img 86405 : x=1272 y=0 | rawDelta=(7,-5)
img 86406 : x=1272 y=0 | rawDelta=(7,-5)
img 86407 : x=1272 y=0 | rawDelta=(7,-5)
img 86408 : x=1272 y=0 | rawDelta=(7,-5)
img 86409 : x=1272 y=0 | rawDelta=(6,-4)
img 86410 : x=1272 y=0 | rawDelta=(6,-4)
```

Ces observations suffisent à montrer expérimentalement que les deux informations ne se comportent pas de la même manière au bord.

---

## Conclusion

Cette expérience montre la différence entre une **position absolue** et un **déplacement relatif**.

La position `x,y` permet de connaître l'emplacement du curseur dans la fenêtre, mais elle est limitée par les frontières de cette fenêtre. Lorsqu'un curseur confiné atteint un bord, sa position peut donc rester inchangée.

Le `rawDelta`, lui, continue à indiquer le déplacement de la souris. Dans l'expérience, plusieurs images montrent ainsi une position fixe à `x=1272, y=0` alors que le `rawDelta` continue de prendre différentes valeurs.

Pour une interaction de type caméra, le `rawDelta` est donc l'information pertinente : il permet de transformer directement le mouvement de la souris en mouvement de la vue, sans dépendre de la position du curseur dans la fenêtre.


