# Exercice 4 — Fermer proprement

## Objectif

L'objectif de cet exercice est de mettre en pratique la gestion des événements avec Nkentseu.

Le programme doit permettre de quitter la boucle principale de deux manières :

* en cliquant sur le bouton de fermeture de la fenêtre ;
* en appuyant sur la touche **Échap** du clavier.

Pour cela, un booléen `tourne` est utilisé pour contrôler la boucle principale. Les deux événements doivent modifier ce même booléen afin que les deux chemins de sortie aboutissent au même endroit.

---

## Gestion de la fermeture de la fenêtre

L'événement :

```cpp
NkWindowCloseEvent
```

est utilisé pour détecter une demande de fermeture de la fenêtre.

Lorsque l'utilisateur clique sur le bouton **X**, le callback associé est exécuté :

```cpp
evenements.AddEventCallback<nkentseu::NkWindowCloseEvent>(
    [&](nkentseu::NkWindowCloseEvent *){
        tourne = false;
    }
);
```

La variable `tourne` passe alors de `true` à `false`.

La condition :

```cpp
while (tourne)
```

devient donc fausse et la boucle se termine.

---

## Gestion de la touche Échap

Le deuxième événement utilisé est :

```cpp
NkKeyPressEvent
```

Il permet de détecter l'appui sur une touche du clavier.

Dans notre cas, on vérifie si la touche pressée correspond à :

```cpp
nkentseu::NkKey::NK_ESCAPE
```

Le code est :

```cpp
evenements.AddEventCallback<nkentseu::NkKeyPressEvent>(
    [&](nkentseu::NkKeyPressEvent *event){
        if (event->GetKey() == nkentseu::NkKey::NK_ESCAPE){
            tourne = false;
        }
    }
);
```

Lorsque la touche **Échap** est pressée, `tourne` devient également `false`.

---

## Pourquoi les deux chemins de sortie aboutissent au même endroit ?

Les deux événements ont des actions différentes, mais ils doivent provoquer le même résultat : **quitter proprement la boucle principale**.

### Fermeture avec le bouton X

```text
Clic sur X
   ↓
NkWindowCloseEvent
   ↓
tourne = false
   ↓
while (tourne) se termine
```

### Fermeture avec Échap

```text
Appui sur Échap
   ↓
NkKeyPressEvent
   ↓
tourne = false
   ↓
while (tourne) se termine
```

Dans les deux cas, l'exécution arrive ensuite au même endroit :

```cpp
fenetre.Close();

return 0;
```

Cela évite de dupliquer le code de fermeture dans chaque callback. La fermeture effective de la fenêtre est donc centralisée à un seul endroit du programme.

---

## Construction et test

Le programme est construit avec Jenga à l'aide de :

```bash
jenga build
```

Après une compilation réussie, le programme est lancé.

Deux tests sont effectués :

### Test 1 — Fermeture avec le bouton X

La fenêtre est ouverte, puis le bouton **X** est utilisé.

Résultat attendu :

```text
NkWindowCloseEvent
        ↓
tourne = false
        ↓
fin de la boucle
        ↓
fenetre.Close()
```

### Test 2 — Fermeture avec Échap

La fenêtre est ouverte, puis la touche **Échap** est pressée.

Résultat attendu :

```text
NkKeyPressEvent
        ↓
NK_ESCAPE détecté
        ↓
tourne = false
        ↓
fin de la boucle
        ↓
fenetre.Close()
```

---

## Conclusion

Cet exercice montre comment utiliser le système d'événements de Nkentseu pour contrôler la fermeture d'une application.

Le booléen `tourne` sert de point commun aux deux mécanismes de sortie. Que l'utilisateur clique sur **X** ou appuie sur **Échap**, la variable passe à `false`, ce qui permet de quitter la boucle et d'effectuer la fermeture de la fenêtre au même endroit.

Cette organisation permet d'avoir un code plus clair et évite de dupliquer les opérations de fermeture.
