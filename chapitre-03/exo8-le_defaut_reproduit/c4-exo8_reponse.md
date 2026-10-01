# Exercice 8 — Le defaut reproduit

## 1. Objectif

L'objectif de cet exercice est d'observer directement la valeur de `rawDeltaX` à chaque image.

La consigne demande de :

* afficher `rawDeltaX` à chaque image ;
* ne réaliser aucune accumulation soi-même ;
* déplacer la souris ;
* arrêter le mouvement de la souris ;
* relever les vingt lignes qui suivent l'arrêt ;
* expliquer ce que ces valeurs permettent de constater.

---

## 2. Lecture de `rawDeltaX`

La valeur de `rawDeltaX` est récupérée directement avec :

```cpp
const nkentseu::int32 rawDeltaX =
    nkentseu::NkInput.MouseRawDeltaX();
```

Elle est ensuite affichée directement dans la console :

```cpp
std::cout << "img " << images
          << " : rawDeltaX=" << rawDeltaX
          << std::endl;
```

Aucune accumulation n'est réalisée dans le programme.

Chaque ligne correspond donc à la valeur de `rawDeltaX` observée pour une image donnée.

---

## 3. Manipulation réalisée

Pour réaliser l'expérience :

1. Le programme est lancé.
2. La souris est déplacée pendant l'exécution.
3. La main est ensuite posée afin d'arrêter complètement le mouvement.
4. La souris n'est plus touchée.
5. Les vingt images qui suivent l'arrêt sont relevées.

---

## 4. Résultats

Les vingt lignes relevées après l'arrêt de la souris sont :

```text
img 13688 : rawDeltaX=0
img 13689 : rawDeltaX=0
img 13690 : rawDeltaX=0
img 13691 : rawDeltaX=0
img 13692 : rawDeltaX=0
img 13693 : rawDeltaX=0
img 13694 : rawDeltaX=0
img 13695 : rawDeltaX=0
img 13696 : rawDeltaX=0
img 13697 : rawDeltaX=0
img 13698 : rawDeltaX=0
img 13699 : rawDeltaX=0
img 13700 : rawDeltaX=0
img 13701 : rawDeltaX=0
img 13702 : rawDeltaX=0
img 13703 : rawDeltaX=0
img 13704 : rawDeltaX=0
img 13705 : rawDeltaX=0
img 13706 : rawDeltaX=0
img 13707 : rawDeltaX=0
```

On constate que les vingt valeurs sont égales à `0`.

---

## 5. Interprétation

Lorsque la souris est immobile, aucun déplacement horizontal n'est détecté.

Ainsi, à chaque nouvelle image, la valeur retournée par `MouseRawDeltaX()` est :

```text
rawDeltaX = 0
```

La valeur est affichée directement à chaque image et aucune somme n'est effectuée par le programme.

---


## 6. Conclusion

L'expérience confirme que `rawDeltaX` représente le déplacement horizontal détecté pour chaque image. Lorsque la souris est déplacée, sa valeur peut varier ; lorsque la souris s'arrête, elle revient à `0` et reste à `0` tant qu'aucun nouveau mouvement horizontal n'est effectué.
