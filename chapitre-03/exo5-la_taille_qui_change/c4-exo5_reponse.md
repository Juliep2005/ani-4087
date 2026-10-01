# Exercice 5 — La taille qui change

## Objectif

L’objectif de cet exercice est d’écouter l’événement `NkWindowResizeEvent` afin d’afficher dans la console la nouvelle taille de la fenêtre à chaque changement.

Deux situations sont testées :

* un redimensionnement **lent et progressif** ;
* un redimensionnement **rapide effectué d’un seul coup**.

Le but est ensuite de comparer les événements reçus dans les deux cas.

---

## Principe

Un callback est associé à l’événement `NkWindowResizeEvent`.

À chaque réception de cet événement, la largeur et la hauteur de la fenêtre sont récupérées avec :

```cpp
event->GetWidth()
event->GetHeight()
```

Puis elles sont affichées dans la console.

Le code utilisé est :

```cpp
#include <iostream>

evenements.AddEventCallback<nkentseu::NkWindowResizeEvent>(
    [&](nkentseu::NkWindowResizeEvent *event){
        std::cout << "Nouvelle taille : "
                  << event->GetWidth() << " x "
                  << event->GetHeight() << std::endl;
    }
);
```

Le traitement des événements reste assuré par :

```cpp
evenements.PollEvents();
```

dans la boucle principale.

---

## Test 1 — Redimensionnement lent

La fenêtre a été redimensionnée progressivement à l'aide de la souris.

La console a affiché un grand nombre de tailles intermédiaires, par exemple :

```text
Nouvelle taille : 1303x759
Nouvelle taille : 1285x712
Nouvelle taille : 1315x759
Nouvelle taille : 1297x712
Nouvelle taille : 1318x759
Nouvelle taille : 1300x712
Nouvelle taille : 1324x759
Nouvelle taille : 1306x712
Nouvelle taille : 1327x759
Nouvelle taille : 1309x712
...
Nouvelle taille : 1475x817
Nouvelle taille : 1457x770
Nouvelle taille : 1485x817
Nouvelle taille : 1467x770
...
Nouvelle taille : 1545x850
Nouvelle taille : 1527x803
...
Nouvelle taille : 1471x803
```

On constate que la taille de la fenêtre change progressivement et que plusieurs événements `NkWindowResizeEvent` sont générés pendant cette opération.

---

## Test 2 — Redimensionnement d’un coup

La fenêtre a ensuite été redimensionnée directement vers une nouvelle dimension.

Le résultat obtenu dans la console est :

```text
Dimension un coup : Nouvelle taille : 1920x991
```

Dans ce cas, un seul événement de redimensionnement a été observé, correspondant à la nouvelle dimension finale.

---

## 5. Comparaison des deux tests

| Type de redimensionnement | Observation                                                                |
| ------------------------- | -------------------------------------------------------------------------- |
| Redimensionnement lent    | De nombreux événements sont reçus avec différentes tailles intermédiaires. |
| Redimensionnement rapide  | Un seul événement a été observé avec la dimension finale `1920x991`.       |

Le redimensionnement lent produit donc une série importante de changements de dimensions, tandis que le redimensionnement effectué d’un seul coup produit ici uniquement la dimension finale observée.

---

## 6. Conclusion

Cet exercice montre que `NkWindowResizeEvent` permet de détecter les changements de taille de la fenêtre et de récupérer ses nouvelles dimensions.

Lors d’un redimensionnement lent, plusieurs événements sont reçus au fur et à mesure de l’évolution de la fenêtre. Les dimensions affichées correspondent alors aux différentes tailles intermédiaires.

Lors d’un redimensionnement effectué d’un seul coup, un seul événement a été observé dans notre test, avec la dimension finale `1920x991`.

On peut donc conclure que **le nombre d’événements reçus dépend du déroulement du redimensionnement** : une opération progressive peut générer de nombreux événements, alors qu’un changement direct peut ne faire apparaître que la taille finale.

Cet exercice permet ainsi de mieux comprendre le fonctionnement des événements de fenêtre et l’importance de `PollEvents()` pour leur traitement.
