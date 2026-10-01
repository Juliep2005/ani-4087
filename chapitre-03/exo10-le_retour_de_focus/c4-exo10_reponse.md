# Exercice — Accumulateur et perte de focus

## Objectif

Observer le comportement de l'accumulateur de `NkMouseRawEvent` lorsque la fenêtre perd puis récupère le focus.

L'expérience est réalisée dans deux situations :

1. Sans remise à zéro de l'accumulateur lorsque la fenêtre perd le focus.
2. Avec remise à zéro de l'accumulateur lorsque la fenêtre perd le focus.

## Expérience

Dans les deux cas :

* la fenêtre est lancée avec l'accumulateur actif ;
* on clique sur une autre fenêtre ;
* on déplace la souris pendant environ **10 secondes** ;
* on revient ensuite sur la fenêtre de l'exercice ;
* on observe la valeur de l'accumulateur.

## Cas 1 — Sans remise à zéro hors focus

Lorsque la fenêtre perd le focus, l'accumulateur n'est pas remis à zéro.

Les mouvements effectués pendant les **10 secondes** peuvent donc rester conservés. Lors du retour sur la fenêtre, une valeur accumulée peut être consommée.

On peut alors observer un comportement de ce type :

```text
Fenêtre hors focus
        ↓
Déplacements de la souris
        ↓
Accumulation des événements
        ↓
Retour dans la fenêtre
        ↓
Consommation du total accumulé
```

L'accumulateur conserve donc des mouvements qui ont été effectués pendant que l'utilisateur était sur une autre fenêtre.

## Cas 2 — Avec remise à zéro hors focus

Lorsque la fenêtre perd le focus, l'accumulateur est remis à zéro.

Les mouvements effectués pendant les **10 secondes** hors de la fenêtre ne sont donc pas conservés pour le retour.

Le comportement devient :

```text
Fenêtre hors focus
        ↓
Remise à zéro de l'accumulateur
        ↓
Déplacements de la souris
        ↓
Retour dans la fenêtre
        ↓
Accumulateur propre
```

## Observation

La comparaison montre que la remise à zéro lors de la perte de focus évite de conserver des mouvements qui ont été effectués alors que la fenêtre n'était plus utilisée.

| Situation              | Comportement                                                      |
| ---------------------- | ----------------------------------------------------------------- |
| Sans remise à zéro     | Des mouvements peuvent rester accumulés pendant la perte de focus |
| Avec remise à zéro     | L'accumulateur repart de zéro                                     |
| Retour dans la fenêtre | Aucun ancien mouvement n'est réinjecté avec la remise à zéro      |

## Rapprochement avec le commentaire du simulateur

Cette expérience met en évidence l'importance de vider l'état accumulé lorsqu'une fenêtre perd le focus.

Sans remise à zéro, des événements anciens peuvent être consommés au retour dans la fenêtre et ne correspondent donc plus à l'action actuelle de l'utilisateur.

Avec la remise à zéro, les mouvements effectués pendant la perte de focus sont ignorés et l'accumulateur repart proprement lorsque la fenêtre reprend le focus.

## Conclusion

La remise à zéro de l'accumulateur lors de la perte de focus permet d'éviter qu'un ancien mouvement de souris soit appliqué brutalement au retour dans la fenêtre. Elle permet ainsi de conserver un état cohérent avec l'interaction actuelle de l'utilisateur.
