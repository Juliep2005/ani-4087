Nous avons réalisé un programme permettant de suivre le déplacement de la souris avec un retard 
réglable entre 0 et 200 ms. Le programme a ensuite été testé par cinq personnes afin de déterminer, 
pour chacune, le retard à partir duquel elle commence à percevoir une différence entre le mouvement 
réel de la souris et le mouvement affiché à l'écran.

Les seuils obtenus sont les suivants :

| Personne   | Seuil de perception |
| ---------- | ------------------: |
| Cristal    |               30 ms |
| Jaurelle   |              180 ms |
| Ange       |               70 ms |
| Juliette   |               20 ms |
| Zidane     |               30 ms |

Les résultats montrent une forte variation entre les participants. Le seuil le plus faible observé est 
de 20 ms chez Juliette, tandis que le seuil le plus élevé est de 180 ms chez Jaurelle. Cela représente
une différence de 160 ms entre les deux extrêmes.

Ce budget correspond à l'ensemble de la chaîne allant de la détection du mouvement jusqu'à l'affichage 
de l'image. Il doit donc être partagé entre plusieurs étapes. En VR, le délai doit être particulièrement 
faible car l'image affichée doit suivre rapidement les mouvements de la tête. Un retard important peut 
créer un décalage entre les sensations corporelles et les informations visuelles et entraîner un inconfort.
