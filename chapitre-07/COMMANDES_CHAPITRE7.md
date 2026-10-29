# Commandes de compilation et d’exécution — Chapitre 7

Ce guide est prévu pour Windows PowerShell, le dépôt local existant, Jenga
2.8.19 et le toolchain `msys2-clang64`. Chaque bloc d’exercice est autonome :
il se place à la racine du workspace Jenga, compile uniquement la cible
indiquée, puis la lance.

## Préparation et conventions

Dans chaque bloc, les résultats existants sont copiés vers un fichier `.bak`
unique avant le lancement. Cela protège notamment les fichiers ouverts en
mode troncature par les programmes. Ne supprimez ces sauvegardes qu’après
avoir vérifié les nouveaux résultats.

Jenga doit être lancé depuis `chapitre-07`, car c’est là que se trouvent
`Chapitre7.jenga` et le chemin relatif vers le kit `../../projet/Kit/R3D.jenga`.
Le workspace déclare les configurations `Debug` et `Release`, la cible
Windows x86-64 et le dialecte C++17.

Toutes les cibles utilisent le kit R3D déclaré dans `Chapitre7.jenga` et
`common/ExerciseMath.h`. Les exercices graphiques 1 à 5, 8 et 9 utilisent
aussi `common/RoomSupport.h` et copient les shaders R3D après compilation.
Les exercices 6, 7 et 10 sont des simulations console sans salle graphique.

| Exercice | Cible Jenga | Source | Dépendance spécifique |
|---|---|---|---|
| 1 — Votre budget calculé | `exo1_budget` | `exo1-votre_budget_calcule/budget.cpp` | Salle R3D et shaders |
| 2 — Instant prédit et sa période | `exo2_instant` | `exo2-1_instant_predit_et_sa_periode/instant.cpp` | Salle R3D et shaders |
| 3 — Mesurer une vitesse | `exo3_vitesse` | `exo3-mesurer_une_vitesse/vitesse.cpp` | Salle R3D et shaders |
| 4 — Le chemin long provoque | `exo4_chemin` | `exo4-le_chemin_long_provoque/chemin.cpp` | Salle R3D et shaders |
| 5 — Prédire et comparer | `exo5_prediction` | `exo5-predire_et_comparer/prediction.cpp` | Salle R3D et shaders |
| 6 — Ce que la prédiction vous apporte | `exo6_gestes` | `exo6-ce_que_la_prediction_vous_apporte/gestes.cpp` | Simulation, pas de fenêtre |
| 7 — La latence injectée et la borne | `exo7_latence` | `exo7-la_latence_injectee_et_la_borne/latence.cpp` | Simulation, pas de fenêtre; Pillow pour la courbe |
| 8 — La stabilité mesurée | `exo8_stabilite` | `exo8-la_stabilite_mesuree/stabilite.cpp` | Salle R3D et shaders |
| 9 — Le verrou de demi-cadence | `exo9_verrou` | `exo9-le_verrou_de_demi_cadence/verrou.cpp` | Salle R3D et shaders |
| 10 — Un meilleur modèle | `exo10_modeles` | `exo10-un_meilleur_modele/modeles.cpp` | Simulation, pas de fenêtre |

La compilation place les exécutables dans
`Build/Bin/Release-Windows/<cible>/<cible>.exe`. La commande `jenga run`
utilise la même cible Jenga; elle ne relance pas une compilation implicite
après le `jenga build` explicite.

Les programmes écrivent les données à côté de leur source, via
`ExerciseOutputPath(__FILE__, ...)`, et non dans le répertoire courant.
Les simulations 6, 7 et 10 n’ont pas besoin de casque VR. Aucune des mesures
de ce guide ne constitue un essai sur casque.

## Exercice 1 — Votre budget calculé

**Répertoire de commande :** racine `chapitre-07`.

Le programme mesure 1 000 intervalles après une période de chauffe. Le premier
lancement utilise l’attente normale de l’écran; le second demande
`NK_VK_MAILBOX=1`. Il ajoute une ligne à `budget.txt` pour chaque lancement.
Le programme indique si l’API graphique active n’est pas Vulkan. La variable
demande MAILBOX, mais ne prouve pas que le pilote a retenu ce mode.

```powershell
$chap7 = 'C:\Users\Dell\Desktop\AIA4\semestre1\Coder_pour_la_VR_l_XR_et_AR1\ani-4087\chapitre-07'
Set-Location $chap7
$resultat = Join-Path $chap7 'exo1-votre_budget_calcule\budget.txt'
if (Test-Path -LiteralPath $resultat) {
    Copy-Item -LiteralPath $resultat -Destination "$resultat.$([guid]::NewGuid()).bak"
}

jenga build --jenga-file Chapitre7.jenga --config Release --target exo1_budget --toolchain msys2-clang64
if ($LASTEXITCODE -ne 0) { throw 'Échec de compilation de exo1_budget.' }

$ancienMailbox = [Environment]::GetEnvironmentVariable('NK_VK_MAILBOX', 'Process')
try {
    [Environment]::SetEnvironmentVariable('NK_VK_MAILBOX', $null, 'Process')
    jenga run exo1_budget --config Release --toolchain msys2-clang64 --jenga-file Chapitre7.jenga
    if ($LASTEXITCODE -ne 0) { throw 'Échec de la mesure avec attente écran.' }

    $env:NK_VK_MAILBOX = '1'
    jenga run exo1_budget --config Release --toolchain msys2-clang64 --jenga-file Chapitre7.jenga
    if ($LASTEXITCODE -ne 0) { throw 'Échec de la mesure avec NK_VK_MAILBOX=1.' }
}
finally {
    [Environment]::SetEnvironmentVariable('NK_VK_MAILBOX', $ancienMailbox, 'Process')
}
```

**Résultat produit :** `exo1-votre_budget_calcule/budget.txt`. Le programme
affiche aussi les budgets théoriques à 72, 90 et 120 Hz. Exemple déjà mesuré
sur cette machine : moyenne **16,710 ms** avec attente; **16,812 ms** avec
`NK_VK_MAILBOX=1`. Ces valeurs dépendent du poste et ne sont pas garanties.

## Exercice 2 — Instant prédit et sa période

**Répertoire de commande :** racine `chapitre-07`.

`MASALLE_HZ` fixe la fréquence utilisée pour calculer la période. Le lancement
à 72 Hz écrit les 20 instants affichés et **tronque** `instants.txt`; les
lancements à 60, 90 et 120 Hz ajoutent chacun leurs résultats au même fichier.
Le programme mesure 600 écarts par fréquence. `MASALLE_EXIT` vaut 700 par
défaut; la valeur `0` désactive cette limite de nombre d’images et laisse le
programme s’arrêter à la fin des 600 écarts.

```powershell
$chap7 = 'C:\Users\Dell\Desktop\AIA4\semestre1\Coder_pour_la_VR_l_XR_et_AR1\ani-4087\chapitre-07'
Set-Location $chap7
$resultat = Join-Path $chap7 'exo2-1_instant_predit_et_sa_periode\instants.txt'
if (Test-Path -LiteralPath $resultat) {
    Copy-Item -LiteralPath $resultat -Destination "$resultat.$([guid]::NewGuid()).bak"
}

jenga build --jenga-file Chapitre7.jenga --config Release --target exo2_instant --toolchain msys2-clang64
if ($LASTEXITCODE -ne 0) { throw 'Échec de compilation de exo2_instant.' }

$ancienHz = [Environment]::GetEnvironmentVariable('MASALLE_HZ', 'Process')
$ancienExit = [Environment]::GetEnvironmentVariable('MASALLE_EXIT', 'Process')
try {
    $env:MASALLE_EXIT = '0'
    foreach ($hz in @(72, 60, 90, 120)) {
        $env:MASALLE_HZ = [string]$hz
        jenga run exo2_instant --config Release --toolchain msys2-clang64 --jenga-file Chapitre7.jenga
        if ($LASTEXITCODE -ne 0) { throw "Échec de la mesure à $hz Hz." }
    }
}
finally {
    [Environment]::SetEnvironmentVariable('MASALLE_HZ', $ancienHz, 'Process')
    [Environment]::SetEnvironmentVariable('MASALLE_EXIT', $ancienExit, 'Process')
}
```

**Résultat produit :** `exo2-1_instant_predit_et_sa_periode/instants.txt`.
Exemple déjà mesuré : moyenne « réel − prédit » de **+3,219 ms à 72 Hz** et
**+0,112 ms à 60 Hz**. Les résultats peuvent varier selon l’écran et
l’ordonnanceur.

## Exercice 3 — Mesurer une vitesse

**Répertoire de commande :** racine `chapitre-07`.

Le premier lancement commande un lacet simulé de 30 degrés par seconde et
remplace `vitesses.txt`. Le second, sans `MASALLE_TOUR`, mesure les deltas de
souris réels et ajoute une série au fichier. Pendant le second lancement,
déplacez la souris dans la fenêtre de la salle; cette mesure ne peut pas être
faite par la commande seule.

```powershell
$chap7 = 'C:\Users\Dell\Desktop\AIA4\semestre1\Coder_pour_la_VR_l_XR_et_AR1\ani-4087\chapitre-07'
Set-Location $chap7
$resultat = Join-Path $chap7 'exo3-mesurer_une_vitesse\vitesses.txt'
if (Test-Path -LiteralPath $resultat) {
    Copy-Item -LiteralPath $resultat -Destination "$resultat.$([guid]::NewGuid()).bak"
}

jenga build --jenga-file Chapitre7.jenga --config Release --target exo3_vitesse --toolchain msys2-clang64
if ($LASTEXITCODE -ne 0) { throw 'Échec de compilation de exo3_vitesse.' }

$ancienTour = [Environment]::GetEnvironmentVariable('MASALLE_TOUR', 'Process')
try {
    $env:MASALLE_TOUR = '30'
    jenga run exo3_vitesse --config Release --toolchain msys2-clang64 --jenga-file Chapitre7.jenga
    if ($LASTEXITCODE -ne 0) { throw 'Échec de la série programmée.' }

    [Environment]::SetEnvironmentVariable('MASALLE_TOUR', $null, 'Process')
    Write-Host "Move the mouse in the window until all 20 samples finish."
    jenga run exo3_vitesse --config Release --toolchain msys2-clang64 --jenga-file Chapitre7.jenga
    if ($LASTEXITCODE -ne 0) { throw "Mouse series failed; confirm the window stayed open." }
}
finally {
    [Environment]::SetEnvironmentVariable('MASALLE_TOUR', $ancienTour, 'Process')
}
```

**Résultat produit :** `exo3-mesurer_une_vitesse/vitesses.txt`. Exemple
confirmé pour la série programmée : 20 images, vitesse Y moyenne **30,000°/s**
(min. **29,998**, max. **30,012**). Les valeurs de la série souris sont à
mesurer sur votre poste; ne les déduisez pas de la série programmée.

## Exercice 4 — Le chemin long provoque

**Répertoire de commande :** racine `chapitre-07`.

Le programme fait tourner la salle à 20°/s pendant un tour complet. Il compare
le calcul sans correction de chemin court au calcul corrigé. Aucun mouvement
de souris n’est requis. `chemin.txt` est tronqué à la fin du lancement.

```powershell
$chap7 = 'C:\Users\Dell\Desktop\AIA4\semestre1\Coder_pour_la_VR_l_XR_et_AR1\ani-4087\chapitre-07'
Set-Location $chap7
$resultat = Join-Path $chap7 'exo4-le_chemin_long_provoque\chemin.txt'
if (Test-Path -LiteralPath $resultat) {
    Copy-Item -LiteralPath $resultat -Destination "$resultat.$([guid]::NewGuid()).bak"
}

jenga build --jenga-file Chapitre7.jenga --config Release --target exo4_chemin --toolchain msys2-clang64
if ($LASTEXITCODE -ne 0) { throw 'Échec de compilation de exo4_chemin.' }
jenga run exo4_chemin --config Release --toolchain msys2-clang64 --jenga-file Chapitre7.jenga
if ($LASTEXITCODE -ne 0) { throw 'Échec de la cible exo4_chemin.' }
```

**Résultat produit :** `exo4-le_chemin_long_provoque/chemin.txt`. Exemple
confirmé dans la série existante : au voisinage du passage à 180°, la vitesse
fautive est d’environ **−20°/s**, tandis que la correction donne environ
**+20°/s**.

## Exercice 5 — Prédire et comparer

**Répertoire de commande :** racine `chapitre-07`.

Le programme doit observer 1 000 intervalles et au moins un delta de souris
réel. Réglez `MASALLE_HZ` sur la fréquence de l’écran utilisée pour votre essai
(72 ci-dessous est la valeur par défaut du programme, pas une vérification de
la fréquence physique). Pendant l’exécution, déplacez régulièrement la souris
dans la fenêtre. Sans mouvement, le programme termine avec une erreur et
n’écrit pas le résultat.

```powershell
$chap7 = 'C:\Users\Dell\Desktop\AIA4\semestre1\Coder_pour_la_VR_l_XR_et_AR1\ani-4087\chapitre-07'
Set-Location $chap7
$resultat = Join-Path $chap7 'exo5-predire_et_comparer\erreurs.txt'
if (Test-Path -LiteralPath $resultat) {
    Copy-Item -LiteralPath $resultat -Destination "$resultat.$([guid]::NewGuid()).bak"
}

jenga build --jenga-file Chapitre7.jenga --config Release --target exo5_prediction --toolchain msys2-clang64
if ($LASTEXITCODE -ne 0) { throw 'Échec de compilation de exo5_prediction.' }

$ancienHz = [Environment]::GetEnvironmentVariable('MASALLE_HZ', 'Process')
try {
    $env:MASALLE_HZ = '72'
    Write-Host 'Déplacez la souris dans la fenêtre pendant les 1 000 comparaisons.'
    jenga run exo5_prediction --config Release --toolchain msys2-clang64 --jenga-file Chapitre7.jenga
    if ($LASTEXITCODE -ne 0) { throw 'Échec ou mesure interrompue; vérifiez le mouvement souris et relancez.' }
}
finally {
    [Environment]::SetEnvironmentVariable('MASALLE_HZ', $ancienHz, 'Process')
}
```

**Résultat produit :** `exo5-predire_et_comparer/erreurs.txt`, avec moyenne,
maximum et nombre d’images pour les comparaisons avec et sans prédiction.
Aucun résultat de souris n’est fourni comme exemple, car cette série reste à
mesurer.

## Exercice 6 — Ce que la prédiction vous apporte

**Répertoire de commande :** racine `chapitre-07`.

C’est une simulation déterministe à 72 Hz (gestes rapide, lent et demi-tour);
elle n’ouvre pas de fenêtre et ne nécessite ni souris ni casque. Le programme
remplace `gestes.txt`.

```powershell
$chap7 = 'C:\Users\Dell\Desktop\AIA4\semestre1\Coder_pour_la_VR_l_XR_et_AR1\ani-4087\chapitre-07'
Set-Location $chap7
$resultat = Join-Path $chap7 'exo6-ce_que_la_prediction_vous_apporte\gestes.txt'
if (Test-Path -LiteralPath $resultat) {
    Copy-Item -LiteralPath $resultat -Destination "$resultat.$([guid]::NewGuid()).bak"
}

jenga build --jenga-file Chapitre7.jenga --config Release --target exo6_gestes --toolchain msys2-clang64
if ($LASTEXITCODE -ne 0) { throw 'Échec de compilation de exo6_gestes.' }
jenga run exo6_gestes --config Release --toolchain msys2-clang64 --jenga-file Chapitre7.jenga
if ($LASTEXITCODE -ne 0) { throw 'Échec de la simulation exo6_gestes.' }
```

**Résultat produit :** `exo6-ce_que_la_prediction_vous_apporte/gestes.txt`.
Exemple confirmé : pour le geste rapide, l’erreur moyenne simulée est
**0,330° avec prédiction** et **3,103° sans prédiction**.

## Exercice 7 — La latence injectée et la borne

**Répertoire de commande :** racine `chapitre-07`.

C’est une simulation à 72 Hz, pas une mesure de casque. L’exécutable génère
`latences.txt` (latences de 0, 10, 20, 40 et 80 ms, plus le cas à une seconde)
et ne génère pas lui-même l’image. La commande Python ci-dessous trace ensuite
les cinq points mesurés dans `latences.txt` avec Pillow 11.3.0, installé dans
l’environnement vérifié.

```powershell
$chap7 = 'C:\Users\Dell\Desktop\AIA4\semestre1\Coder_pour_la_VR_l_XR_et_AR1\ani-4087\chapitre-07'
Set-Location $chap7
$latences = Join-Path $chap7 'exo7-la_latence_injectee_et_la_borne\latences.txt'
$courbe = Join-Path $chap7 'exo7-la_latence_injectee_et_la_borne\courbe.png'
foreach ($resultat in @($latences, $courbe)) {
    if (Test-Path -LiteralPath $resultat) {
        Copy-Item -LiteralPath $resultat -Destination "$resultat.$([guid]::NewGuid()).bak"
    }
}

jenga build --jenga-file Chapitre7.jenga --config Release --target exo7_latence --toolchain msys2-clang64
if ($LASTEXITCODE -ne 0) { throw 'Échec de compilation de exo7_latence.' }
jenga run exo7_latence --config Release --toolchain msys2-clang64 --jenga-file Chapitre7.jenga
if ($LASTEXITCODE -ne 0) { throw 'Échec de la simulation exo7_latence.' }

@'
from pathlib import Path
import re
from PIL import Image, ImageDraw, ImageFont

folder = Path("exo7-la_latence_injectee_et_la_borne")
text = (folder / "latences.txt").read_text(encoding="utf-8")
matches = re.findall(
    r"^latence ([0-9.]+) ms : avec ([0-9.]+) deg .*?, sans ([0-9.]+) deg",
    text,
    re.MULTILINE,
)
if len(matches) != 5:
    raise SystemExit(f"5 points de latence attendus, {len(matches)} trouvés")
rows = [(float(x), float(with_prediction), float(without_prediction))
        for x, with_prediction, without_prediction in matches]

width, height = 900, 560
left, right, top, bottom = 105, 50, 58, 92
plot_width, plot_height = width - left - right, height - top - bottom
image = Image.new("RGB", (width, height), "white")
draw = ImageDraw.Draw(image)
font = ImageFont.truetype("arial.ttf", 17)
small = ImageFont.truetype("arial.ttf", 14)
title = ImageFont.truetype("arial.ttf", 22)
draw.text((left, 18), "Erreur moyenne par latence (72 Hz)", fill="#172033", font=title)

for tick in range(0, 11, 2):
    y = top + plot_height - tick / 10 * plot_height
    draw.line((left, y, width - right, y), fill="#d9dee8")
    draw.text((left - 35, y - 9), str(tick), fill="#333333", font=small)
draw.line((left, top, left, top + plot_height), fill="#303846", width=2)
draw.line((left, top + plot_height, width - right, top + plot_height),
          fill="#303846", width=2)

def point(latency, error):
    x = left + latency / 80 * plot_width
    y = top + plot_height - min(error, 10) / 10 * plot_height
    return x, y

for latency, _, _ in rows:
    x, _ = point(latency, 0)
    draw.line((x, top + plot_height, x, top + plot_height + 5), fill="#303846")
    draw.text((x - 12, top + plot_height + 10), f"{latency:g}",
              fill="#333333", font=small)

series = ((1, "#c43d3d", "Avec prediction"),
          (2, "#2267a8", "Sans prediction"))
for column, color, label in series:
    points = [point(row[0], row[column]) for row in rows]
    draw.line(points, fill=color, width=4)
    for x, y in points:
        draw.ellipse((x - 5, y - 5, x + 5, y + 5), fill=color, outline="white")

for index, (_, color, label) in enumerate(series):
    x, y = left + index * 260, height - 36
    draw.line((x, y + 8, x + 30, y + 8), fill=color, width=4)
    draw.ellipse((x + 11, y + 3, x + 21, y + 13), fill=color)
    draw.text((x + 38, y), label, fill="#222222", font=font)
draw.text((left + plot_width / 2 - 90, height - 61),
          "Latence injectee (ms)", fill="#222222", font=font)
axis = Image.new("RGBA", (210, 30), (255, 255, 255, 0))
ImageDraw.Draw(axis).text((0, 0), "Erreur moyenne (degres)",
                          fill="#222222", font=font)
axis = axis.rotate(90, expand=True)
image.paste(axis, (18, top + plot_height // 2 - axis.height // 2), axis)
image.save(folder / "courbe.png", format="PNG", optimize=True)
print(f"Courbe créée : {folder / 'courbe.png'}")
'@ | python -
if ($LASTEXITCODE -ne 0) { throw 'Échec de génération de courbe.png.' }
```

**Résultats produits :** `exo7-la_latence_injectee_et_la_borne/latences.txt`
et `exo7-la_latence_injectee_et_la_borne/courbe.png`. Exemple confirmé dans la
simulation existante : à 80 ms, erreurs moyennes **4,152° avec prédiction**
et **9,692° sans prédiction**. À une seconde sans borne, l’horizon mesuré est
**888,889 ms**; avec la borne, il est ramené à **100 ms**.

## Exercice 8 — La stabilité mesurée

**Répertoire de commande :** racine `chapitre-07`.

Chaque lancement comprend 30 intervalles de chauffe puis une mesure d’une
minute. Le premier utilise l’écran; le second demande `NK_VK_MAILBOX=1`. Le
second résultat est ajouté à `stabilite.txt`. Gardez la fenêtre active pendant
chaque essai; interrompre la fenêtre empêche de terminer la mesure.

```powershell
$chap7 = 'C:\Users\Dell\Desktop\AIA4\semestre1\Coder_pour_la_VR_l_XR_et_AR1\ani-4087\chapitre-07'
Set-Location $chap7
$resultat = Join-Path $chap7 'exo8-la_stabilite_mesuree\stabilite.txt'
if (Test-Path -LiteralPath $resultat) {
    Copy-Item -LiteralPath $resultat -Destination "$resultat.$([guid]::NewGuid()).bak"
}

jenga build --jenga-file Chapitre7.jenga --config Release --target exo8_stabilite --toolchain msys2-clang64
if ($LASTEXITCODE -ne 0) { throw 'Échec de compilation de exo8_stabilite.' }

$ancienMailbox = [Environment]::GetEnvironmentVariable('NK_VK_MAILBOX', 'Process')
try {
    [Environment]::SetEnvironmentVariable('NK_VK_MAILBOX', $null, 'Process')
    jenga run exo8_stabilite --config Release --toolchain msys2-clang64 --jenga-file Chapitre7.jenga
    if ($LASTEXITCODE -ne 0) { throw 'Échec de la mesure avec attente écran.' }

    $env:NK_VK_MAILBOX = '1'
    jenga run exo8_stabilite --config Release --toolchain msys2-clang64 --jenga-file Chapitre7.jenga
    if ($LASTEXITCODE -ne 0) { throw 'Échec de la mesure avec NK_VK_MAILBOX=1.' }
}
finally {
    [Environment]::SetEnvironmentVariable('NK_VK_MAILBOX', $ancienMailbox, 'Process')
}
```

**Résultat produit :** `exo8-la_stabilite_mesuree/stabilite.txt`, qui donne
le nombre d’intervalles, la moyenne, l’écart type, le maximum et le nombre
d’intervalles hors budget de 11,111 ms (90 Hz). Exemple déjà mesuré : 3 557
intervalles pour l’essai écran; le temps d’exécution d’un lancement est d’au
moins une minute. L’acceptation effective de MAILBOX n’est pas journalisée.

## Exercice 9 — Le verrou de demi-cadence

**Répertoire de commande :** racine `chapitre-07`.

`MASALLE_HZ` contrôle la période visée par le verrou et `MASALLE_COPIES` le
nombre initial de salles à calibrer. Les valeurs ci-dessous (`60` Hz et `2`
copies) correspondent aux paramètres des résultats consignés; réglez la
fréquence sur celle de votre écran si elle est différente. Le programme
chauffe, calibre la charge, puis mesure 1 000 intervalles sans verrou et
1 000 avec verrou. `verrou.txt` est tronqué à la fin.

```powershell
$chap7 = 'C:\Users\Dell\Desktop\AIA4\semestre1\Coder_pour_la_VR_l_XR_et_AR1\ani-4087\chapitre-07'
Set-Location $chap7
$resultat = Join-Path $chap7 'exo9-le_verrou_de_demi_cadence\verrou.txt'
if (Test-Path -LiteralPath $resultat) {
    Copy-Item -LiteralPath $resultat -Destination "$resultat.$([guid]::NewGuid()).bak"
}

jenga build --jenga-file Chapitre7.jenga --config Release --target exo9_verrou --toolchain msys2-clang64
if ($LASTEXITCODE -ne 0) { throw 'Échec de compilation de exo9_verrou.' }

$ancienHz = [Environment]::GetEnvironmentVariable('MASALLE_HZ', 'Process')
$anciennesCopies = [Environment]::GetEnvironmentVariable('MASALLE_COPIES', 'Process')
try {
    $env:MASALLE_HZ = '60'
    $env:MASALLE_COPIES = '2'
    jenga run exo9_verrou --config Release --toolchain msys2-clang64 --jenga-file Chapitre7.jenga
    if ($LASTEXITCODE -ne 0) { throw 'Échec de la mesure exo9_verrou.' }
}
finally {
    [Environment]::SetEnvironmentVariable('MASALLE_HZ', $ancienHz, 'Process')
    [Environment]::SetEnvironmentVariable('MASALLE_COPIES', $anciennesCopies, 'Process')
}
```

**Résultat produit :** `exo9-le_verrou_de_demi_cadence/verrou.txt`. Exemple
déjà mesuré avec ces paramètres : sans verrou, moyenne **29,155 ms**; avec
verrou, moyenne **36,608 ms**. Ces valeurs sont des mesures locales sur écran,
pas une validation du comportement ASW d’un casque.

## Exercice 10 — Un meilleur modèle

**Répertoire de commande :** racine `chapitre-07`.

C’est une simulation déterministe à 72 Hz qui compare la prédiction à vitesse
constante à celle avec accélération constante pour quatre gestes. Elle
n’ouvre pas de fenêtre et n’exige aucun matériel. Le programme remplace
`modeles.txt`.

```powershell
$chap7 = 'C:\Users\Dell\Desktop\AIA4\semestre1\Coder_pour_la_VR_l_XR_et_AR1\ani-4087\chapitre-07'
Set-Location $chap7
$resultat = Join-Path $chap7 'exo10-un_meilleur_modele\modeles.txt'
if (Test-Path -LiteralPath $resultat) {
    Copy-Item -LiteralPath $resultat -Destination "$resultat.$([guid]::NewGuid()).bak"
}

jenga build --jenga-file Chapitre7.jenga --config Release --target exo10_modeles --toolchain msys2-clang64
if ($LASTEXITCODE -ne 0) { throw 'Échec de compilation de exo10_modeles.' }
jenga run exo10_modeles --config Release --toolchain msys2-clang64 --jenga-file Chapitre7.jenga
if ($LASTEXITCODE -ne 0) { throw 'Échec de la simulation exo10_modeles.' }
```

**Résultat produit :** `exo10-un_meilleur_modele/modeles.txt`. Exemple
confirmé : pour le geste rapide, les erreurs moyennes consignées sont
**0,223°** pour le modèle à vitesse et **0,124°** pour le modèle à
accélération; les résultats sont simulés, pas mesurés sur casque.

## Vérifications et limites

- La commande de compilation individuelle utilise l’option Jenga
  `--target`; la commande d’exécution utilise le nom de projet comme argument
  positionnel. Ce sont les options affichées par `jenga build --help` et
  `jenga run --help` dans l’environnement installé.
- Le toolchain utilisé et vérifié pour ce workspace est
  `msys2-clang64`. L’auto-détection `clang-mingw` a déjà échoué avec le kit
  local; ne retirez pas l’option `--toolchain` sans vérifier votre
  installation.
- Les noms de cible Jenga et les chemins des exécutables viennent de
  `Chapitre7.jenga` et des binaires `Release-Windows` présents dans le
  workspace; ils ne sont pas déduits des noms de fichiers sources.
- Les exercices 3 et 5 nécessitent un mouvement réel de souris. L’exercice 9
  mesure le rendu sur l’écran de bureau. Aucun lancement ne fournit de mesure
  réelle de casque.
- Les résultats chiffrés donnés à titre d’exemple sont ceux déjà présents
  dans les fichiers de résultats du dépôt. Les exercices de rendu dépendent
  du poste, de l’écran, du pilote et des événements de souris; vos chiffres
  peuvent différer.
- Pour vérifier une cible sans relancer son programme, compilez-la avec la
  commande de son bloc. Pour vérifier manuellement l’existence d’un exécutable,
  par exemple :

```powershell
Test-Path 'C:\Users\Dell\Desktop\AIA4\semestre1\Coder_pour_la_VR_l_XR_et_AR1\ani-4087\chapitre-07\Build\Bin\Release-Windows\exo1_budget\exo1_budget.exe'
```
