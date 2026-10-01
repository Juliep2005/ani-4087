## Exercice 2 : La fenetre qui ne repond pas

Pour cette expérience, l'appel à `PollEvents()` a été supprimé du corps de la boucle :

```cpp
while (fenetre.IsOpen()){
    // Aucun traitement des événements
}
```

Après compilation et exécution, la fenêtre **Ma salle Juliette** s'ouvre normalement. Cependant, comme aucun événement n'est traité, le programme reste dans une boucle qui s'exécute continuellement.

Sur ma machine, la fenêtre reste affichée et semble fonctionner pendant une durée indéterminée. Le système n'affiche pas immédiatement le message indiquant que la fenêtre ne répond pas.

Le message **« Ne répond pas »** apparaît lorsque je clique sur une autre fenêtre. À ce moment-là, la fenêtre `Ma salle Juliette` est considérée comme bloquée. J'ai ensuite dû forcer l'arrêt du programme.

**Observation :** sans `PollEvents()`, la fenêtre ne traite plus les événements du système. La boucle continue donc de s'exécuter sans permettre à l'application de gérer correctement les événements de la fenêtre.

**Temps avant le blocage :** non mesurable précisément sur ma machine, car la fenêtre reste affichée indéfiniment tant qu'aucune interaction avec une autre fenêtre ne provoque la détection du blocage.

**Capture :** ![Fenêtre NKWindow](capture_fenetre_bloquee.png)
