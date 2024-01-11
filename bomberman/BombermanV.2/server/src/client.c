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
    // Copy client name and set socket and player data
    strcpy(client->pseudo, name);
    client->socket = socket_client;
    client->player = (struct _player *)malloc(sizeof(struct _player));

    // Check if memory allocation for player data is successful
    if (client->player == NULL)
    {
        perror("Memory Allocation Error");
        exit(EXIT_FAILURE);
    }

    // Initialize player data for the client
    initialize_player(client->player);

    // Store the client in the global array
    tab_clients[nbr_client] = client;
}

// Print the list of all connected clients
void print_all_clients(int nbr_client)
{
    // Lock the mutex to prevent concurrent access to shared data
    pthread_mutex_lock(&mutex);

    printf("List of connected clients:\n");

    for (int i = 0; i < nbr_client; i++)
    {
        printf("Client %d - Pseudo: %s\n", i + 1, tab_clients[i]->pseudo);
    }

    // Unlock the mutex after printing
    pthread_mutex_unlock(&mutex);
}

// Write a message to the client
void write_to_client(Client client, char buff[BUFF_SIZE])
{
    // Use write to send the message to the client's socket
    if (write(client->socket, buff, strlen(buff)) < 0)
    {
        perror("Error writing message");
    }
}
