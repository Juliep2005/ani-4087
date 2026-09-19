Extrapolation d'une pose à vitesse constante :

Pour la position, l'extrapolation est réalisée à partir de la relation :  p′=p+vdt
où p représente la position initiale et v la vitesse linéaire.

Pour l'orientation, la vitesse angulaire ω permet de déterminer l'angle de rotation pendant dt :  θ=∥ω∥dt

L'axe de rotation est obtenu en normalisant le vecteur de vitesse angulaire. Un quaternion représentant cette rotation 
est ensuite construit puis composé avec le quaternion de l'orientation initiale afin d'obtenir l'orientation extrapolée.

Un traitement particulier est prévu lorsque la vitesse angulaire est nulle. Dans ce cas, aucune rotation n'est effectuée 
et l'orientation initiale est conservée. Cela permet notamment d'éviter une division par zéro lors de la normalisation de 
la vitesse angulaire.

Résultat : le programme lit la pose initiale, la vitesse linéaire, la vitesse angulaire et la durée dt, puis affiche la 
position et l'orientation extrapolées après dt secondes.

Pour une pose initiale donnée, une vitesse linéaire et une vitesse angulaire constantes, le programme calcule la nouvelle
position par déplacement linéaire et la nouvelle orientation par rotation autour de l'axe défini par la vitesse angulaire. 
Lorsque la vitesse angulaire est nulle, seule la position évolue tandis que l'orientation reste inchangée.
