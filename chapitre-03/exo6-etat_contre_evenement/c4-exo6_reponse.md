# Exercice 6 — Etat contre evenement

## Objectif

L'objectif de cet exercice est de comparer deux façons de détecter l'utilisation d'une touche :

* `NkInput.IsKeyDown`, qui permet de savoir si une touche est actuellement maintenue ;
* `NkKeyPressEvent`, qui permet de détecter un événement de pression sur une touche.

Deux compteurs sont utilisés afin d'observer la différence entre ces deux mécanismes.

---

## Principe

Le premier compteur est incrémenté lorsque la touche **Espace est maintenue** :

```cpp
if (nkentseu::NkInput.IsKeyDown(nkentseu::NkKey::NK_SPACE)){
    ++compteurEtat;
}
```

Ce test est effectué à chaque tour de la boucle principale.

Le deuxième compteur est incrémenté lorsqu'un événement `NkKeyPressEvent` correspondant à la touche Espace est reçu :

```cpp
evenements.AddEventCallback<nkentseu::NkKeyPressEvent>(
    [&](nkentseu::NkKeyPressEvent *event){

        if (event->GetKey() == nkentseu::NkKey::NK_SPACE){
            ++compteurEvenement;
        }

        if (event->GetKey() == nkentseu::NkKey::NK_ESCAPE){
            tourne = false;
        }
    }
);
```

---

## Code utilisé

```cpp
#include <NKWindow/NKWindow.h>
#include <NKEvent/NkEvent.h>
#include <NKWindow/NKMain.h>
#include <iostream>


int nkmain(const nkentseu::NkEntryState &state){
    nkentseu::NkWindowConfig config;

    config.title = "Ma salle Juliette";
    config.width = 1280;
    config.height = 720;
    config.minimizable = true;

    nkentseu::NkWindow fenetre(config);

    if (!fenetre.IsValid()){
        return 1;
    }

    bool tourne = true;

    using uint64 = std::uint64_t;
    uint64 compteurEtat = 0;
    uint64 compteurEvenement = 0;

    nkentseu::NkEventSystem &evenements = nkentseu::NkEvents();

    evenements.AddEventCallback<nkentseu::NkKeyPressEvent>(
        [&](nkentseu::NkKeyPressEvent *event){

            if (event->GetKey() == nkentseu::NkKey::NK_SPACE){
                ++compteurEvenement;
            }

            if (event->GetKey() == nkentseu::NkKey::NK_ESCAPE){
                tourne = false;
            }
        }
    );

    evenements.AddEventCallback<nkentseu::NkWindowCloseEvent>(
        [&](nkentseu::NkWindowCloseEvent *){
            tourne = false;
        }
    );

    while (tourne){ 
        evenements.PollEvents(); 

        if (nkentseu::NkInput.IsKeyDown(nkentseu::NkKey::NK_SPACE)){
            ++compteurEtat;
        }

        // nkentseu::NkClock::Sleep((int64)10);
    }

    fenetre.Close();

    std::cout << "Compteur IsKeyDown : " << compteurEtat << std::endl;
    std::cout << "Compteur NkKeyPressEvent : "<< compteurEvenement << std::endl;
    
    return 0;
}
```

---

## Test réalisé

Pour réaliser le test :

1. Le programme est lancé.
2. La touche **Espace** est maintenue pendant environ une seconde.
3. La touche **Espace** est relâchée.
4. La touche **Échap** est ensuite utilisée pour terminer le programme.

Le programme s'est terminé normalement après environ **39,93 secondes** d'exécution totale.

---

## Résultats obtenus

La console affiche :

```text
Compteur IsKeyDown : 54774
Compteur NkKeyPressEvent : 1
```

Les résultats sont donc :

| Méthode             | Nombre obtenu |
| ------------------- | ------------: |
| `NkInput.IsKeyDown` |    **54 774** |
| `NkKeyPressEvent`   |         **1** |

---

## Explication de l'écart

L'écart important entre les deux compteurs s'explique par leur fonctionnement différent.

### `NkInput.IsKeyDown`

`IsKeyDown` permet de connaître **l'état actuel de la touche**.

Dans notre programme, cette fonction est testée à chaque tour de la boucle :

```cpp
if (nkentseu::NkInput.IsKeyDown(nkentseu::NkKey::NK_SPACE)){
    ++compteurEtat;
}
```

Tant que la touche Espace est considérée comme maintenue, le compteur peut donc être incrémenté de nombreuses fois.

C'est pourquoi on obtient :

```text
54 774
```

### `NkKeyPressEvent`

`NkKeyPressEvent` correspond à un **événement de pression**.

Dans notre test, une pression sur Espace produit un événement :

```text
1
```

Le compteur n'est donc pas incrémenté à chaque tour de boucle pendant que la touche reste enfoncée.

---

## Conclusion

Cette expérience montre clairement la différence entre les deux mécanismes.

`IsKeyDown` sert à connaître **l'état d'une touche** et est interrogé continuellement dans la boucle du programme. Une touche maintenue peut donc provoquer un grand nombre d'incréments du compteur.

`NkKeyPressEvent` correspond quant à lui à **l'événement de pression de la touche**. Dans notre test, une seule pression sur Espace a produit un seul événement.

On obtient ainsi :

```text
IsKeyDown          → 54 774
NkKeyPressEvent    → 1
```

L'écart observé montre donc que **`IsKeyDown` et `NkKeyPressEvent` ne mesurent pas la même chose : le premier renseigne sur un état, tandis que le second signale une pression sous forme d'événement.**
