import pygame
import time

# Initialisation de pygame
pygame.init()

# Création de la fenêtre
largeur = 1000
hauteur = 700
fenetre = pygame.display.set_mode((largeur, hauteur))
pygame.display.set_caption("Expérience - Retard de la souris")

# Couleurs
BLANC = (255, 255, 255)
ROUGE = (255, 0, 0)
NOIR = (0, 0, 0)

# Police
police = pygame.font.Font(None, 32)

# Retard initial en millisecondes
retard_ms = 0

# Position affichée du cercle
position_affichee = pygame.mouse.get_pos()

# Historique des positions de la souris
historique = []

# Horloge
horloge = pygame.time.Clock()

# Boucle principale
execution = True

while execution:

    # Gestion des événements
    for evenement in pygame.event.get():

        if evenement.type == pygame.QUIT:
            execution = False

        # Augmenter le retard avec la flèche droite
        elif evenement.type == pygame.KEYDOWN:

            if evenement.key == pygame.K_RIGHT:
                retard_ms = min(200, retard_ms + 10)

            # Diminuer le retard avec la flèche gauche
            elif evenement.key == pygame.K_LEFT:
                retard_ms = max(0, retard_ms - 10)

            # Remettre le retard à zéro
            elif evenement.key == pygame.K_r:
                retard_ms = 0

    # Position actuelle de la souris
    position_souris = pygame.mouse.get_pos()

    # Enregistrer la position et l'heure
    historique.append((time.perf_counter(), position_souris))

    # Temps actuel
    temps_actuel = time.perf_counter()

    # Temps recherché dans l'historique
    temps_recherche = temps_actuel - retard_ms / 1000

    # Chercher une ancienne position correspondant au retard
    ancienne_position = None

    for temps, position in reversed(historique):

        if temps <= temps_recherche:
            ancienne_position = position
            break

    # Si une ancienne position est trouvée
    if ancienne_position is not None:
        position_affichee = ancienne_position

    # Supprimer les anciennes données
    limite = temps_actuel - 1

    historique = [
        element for element in historique
        if element[0] >= limite
    ]

    # Effacer l'écran
    fenetre.fill(BLANC)

    # Dessiner le cercle retardé
    pygame.draw.circle(
        fenetre,
        ROUGE,
        position_affichee,
        20
    )

    # Afficher le retard
    texte = police.render(
        f"Retard : {retard_ms} ms",
        True,
        NOIR
    )

    fenetre.blit(texte, (20, 20))

    # Instructions
    instructions = police.render(
        "← diminuer | → augmenter | R : remettre à 0",
        True,
        NOIR
    )

    fenetre.blit(instructions, (20, 55))

    # Actualiser l'écran
    pygame.display.flip()

    # Limiter à 120 images/seconde
    horloge.tick(120)

# Fermer pygame
pygame.quit()