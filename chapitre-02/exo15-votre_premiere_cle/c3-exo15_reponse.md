# Exercice15

## Objectif

L'objectif est de générer une clé de signature permettant de signer une application Android.


## Commande utilisée

La commande `keytool` a été utilisée pour générer le keystore :

```bash
keytool -genkeypair -v -keystore "C:\Users\Dell\Documents\AndroidKeys\salle-release.keystore" -alias salle -keyalg RSA -keysize 2048 -validity 10000
```

Lors de l'exécution, `keytool` demande le mot de passe du keystore ainsi que les informations nécessaires à la création de la clé.

## Emplacement de la clé

Le fichier de signature est enregistré à l'emplacement suivant :

```text
C:\Users\Dell\Documents\AndroidKeys\salle-release.keystore
```

Cet emplacement est situé en dehors du dépôt du projet.

## Emplacement du mot de passe

Le mot de passe associé au keystore est conservé séparément du dépôt, dans un emplacement personnel sécurisé.

Il n'est pas enregistré dans le dépôt Git, dans le fichier `README.md` ou dans le fichier `salle.jenga`.



