import time

durees = []

for i in range(1000):
    debut = time.perf_counter()

    # Opération effectuée à chaque image
    # Exemple : effacement/calcul
    x = 0
    for j in range(1000):
        x += j

    fin = time.perf_counter()

    duree_ms = (fin - debut) * 1000
    durees.append(duree_ms)

plus_longue = max(durees)
nb_depassements = sum(1 for d in durees if d > 11)

print(f"Plus longue image : {plus_longue:.3f} ms")
print(f"Images dépassant 11 ms : {nb_depassements}")

# import time

# durees_rendu = []

# for i in range(1000):
#     debut = time.perf_counter()

#     # RENDU SEUL
#     # Mettre ici uniquement les instructions de dessin/effacement
#     x = 0
#     for j in range(1000):
#         x += j

#     fin = time.perf_counter()

#     duree_ms = (fin - debut) * 1000
#     durees_rendu.append(duree_ms)

# temps_rendu = max(durees_rendu)

# print(f"Temps du rendu seul : {temps_rendu:.3f} ms")