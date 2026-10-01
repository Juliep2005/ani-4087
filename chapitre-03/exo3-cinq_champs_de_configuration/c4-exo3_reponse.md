# Exercice — Cinq champs de configuration

## Objectif

L'objectif de cet exercice est d'explorer des champs supplémentaires de la structure `NkWindowConfig` qui n'ont pas été présentés dans le chapitre.

Pour cela, cinq champs ont été sélectionnés dans le fichier `NkWindowConfig.h`. Chaque champ a été testé individuellement afin d'observer son influence sur le comportement ou l'apparence de la fenêtre.

L'objectif est également de comparer le comportement attendu avec le comportement réellement observé lors de l'exécution.

---

## Champs testés

Les cinq champs sélectionnés sont :

* `resizable`
* `minimizable`
* `maximizable`
* `hasShadow`
* `alwaysOnTop`

Chaque modification a été effectuée séparément afin de pouvoir identifier précisément l'effet de chaque paramètre.

---

## Test de cinq champs supplémentaires de `NkWindowConfig`

| Champ testé   | Valeur           | Ce que j'attendais                               | Observation                 | Pourquoi ?                                                                                                                            |
| ------------- | ---------------- | ------------------------------------------------ | --------------------------- | ------------------------------------------------------------------------------------------------------------------------------------- |
| `resizable`   | `false` / `true` | Pouvoir ou non redimensionner la fenêtre         | Aucun changement visible    | Le paramètre ne semble pas être appliqué visiblement par le backend utilisé sous Windows.                                             |
| `minimizable` | `false` / `true` | Désactiver ou activer la réduction de la fenêtre | Aucun changement visible    | La gestion de la réduction semble rester contrôlée par le système ou le backend.                                                      |
| `maximizable` | `false` / `true` | Désactiver ou activer la maximisation            | Aucun changement visible    | Aucun effet visible avec les deux valeurs ; le paramètre ne semble pas être appliqué dans cette configuration.                        |
| `hasShadow`   | `false` / `true` | Faire apparaître ou disparaître l'ombre          | Aucun changement visible    | L'ombre semble être gérée par Windows indépendamment de ce paramètre.                                                                 |
| `alwaysOnTop` | `true`           | Garder la fenêtre au-dessus des autres           | **Le changement est très visible : la fenêtre reste constamment au premier plan. Il a même été nécessaire d'utiliser le Gestionnaire des tâches pour pouvoir la fermer.** | Le paramètre est bien pris en compte : la fenêtre reste au premier plan. Pour la fermer, j'ai dû utiliser le Gestionnaire des tâches. |
                                       


### Conclusion


Cette expérimentation a permis de tester cinq paramètres supplémentaires de `NkWindowConfig`.

Les quatre premiers champs testés (`resizable`, `minimizable`, `maximizable` et `hasShadow`) n'ont pas produit de changement visible dans l'environnement de test.

En revanche, le champ `alwaysOnTop` a produit un effet très net : avec la valeur `true`, la fenêtre reste toujours au-dessus des autres fenêtres.

Cette expérience montre que certains paramètres de configuration peuvent avoir un effet directement observable, tandis que d'autres peuvent ne pas produire de différence visible selon l'environnement ou la manière dont la fenêtre est gérée par le système.
