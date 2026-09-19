L'expérience (Le saut du doigt) :

| Distance de l'objet |   Décalage observé |
| ------------------- | -----------------: |
| 30 cm               |             6 cm   |
| 1 m                 |             3 cm   |
| 3 m                 |       1 cm environ |

Les deux yeux sont séparés l'un de l'autre et ne regardent donc pas le doigt exactement depuis le même point de vue. Chaque œil reçoit ainsi une image légèrement différente du doigt. Cela crée un décalage entre les deux images. Nos mesures le montrent : le décalage est d'environ 6 cm à 30 cm, 3 cm à 1 m et 1 cm à 3 m. On constate donc que le décalage diminue lorsque la distance de l'objet augmente.

Puisque les deux yeux ne voient pas exactement la même image du monde réel, un casque VR doit fabriquer deux images différentes à chaque instant : une image destinée à l'œil gauche et une autre destinée à l'œil droit. Ces deux images reproduisent les deux points de vue différents des yeux afin de donner une perception de profondeur.

D'après le premier exercice, le temps disponible dépend de la fréquence d'affichage :

72 Hz → 13,9 ms par image
90 Hz → 11,1 ms par image
120 Hz → 8,3 ms par image

Mais les différentes étapes matérielles consomment déjà environ 8,0 ms. Il reste donc pour l'application :

à 72 Hz : 5,9 ms
à 90 Hz : 3,1 ms
à 120 Hz : 0,3 ms

Ainsi, le programme VR doit calculer les deux vues stéréoscopiques dans un temps très court. À 120 Hz, il ne reste pratiquement plus de temps pour le calcul de l'application.

