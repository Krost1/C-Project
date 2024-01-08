#include "../include/inc.h"
#include "../include/cJSON.h"
#define MAX_CLIENTS 3
#define BUF_SIZE 1024

int nbr_client = 0;

typedef struct play_
{
    int life;
    int speed;
    int nbClassucBomb;
    int nbMine;
    int nbRemoteBomb;
    int impactDist;
    bool invincible;
} *Player;

struct Client
{
    char *pseudo;
    int socket;
    int ip;
    int port;
    Player player;
};

struct Client clients[MAX_CLIENTS];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void sigint_handler(int sig);
char **tokenize(char *chain);
void broadcast_message(char *message, int socket_client);

char *tableauEnString(const Tab tableau)
{
    char *gridString = (char *)malloc(200 * sizeof(char)); // Assurez-vous d'allouer suffisamment de mémoire.
    if (gridString == NULL)
    {
        perror("Allocation mémoire pour gridString échouée");
        exit(EXIT_FAILURE);
    }

    int index = 0;

    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            if (tableau[i][j] == 0)
            {
                gridString[index] = '.';
            }
            else
            {
                gridString[index] = tableau[i][j];
            }
            index++;
        }
        gridString[index] = '\n';
        index++;
    }

    gridString[index] = '\0'; // Ajoutez le caractère de fin de chaîne
    return gridString;
}

void move(char *buff, int socket_client, int client_index)
{
    // Créez un tableau temporaire pour marquer la position actuelle de tous les clients
    Tab tempTab;

    // Initialisez le tableau temporaire
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            tempTab[i][j] = 0;
        }
    }

    // Mettez à jour la position du client dans le tableau temporaire
    if (strcmp(buff, "z") == 0)
    {
        clients[client_index].x = (clients[client_index].x + 1) % 20;
    }
    else if (strcmp(buff, "s") == 0)
    {
        clients[client_index].x = (clients[client_index].x - 1 + 20) % 20;
    }
    else if (strcmp(buff, "d") == 0)
    {
        clients[client_index].y = (clients[client_index].y + 1) % 20;
    }
    else if (strcmp(buff, "q") == 0)
    {
        clients[client_index].y = (clients[client_index].y - 1 + 20) % 20;
    }

    // Marquez la position actuelle de tous les clients sur le tableau temporaire
    for (int i = 0; i < nbr_client; ++i)
    {
        tempTab[clients[i].x][clients[i].y] = 'X';
    }

    // Convertissez le tableau temporaire en une chaîne JSON mise à jour
    char *updatedTableauJSON = tableauEnString(tempTab);

    // Diffusez la chaîne JSON mise à jour à tous les clients
    broadcast_message(updatedTableauJSON, socket_client);

    // Libérez la mémoire allouée pour la chaîne JSON
    free(updatedTableauJSON);
}

void *handle_client(void *socket_desc)
{
    int socket_client = *(int *)socket_desc;
    char buf[BUF_SIZE];

    // Lire le premier message comme le pseudo du client
    int pseudo_len = read(socket_client, buf, BUF_SIZE - 1);
    if (pseudo_len <= 0)
    {
        perror("Erreur de lecture du pseudo");
        close(socket_client);
        return NULL;
    }
    buf[pseudo_len] = '\0';

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    getpeername(socket_client, (struct sockaddr *)&client_addr, &len);

    pthread_mutex_lock(&mutex);
    clients[nbr_client].socket = socket_client;
    clients[nbr_client].ip = client_addr.sin_addr.s_addr;
    clients[nbr_client].port = ntohs(client_addr.sin_port);
    clients[nbr_client].pseudo = strdup(buf);
    clients[nbr_client].x = rand() % 20;
    clients[nbr_client].y = rand() % 20;
    nbr_client++;
    pthread_mutex_unlock(&mutex);

    int message_len;
    while ((message_len = read(socket_client, buf, BUF_SIZE - 1)) > 0)
    {
        buf[message_len] = '\0';
        move(buf, socket_client, nbr_client);
    }

    // Suppression du client
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < nbr_client; i++)
    {
        if (clients[i].socket == socket_client)
        {
            free(clients[i].pseudo); // Libérer la mémoire du pseudo
            for (int j = i; j < nbr_client - 1; j++)
            {
                clients[j] = clients[j + 1];
            }
            nbr_client--;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    close(socket_client);
    return NULL;
}

int main(int argc, char const *argv[])
{
    int socket_server = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_server == -1)
    {
        perror("Erreur création socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(socket_server, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("Erreur bind");
        close(socket_server);
        exit(EXIT_FAILURE);
    }

    // Set socket
    int opt = 1;
    if (setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("Erreur setsockopt");
        close(socket_server);
        exit(EXIT_FAILURE);
    }

    if (listen(socket_server, 10) < 0)
    {
        perror("Erreur d'ecoute");
        close(socket_server);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, sigint_handler);

    while (1)
    {
        int socket_client;
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);

        if ((socket_client = accept(socket_server, (struct sockaddr *)&client_addr, &len)) < 0)
        {
            perror("Erreur d'attente de connexion");
            continue;
        }

        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, (void *)&socket_client))
        {
            perror("Erreur création thread");
            close(socket_client);
            exit(EXIT_FAILURE);
        }
        else
        {
            pthread_detach(thread);
        }
    }

    close(socket_server);
    return 0;
}

int length(char *chain)
{
    int count = 0;
    while (chain[count] != '\0')
    {
        count++;
    }
    return count;
}

void sigint_handler(int sig)
{
    printf("Arrêt du serveur.\n");
    exit(0);
}

void broadcast_message(char *message, int socket_client)
{
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < nbr_client; i++)
    {
        if (clients[i].socket != socket_client)
        {
            if (write(clients[i].socket, message, strlen(message)) < 0)
            {
                perror("Erreur d'écriture message");
                continue;
            }
        }
    }
    pthread_mutex_unlock(&mutex);
}

char *readAndPrintJsonFromFile(char *filePath)
{
    FILE *filePointer = fopen(filePath, "r");
    if (filePointer == NULL)
    {
        printf("Error: Unable to open the file %s.\n", filePath);
        exit(EXIT_FAILURE);
    }

    char fileContents[1024];
    int fileLength = fread(fileContents, 1, sizeof(fileContents), filePointer);
    fclose(filePointer);

    cJSON *parsedJson = cJSON_Parse(fileContents);
    if (parsedJson == NULL)
    {
        const char *errorPointer = cJSON_GetErrorPtr();
        if (errorPointer != NULL)
        {
            printf("Error: %s\n", errorPointer);
        }
        cJSON_Delete(parsedJson);
        exit(EXIT_FAILURE);
    }

    char *jsonString = cJSON_Print(parsedJson);

    cJSON_Delete(parsedJson);
    return jsonString;
}

char **tokenize(char *chain)
{
    int i = 0, j = 0, start = 0, size = length(chain);

    char **token = (char **)malloc(sizeof(char *) * (size / 2 + 2));
    if (token == NULL)
    {
        perror("Allocation mémoire pour token échouée");
        exit(EXIT_FAILURE);
    }

    while (chain[i] != '\0')
    {
        if (chain[i] == ' ')
        {
            if (i != start)
            {
                int tokenLength = i - start;
                token[j] = (char *)malloc(sizeof(char) * (tokenLength + 1));
                if (token[j] == NULL)
                {
                    perror("Allocation mémoire pour token[j] échouée");
                    while (j > 0)
                    {
                        free(token[--j]);
                    }
                    free(token);
                    exit(EXIT_FAILURE);
                }

                for (int k = 0; k < tokenLength; k++)
                {
                    token[j][k] = chain[start + k];
                }
                token[j][tokenLength] = '\0';

                j++;
            }
            start = i + 1;
        }
        i++;
    }

    if (start != i)
    {
        int tokenLength = i - start;
        token[j] = (char *)malloc(sizeof(char) * (tokenLength + 1));
        if (token[j] == NULL)
        {
            perror("Allocation mémoire pour token[j] échouée");
            while (j > 0)
            {
                free(token[--j]);
            }
            free(token);
            exit(EXIT_FAILURE);
        }

        for (int k = 0; k < tokenLength; k++)
        {
            token[j][k] = chain[start + k];
        }
        token[j][tokenLength] = '\0';

        j++;
    }

    token[j] = NULL;
    return token;
}
