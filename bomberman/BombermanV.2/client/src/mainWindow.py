import pygame
import sys

pygame.init()

# Générer la fenêtre du jeu
pygame.display.set_caption("Bomberman")
screen = pygame.display.set_mode((1080, 720))

# Charger l'image de fond et la redimensionner pour qu'elle ait les mêmes dimensions que la fenêtre
background = pygame.image.load('../assets/blanc.png')
background = pygame.transform.scale(background, (1080, 720))

# Charger l'image à placer au centre
center_image = pygame.image.load('../assets/background.png')

# Obtenir les dimensions de l'écran
screen_width, screen_height = screen.get_size()

# Obtenir les dimensions de l'image à placer au centre
image_width, image_height = center_image.get_size()

# Calculer les coordonnées pour placer l'image au centre
x_center = (screen_width - image_width) // 2
y_center = (screen_height - image_height) // 2 - 150

running = True

# Boucle pour laisser la fenêtre ouverte
while running:
    # Appliquer l'arrière-plan dans notre jeu
    screen.blit(background, (0, 0))

    # Placer l'image au centre de la fenêtre
    screen.blit(center_image, (x_center, y_center))

    pygame.display.flip()

    # Si le joueur ferme la fenêtre
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            pygame.quit()
            sys.exit()  