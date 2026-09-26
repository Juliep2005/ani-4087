# Exercice19

## 1. Crash volontaire

Pour provoquer volontairement un crash, nous avons déréférencé un pointeur nul :

```cpp
int* pointeur = nullptr;
*pointeur = 42;
```

Cette opération provoque une erreur de segmentation (`SIGSEGV`).

## 2. Lecture de la trace

Après avoir lancé l'application sur l'appareil, nous avons consulté le journal avec :

```bat
"C:\Android\platform-tools\adb.exe" logcat
```

Pour rechercher directement le crash :

```bat
"C:\Android\platform-tools\adb.exe" logcat | findstr /I "F DEBUG SIGSEGV Fatal signal"
```

La trace obtenue contient notamment :

```text
signal 11 (SIGSEGV)
fault addr 0x00000000
backtrace:
#00 ... libMaSalle.so
#01 ... libMaSalle.so
```

## 3. Ce que la trace nous apprend

La trace montre :

* qu'un `SIGSEGV` s'est produit ;
* que l'adresse fautive est `0x00000000` ;
* que le crash concerne notre bibliothèque native `libMaSalle.so` ;
* la pile d'appels (`backtrace`) au moment du crash.

## 4. Ce qu'elle ne nous apprend pas directement

La trace brute ne donne pas forcément :

* la ligne exacte du fichier `.cpp` ;
* la raison pour laquelle le pointeur est devenu nul ;
* la cause logique complète de l'erreur.

### Conclusion

La trace permet d'identifier **la nature et le contexte du crash**, mais elle ne suffit pas toujours à déterminer précisément **la ligne et la cause logique de l'erreur**.
