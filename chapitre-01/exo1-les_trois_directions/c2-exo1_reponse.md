Etant donné qu'il y'a pas eu de précision sur le langage à utiliser, nous avons utilisé python :

Les trois fonctions définies pour représenter les directions sous forme de vecteurs unitaires :

def Avant():
    return (0, 0, -1)

def Haut():
    return (0, 1, 0)

def Droite():
    return (1, 0, 0)

# Lire les trois réels
x, y, z = map(float, input().split())

# Produit scalaire avec Avant
p_avant = x * Avant()[0] + y * Avant()[1] + z * Avant()[2]

# Produit scalaire avec Haut
p_haut = x * Haut()[0] + y * Haut()[1] + z * Haut()[2]

# Produit scalaire avec Droite
p_droite = x * Droite()[0] + y * Droite()[1] + z * Droite()[2]

# Affichage avec 4 décimales
print(f"{p_avant:.4f}")
print(f"{p_haut:.4f}")
print(f"{p_droite:.4f}")
