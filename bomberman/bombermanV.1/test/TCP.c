#include "../include/inc.h"
#include "../include/cJSON.h"

#define MAX_GAMES 100
#define MAX_CLIENTS 100

#define BUF_SIZE 1024

struct play_
{
    int initPosX, initPosY;
    int life;
    int speed;
    int nbClassicBomb;
    int nbMine;
    int nbRemoteBomb;
    int impactDist;
    bool invincible;
};

struct Client
{
    char *pseudo;
    int socket;
    int ip;
    int port;
    struct play_ *player;
};

struct Game
{
    struct Client client[4];
    char name[BUF_SIZE];
    int nbrPlayer;
    int mapId;
};

struct Game **tab_games;
struct Client **tab_clients;

int nbr_client = 0;
int nbr_partie = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void sigint_handler(int sig);
char **tokenize(char *chain);
void broadcast_message(char *message, struct Game game, int socket_client);
char *readAndPrintJsonFromFile(char *filePath);
void printClientDetails(struct Client client);
void modify_ongoing_game(cJSON *name, cJSON *mapId);
char *create_game_response(struct Client client, cJSON *mapId);
char *join_game_response(struct Game game);

void handler_game(struct Game game, int sockent_client)
{

    for (int i = 0; i < game.nbrPlayer; i++)
    {
        broadcast_message("partie lancé ", game, sockent_client);
    }
}

void handle_create_game(struct Client client, char *buff)
{
    cJSON *request = cJSON_Parse(buff);
    cJSON *name = cJSON_GetObjectItemCaseSensitive(request, "name");
    cJSON *mapId = cJSON_GetObjectItemCaseSensitive(request, "mapId");

    char *response = NULL;

    if (cJSON_IsString(name) && cJSON_IsNumber(mapId))
    {
        struct Game *game = malloc(sizeof(struct Game));
        if (game == NULL)
        {
            perror("Memory allocation error");
            exit(EXIT_FAILURE);
        }

        game->client[0] = client;

        strncpy(game->name, name->valuestring, sizeof(game->name) - 1);
        game->name[sizeof(game->name) - 1] = '\0';

        game->nbrPlayer = 1;
        game->mapId = mapId->valueint;
        tab_games[nbr_partie] = game;

        modify_ongoing_game(name, mapId);
        response = create_game_response(client, mapId);
        if (response != NULL)
        {
            if (write(client.socket, response, strlen(response)) < 0)
            {
                perror("Erreur d'envoi de reponse");
                close(client.socket);
            }
            free(response);
        }

        nbr_partie++;
    }
    else
    {
        char *cant_create = readAndPrintJsonFromFile("ERROR_CREATE_GAME.json");
        if (write(client.socket, cant_create, strlen(cant_create)) < 0)
        {
            perror("Erreur d'envoi de reponse");
            close(client.socket);
        }
        free(cant_create);
    }

    cJSON_Delete(request);
}

void handle_join_game(struct Client client, char *buff)
{

    cJSON *request = cJSON_Parse(buff);
    cJSON *name = cJSON_GetObjectItemCaseSensitive(request, "name");
    char *response = NULL;

    if (cJSON_IsString(name))
    {
        for (int i = 0; i < nbr_partie; i++)
        {
            if (strcmp(tab_games[i]->name, name->valuestring) == 0)
            {
                if (tab_games[i]->nbrPlayer < 4)
                {
                    tab_games[i]->client[tab_games[i]->nbrPlayer] = client;
                    tab_games[i]->nbrPlayer++;

                    response = join_game_response(*tab_games[i]);

                    printf("%s", response);
                    if (write(client.socket, response, strlen(response)) < 0)
                    {
                        perror("Erreur d'envoi de réponse");
                        close(client.socket);
                    }

                    if (tab_games[i]->nbrPlayer == 2)
                    {
                        handler_game(*tab_games[i], client.socket);
                    }
                }
                else
                {
                    char message[] = "La partie est pleine. Impossible de rejoindre.";
                    if (write(client.socket, message, strlen(message)) < 0)
                    {
                        perror("Erreur d'envoi de réponse");
                        close(client.socket);
                    }
                }
            }
        }
    }
    else
    {
        char *cant_join = readAndPrintJsonFromFile("ERROR_JOIN_GAME.json");
        if (write(client.socket, cant_join, strlen(cant_join)) < 0)
        {
            perror("Erreur d'envoi de réponse");
            close(client.socket);
        }
        free(cant_join);
    }

    cJSON_Delete(request);
}

void handle_json(struct Client client, char *buff)
{
    char *jsonString = NULL;
    char **token = tokenize(buff);
    if (token == NULL)
    {
        perror("Erreur de tokenization");
        exit(EXIT_FAILURE);
    }

    if (strcmp(token[0], "GET") == 0)
    {
        if (strcmp(token[1], "maps/list") == 0)
        {
            jsonString = readAndPrintJsonFromFile("../JSON/MAPS_LIST.json");

            if (jsonString != NULL)
            {
                ssize_t write_result = write(client.socket, jsonString, strlen(jsonString));
                if (write_result < 0)
                {
                    perror("Erreur d'écriture message");
                }
                free(jsonString);
            }
        }
        else if (strcmp(token[1], "game/list") == 0)
        {
            jsonString = readAndPrintJsonFromFile("../JSON/ONGOING_GAME.json");
            if (jsonString != NULL)
            {
                ssize_t write_result = write(client.socket, jsonString, strlen(jsonString));
                if (write_result < 0)
                {
                    perror("Erreur d'écriture message");
                }
                free(jsonString);
            }
        }
        else
        {
            perror("Endpoint non reconnu pour la méthode GET.");
        }
    }
    else
    {
        cJSON *request = cJSON_Parse(buff);
        if (request != NULL)
        {
            cJSON *requete = cJSON_GetObjectItemCaseSensitive(request, "request");
            if (cJSON_IsString(requete))
            {
                if (strcmp(requete->valuestring, "POST game/create") == 0)
                {
                    if (nbr_partie < 3)
                    {
                        cJSON_DeleteItemFromObject(request, "request");
                        char *json_str = cJSON_Print(request);

                        handle_create_game(client, json_str);
                        cJSON_free(json_str);
                    }
                    else
                    {
                        char message[] = "Le nombre de parties en cours est atteint";

                        ssize_t write_result = write(client.socket, message, strlen(message));
                        if (write_result < 0)
                        {
                            perror("Erreur d'écriture message");
                        }
                    }
                }
                else if (strcmp(requete->valuestring, "POST game/join") == 0)
                {
                    cJSON_DeleteItemFromObject(request, "request");
                    char *json_str = cJSON_Print(request);

                    handle_join_game(client, json_str);
                    cJSON_free(json_str);
                }
                else
                {
                    perror("Endpoint non reconnu pour la méthode POST.");
                }

                cJSON_Delete(request);
            }
            else
            {
                perror("La clé 'request' n'est pas une chaîne.");
            }
        }
        else
        {
            perror("Erreur de parsing JSON.");
        }
    }

    if (token)
    {
        for (int k = 0; token[k]; k++)
        {
            free(token[k]);
        }
        free(token);
    }
}

void *handle_client(void *socket_desc)
{
    int socket_client = *(int *)socket_desc;
    char buf[BUF_SIZE];

    if (read(socket_client, buf, sizeof(buf)) < 0)
    {
        perror("Erreur de lecture du pseudo");
        close(socket_client);
    }
    buf[strlen(buf)] = '\0';

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    getpeername(socket_client, (struct sockaddr *)&client_addr, &len);

    pthread_mutex_lock(&mutex);
    struct Client *client = (struct Client *)malloc(sizeof(struct Client));
    client->socket = socket_client;
    client->ip = client_addr.sin_addr.s_addr;
    client->port = ntohs(client_addr.sin_port);
    client->pseudo = strdup(buf);
    client->player = (struct play_ *)malloc(sizeof(struct play_));
    if (client->player == NULL)
    {
        perror("Allocation Mémoire");
        exit(EXIT_FAILURE);
    }
    client->player->initPosX = 0;
    client->player->initPosY = 2;
    client->player->life = 100;
    client->player->speed = 75;
    client->player->nbClassicBomb = 2;
    client->player->nbMine = 0;
    client->player->nbRemoteBomb = 1;
    client->player->impactDist = 2;
    client->player->invincible = false;
    tab_clients[nbr_client] = client;
    nbr_client++;
    pthread_mutex_unlock(&mutex);

    while (read(socket_client, buf, sizeof(buf)) > 0)
    {
        buf[strlen(buf)] = '\0';
        printf("%s", buf);
        handle_json(*client, buf);
    }

    for (int i = 0; i < nbr_client; i++)
    {
        free(tab_clients[i]->pseudo);
        free(tab_clients[i]->player);
        free(tab_clients[i]);
    }

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
    tab_games = malloc(sizeof(struct Game *) * MAX_GAMES);
    if (tab_games == NULL)
    {
        perror("Erreur d'allocation de mémoire pour tab_games");
        exit(EXIT_FAILURE);
    }

    tab_clients = malloc(sizeof(struct Client *) * MAX_CLIENTS);
    if (tab_clients == NULL)
    {
        perror("Erreur d'allocation de mémoire pour tab_clients");
        free(tab_games);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(42069);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_server, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("Erreur bind");
        close(socket_server);
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("Erreur setsockopt");
        close(socket_server);
        exit(EXIT_FAILURE);
    }

    if (listen(socket_server, 10) == -1)
    {
        perror("Erreur listen");
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
            perror("Erreur accept");
            close(socket_server);
            exit(EXIT_FAILURE);
        }

        pthread_t thread;
        if ((pthread_create(&thread, NULL, handle_client, &socket_client)) != 0)
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
    for (int i = 0; i < nbr_partie; i++)
    {
        free(tab_games[i]);
    }
    free(tab_games);

    for (int i = 0; i < nbr_client; i++)
    {
        free(tab_clients[i]);
    }
    free(tab_clients);

    close(socket_server);

    return 0;
}

void sigint_handler(int sig)
{
    (void)sig;

    printf("Info Client :\n");
    for (int i = 0; i < nbr_client; i++)
    {
        printf("%s\n", tab_clients[i]->pseudo);
        close(tab_clients[i]->socket);
        free(tab_clients[i]->pseudo);
        free(tab_clients[i]->player);
        free(tab_clients[i]);
    }
    free(tab_clients);

    printf("Info Game :\n");
    for (int i = 0; i < nbr_partie; i++)
    {
        printf("%s\n", tab_games[i]->name);
        for (int j = 0; j < tab_games[i]->nbrPlayer; j++)
        {
            free(tab_games[i]->client[j].pseudo);
            free(tab_games[i]->client[j].player);
            free(&tab_games[i]->client[j]);
        }
        free(tab_games[i]->client);
        free(tab_games[i]);
    }
    free(tab_games);

    pthread_mutex_destroy(&mutex);
    exit(EXIT_SUCCESS);
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

void concatane_index(char *chain_one, char *chain_two, int n)
{
    for (int i = 0; i < n; i++)
    {
        chain_one[i] = chain_two[i];
    }
    chain_one[n] = '\0';
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
                concatane_index(token[j], chain + start, tokenLength);
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
        concatane_index(token[j], chain + start, tokenLength);
        j++;
    }

    token[j] = NULL;
    return token;
}

void broadcast_message(char *message, struct Game game, int socket_client)
{
    for (int i = 0; i < game.nbrPlayer; i++)
    {
        if (game.client[i].socket != socket_client)
        {
            if (write(game.client[i].socket, message, strlen(message)) < 0)
            {
                perror("Erreur d'écriture message");
                continue;
            }
        }
    }
}

char *readAndPrintJsonFromFile(char *filePath)
{
    FILE *fd = fopen(filePath, "r");
    if (fd == NULL)
    {
        fprintf(stderr, "Error: Unable to open the file %s.\n", filePath);
        return NULL;
    }

    fseek(fd, 0, SEEK_END);
    long fileLength = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    char *buffer = (char *)malloc(fileLength + 1);
    if (buffer == NULL)
    {
        fclose(fd);
        perror("Error: Unable to allocate memory");
        return NULL;
    }

    int bytesRead = fread(buffer, 1, fileLength, fd);
    fclose(fd);

    if (bytesRead != fileLength)
    {
        free(buffer);
        fprintf(stderr, "Error reading from the file %s.\n", filePath);
        return NULL;
    }

    buffer[fileLength] = '\0';

    cJSON *parsedJson = cJSON_Parse(buffer);
    free(buffer);

    if (parsedJson == NULL)
    {
        const char *errorPointer = cJSON_GetErrorPtr();
        if (errorPointer != NULL)
        {
            fprintf(stderr, "Error: %s\n", errorPointer);
        }
        cJSON_Delete(parsedJson);
        return NULL;
    }

    char *jsonString = cJSON_Print(parsedJson);

    cJSON_Delete(parsedJson);
    return jsonString;
}

void modify_ongoing_game(cJSON *name, cJSON *mapId)
{
    FILE *fd = fopen("../JSON/ONGOING_GAME.json", "r");
    if (fd == NULL)
    {
        printf("Erreur: impossible d'ouvrire le fichier.\n");
        exit(EXIT_FAILURE);
    }

    char buffer[BUF_SIZE];
    int len = fread(buffer, 1, sizeof(buffer), fd);
    fclose(fd);

    cJSON *json = cJSON_Parse(buffer);
    if (json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            printf("Error: %s\n", error_ptr);
        }
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    }

    cJSON *tabGames = cJSON_GetObjectItem(json, "games");
    if (!cJSON_IsArray(tabGames))
    {
        cJSON_AddItemToObject(json, "games", cJSON_CreateArray());
        tabGames = cJSON_GetObjectItem(json, "games");
    }

    cJSON *newGame = cJSON_CreateObject();
    cJSON_AddStringToObject(newGame, "name", name->valuestring);
    cJSON_AddNumberToObject(newGame, "nbPlayer", 1);
    cJSON_AddNumberToObject(newGame, "mapId", mapId->valueint);

    cJSON_AddItemToArray(tabGames, newGame);

    cJSON *nbGamesList = cJSON_GetObjectItemCaseSensitive(json, "nbGamesList");
    if (cJSON_IsNumber(nbGamesList))
    {
        nbGamesList->valueint++;
    }

    char *test = cJSON_Print(json);

    fd = fopen("../JSON/ONGOING_GAME.json", "w");
    if (fd == NULL)
    {
        printf("Error: Unable to open the file for writing.\n");
        exit(EXIT_FAILURE);
    }

    char *json_str = cJSON_Print(json);

    if (fputs(json_str, fd) == EOF)
    {
        perror("Error writing to file");
        exit(EXIT_FAILURE);
    }

    fclose(fd);

    cJSON_Delete(json);
}

char *create_game_response(struct Client client, cJSON *mapId)
{
    if (cJSON_IsNumber(mapId))
    {
        cJSON *response = cJSON_CreateObject();

        if (response == NULL)
        {
            fprintf(stderr, "Erreur lors de la création de l'objet JSON.\n");
            return NULL;
        }

        cJSON_AddStringToObject(response, "action", "game/create");
        cJSON_AddStringToObject(response, "status", "201");
        cJSON_AddStringToObject(response, "message", "game created");
        cJSON_AddNumberToObject(response, "nbPlayers", 1);
        cJSON_AddNumberToObject(response, "mapId", mapId->valueint);
        cJSON_AddStringToObject(response, "startPos", "3,2");

        cJSON *player = cJSON_CreateObject();
        if (player == NULL)
        {
            fprintf(stderr, "Erreur lors de la création de l'objet joueur JSON.\n");
            cJSON_Delete(response);
            return NULL;
        }

        cJSON_AddNumberToObject(player, "life", 100);
        cJSON_AddNumberToObject(player, "speed", 1);
        cJSON_AddNumberToObject(player, "nbClassicBomb", 2);
        cJSON_AddNumberToObject(player, "nbMine", 0);
        cJSON_AddNumberToObject(player, "nbRemoteBomb", 1);
        cJSON_AddNumberToObject(player, "impactDist", 2);
        cJSON_AddBoolToObject(player, "invincible", false);

        cJSON_AddItemToObject(response, "player", player);

        char *json_str = cJSON_Print(response);
        if (json_str == NULL)
        {
            fprintf(stderr, "Erreur lors de l'impression JSON.\n");
        }

        cJSON_Delete(response);
        return json_str;
    }

    return NULL;
}

char *join_game_response(struct Game game)
{
    cJSON *response = cJSON_CreateObject();

    if (response == NULL)
    {
        perror("Erreur lors de la création de l'objet JSON.\n");
        return NULL;
    }

    cJSON_AddStringToObject(response, "action", "game/join");
    cJSON_AddStringToObject(response, "status", "201");
    cJSON_AddStringToObject(response, "message", "game joined");
    cJSON_AddNumberToObject(response, "nbPlayers", game.nbrPlayer);
    cJSON_AddNumberToObject(response, "mapId", game.mapId);

    cJSON *tab = cJSON_CreateArray();

    for (int i = 1; i <= game.nbrPlayer; i++)
    {
        cJSON *newGame = cJSON_CreateObject();
        cJSON_AddStringToObject(newGame, "name", game.client[i].pseudo);
        cJSON_AddNumberToObject(newGame, "pos", game.client[i].player->initPosY);
        cJSON_AddItemToArray(tab, newGame);
    }

    cJSON_AddItemToObject(response, "players", tab);
    cJSON_AddStringToObject(response, "startPos", "3,2");

    cJSON *player = cJSON_CreateObject();
    if (player == NULL)
    {
        perror("Erreur lors de la création de l'objet joueur JSON.\n");
        cJSON_Delete(response);
        return NULL;
    }

    cJSON_AddNumberToObject(player, "life", 100);
    cJSON_AddNumberToObject(player, "speed", 1);
    cJSON_AddNumberToObject(player, "nbClassicBomb", 1);
    cJSON_AddNumberToObject(player, "nbMine", 0);
    cJSON_AddNumberToObject(player, "nbRemoteBomb", 0);
    cJSON_AddNumberToObject(player, "impactDist", 0);
    cJSON_AddBoolToObject(player, "invincible", false);

    cJSON_AddItemToObject(response, "player", player);

    char *json_str = cJSON_Print(response);
    if (json_str == NULL)
    {
        perror("Erreur lors de l'impression JSON.\n");
    }

    cJSON_Delete(response);
    return json_str;
}