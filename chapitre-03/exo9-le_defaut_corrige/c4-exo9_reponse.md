# # Exercice 9 

## Objectif

Mettre en place un accumulateur avec `NkMouseRawEvent` et comparer son résultat avec la lecture directe de `NkInput.MouseRawDeltaX()`.

## Principe

À chaque événement `NkMouseRawEvent`, le déplacement horizontal est ajouté à l'accumulateur :

```cpp
accumulateurRawDeltaX += event->GetDeltaX();
```

À chaque image, le total accumulé est récupéré puis l'accumulateur est remis à zéro :

```cpp
const nkentseu::int64 rawDeltaXAccumule =
    accumulateurRawDeltaX;

accumulateurRawDeltaX = 0;
```

Les deux valeurs sont ensuite affichées côte à côte :

```text
direct=2 | accumule=2
direct=1 | accumule=1
direct=-1 | accumule=-1
direct=-2 | accumule=-2
direct=-3 | accumule=-3
```

## Résultats observés

Lors du déplacement de la souris, différentes valeurs positives et négatives ont été observées.

Exemples relevés :

```text
img 28991 : direct=2 | accumule=2

img 29007 : direct=1 | accumule=1

img 29090 : direct=-1 | accumule=-1

img 29104 : direct=-2 | accumule=-2

img 29122 : direct=-3 | accumule=-3

img 29133 : direct=-2 | accumule=-2

img 29148 : direct=-3 | accumule=-3

img 29226 : direct=-3 | accumule=-3

img 29259 : direct=-1 | accumule=-1

img 29292 : direct=-1 | accumule=-1

img 29326 : direct=-1 | accumule=-1

img 29340 : direct=0 | accumule=0
```

Lorsque la souris est restée immobile, les valeurs sont devenues nulles :

```text
direct=0 | accumule=0
```

La série s'est terminée à :

```text
Images : 29999
FIN D'EXECUTION — termine normalement (20.50s)
```

## Observation

Les deux mesures peuvent avoir des valeurs différentes sur certaines images. Par exemple, on observe parfois :

```text
direct=2 | accumule=0
```

Cela s'explique par le fait que les deux valeurs ne proviennent pas exactement du même mécanisme :

* `direct` utilise `NkInput.MouseRawDeltaX()`.
* `accumule` additionne les `NkMouseRawEvent` reçus pendant `PollEvents()`.
* L'accumulateur est ensuite remis à zéro à chaque image.

## Conclusion

L'expérience montre que `NkMouseRawEvent` permet de récupérer les déplacements bruts de la souris sous forme d'événements et de les accumuler avant leur consommation par l'image. Lorsque la souris est immobile, les deux mesures deviennent nulles.
