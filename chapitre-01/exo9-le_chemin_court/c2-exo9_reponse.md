Pour deux orientations séparées de dt, la vitesse angulaire moyenne est obtenue en calculant le quaternion relatif, 
puis son angle de rotation divisé par dt. Le chemin court est imposé en changeant le signe du quaternion relatif lorsque 
sa composante scalaire est négative. Sans cette correction, deux quaternions représentant la même orientation, mais de signes
opposés, peuvent être interprétés comme une rotation presque complète. Dans notre exemple, on obtient environ 0 rad/s avec le
chemin court, contre 627,3 rad/s sans le forçage, ce qui montre le caractère aberrant du second résultat.
