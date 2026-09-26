# Exercice16

## 1. Empaquetage du programme

Le programme `MaSalle` a été compilé pour Android avec l'architecture `arm64-v8a`.

La compilation et l'édition des liens ont été réalisées avec succès :

```text
✓ Built: Build\Bin\Release-Android\MaSalle\libMaSalle.so
✓ Build Successful
```

L'application a ensuite été empaquetée sous la forme d'un fichier APK Android.

Le paquet final obtenu est :

```text
MaSalle-Release-signed.apk
```

Chemin du paquet :

```text
Build\Bin\Release-Android\MaSalle\android-build-arm64-v8a\MaSalle-Release-signed.apk
```

## 2. Signature du paquet

L'APK a été aligné avec `zipalign`, puis signé à l'aide du keystore Android :

```text
C:\Users\Dell\.android\debug.keystore
```

La vérification avec `apksigner` a confirmé la validité de la signature :

```text
Verified using v2 scheme (APK Signature Scheme v2): true
Verified using v3 scheme (APK Signature Scheme v3): true
Number of signers: 1
```

Le paquet est donc correctement signé.

## 3. Taille du paquet

La taille du fichier APK signé obtenu est :

```text
37 283 octets
```

Soit environ :

```text
36,41 KiB
```

## 4. Contenu du paquet

Comme un fichier APK est une archive, son contenu a été affiché avec la commande :

```bat
tar -tf "Build\Bin\Release-Android\MaSalle\android-build-arm64-v8a\MaSalle-Release-signed.apk"
```

La liste obtenue est :

```text
AndroidManifest.xml
resources.arsc
lib/arm64-v8a/libMaSalle.so
META-INF/ANDROIDD.SF
META-INF/ANDROIDD.RSA
META-INF/MANIFEST.MF
```

### Rôle des principaux éléments

| Élément                       | Description                                                                       |
| ----------------------------- | --------------------------------------------------------------------------------- |
| `AndroidManifest.xml`         | Décrit la configuration et les informations principales de l'application Android. |
| `resources.arsc`              | Contient les ressources Android compilées.                                        |
| `lib/arm64-v8a/libMaSalle.so` | Bibliothèque native contenant le programme C++ compilé pour l'architecture ARM64. |
| `META-INF/ANDROIDD.SF`        | Fichier associé à la signature du paquet.                                         |
| `META-INF/ANDROIDD.RSA`       | Contient les informations cryptographiques liées à la signature.                  |
| `META-INF/MANIFEST.MF`        | Manifest de l'archive utilisé dans le processus de signature.                     |

## 5. Résultat

L'application `MaSalle` a donc été :

* compilée pour Android ;
* empaquetée dans un fichier `.apk` ;
* alignée avec `zipalign` ;
* signée avec un keystore ;
* vérifiée avec `apksigner` ;
* inspectée comme une archive afin d'en relever le contenu.

**Taille finale du paquet : 37 283 octets.**
