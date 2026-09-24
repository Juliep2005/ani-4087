# Exercice13

La commande utilisée est :

```text
jenga info -v --jenga-file salle.jenga
```

## Available Toolchains

| Nom        | Famille | Système cible | Architecture | Environnement |
| ---------- | ------- | ------------- | ------------ | ------------- |
| `host-gcc` | gcc     | Windows       | x86_64       | mingw         |
| `mingw`    | gcc     | Windows       | x86_64       | mingw         |

## Ce qui est présent

Deux toolchains sont détectées par Jenga :

* `host-gcc`
* `mingw`

Elles utilisent toutes les deux GCC, ciblent Windows en architecture x86_64 et utilisent l'environnement MinGW.

## Ce qui manque

Le tableau ne présente aucune toolchain ciblant Linux ou macOS.

La machine dispose donc, du point de vue de Jenga, de toolchains GCC pour **Windows x86_64**, mais aucune toolchain Linux ou macOS n'est actuellement détectée.

## Informations sur la machine

* Système hôte : Windows
* Architecture : x86_64
* Environnement hôte : MSVC
* Python : 3.10.11
* Version de Jenga : 2.8.1

## Conclusion

La machine est actuellement équipée des toolchains nécessaires pour construire le projet sous Windows avec GCC/MinGW. Les toolchains pour les autres systèmes cibles ne sont pas affichées par Jenga et n'ont donc pas pu être vérifiées comme disponibles sur cette machine.
