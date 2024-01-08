import pygame

pygame.init()

# generer la fenetre du jeu
pygame.display.set_caption("Bomberman")
sreen = pygame.display.set_mode((1080,720))


# importer l'arrière plan 
background = pygame.image.load('assets/bacground.jps')

running = True

# bouble pour laisser la fenetre ouverte
while running:
    
    # aplliquer l'arriere plan dans notre jeu
    sreen.blit(background,(0,0))
    
    pygame.display.flip()
    
    # si le houeur ferme la fenetre
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            pygame.quit()