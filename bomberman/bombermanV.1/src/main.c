#include "../include/inc.h"
#include "../include/cJSON.h"
#include "../include/str.h"
#include "../include/json_function.h"

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

int main()
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
