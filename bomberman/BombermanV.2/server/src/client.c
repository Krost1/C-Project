#include "../include/inc.h"

void initialize_player(struct _player *player)
{
    player->posX = 0;
    player->posY = 2;
    player->life = 100;
    player->speed = 75;
    player->nbClassicBomb = 2;
    player->nbMine = 0;
    player->nbRemoteBomb = 1;
    player->impactDist = 2;
    player->invincible = false;
}

void create_client(Client client, char name[BUFF_SIZE], int socket_client, int nbr_client)
{
    strcpy(client->pseudo, name);
    client->socket = socket_client;
    client->player = (struct _player *)malloc(sizeof(struct _player));

    if (client->player == NULL)
    {
        perror("Allocation Mémoire");
        exit(EXIT_FAILURE);
    }

    initialize_player(client->player);
    tab_clients[nbr_client] = client;
}

void print_all_clients(int nbr_client)
{
    pthread_mutex_lock(&mutex);

    printf("Liste des clients connectés :\n");

    for (int i = 0; i < nbr_client; i++)
    {
        printf("Client %d - Pseudo: %s\n", i + 1, tab_clients[i]->pseudo);
    }

    pthread_mutex_unlock(&mutex);
}

void write_to_client(Client client, char buff[BUFF_SIZE])
{
    if (write(client->socket, buff, strlen(buff)) < 0)
    {
        perror("Erreur d'écriture message");
    }
}
