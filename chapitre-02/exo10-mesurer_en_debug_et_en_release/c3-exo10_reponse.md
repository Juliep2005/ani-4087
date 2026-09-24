# Exercice10

## Objectif

Mesurer le temps d'exécution d'un calcul lourd en configuration Debug puis en configuration Release, afin de comparer les performances.

## Calcul effectué

Un calcul répétitif a été réalisé dans une boucle de 500 000 000 itérations. Le temps d'exécution est mesuré avec `std::chrono`.

## Résultats

| Configuration            | Temps de calcul |
| ------------------------ | --------------: |
| Debug                    |      1471,81 ms |
| Release                  |      1348,03 ms |
| Durée d'une image casque |           11 ms |

La configuration Release est plus rapide que la configuration Debug pour ce calcul.

## Comparaison avec les 11 ms

Une image de casque dure 11 ms.

* Debug : 1471,81 ms ≫ 11 ms
* Release : 1348,03 ms ≫ 11 ms

Les deux mesures dépassent donc largement le budget de 11 ms.

## Conclusion

La mesure qui aurait pu faire prendre une mauvaise décision est la mesure **Release**, si elle était utilisée pour conclure que le calcul pouvait être exécuté dans une image simplement parce que Release est plus rapide que Debug.

Cependant, dans notre cas, les deux mesures sont supérieures à 11 ms. Elles conduisent donc à la même conclusion : ce calcul ne peut pas tenir dans le budget d'une seule image du casque.

La différence entre Debug et Release montre néanmoins qu'une mesure effectuée dans une seule configuration ne représente pas nécessairement les performances de l'autre configuration.
