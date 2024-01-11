#include "../lib/cJSON.h"
#include "../include/inc.h"
#include "../include/server_inc.h"
#include "../include/client_inc.h"
#include "../include/json_function.h"

// Global variables
int nbr_client = 0;
int nbr_partie = 0;

Client tab_clients[MAX_CLIENT];
Game tab_games[MAX_GAME];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Signal handler for Ctrl+C (SIGINT).
 *
 * This function is invoked when the program receives a Ctrl+C signal. It cleans up
 * memory for all connected clients before exiting the program.
 *
 * @param sig  The signal number (SIGINT in this case).
 */
void sigint_handler(int sig)
{
    (void)sig;
    pthread_mutex_lock(&mutex);

    // Free memory for each client
    for (int i = 0; i < nbr_client; i++)
    {
        free(tab_clients[i]->player);
        free(tab_clients[i]);
    }

    pthread_mutex_unlock(&mutex);
    exit(EXIT_SUCCESS);
}

/**
 * @brief Handles creating a game based on the client's request.
 *
 * This function parses the client's JSON request to create a game, allocates memory for
 * the game structure, and updates the ongoing game list. It also sends a response to the
 * client indicating whether the game creation was successful.
 *
 * @param client  The client initiating the game creation.
 * @param buff    The JSON request received from the client.
 */
void handle_create_game(Client client, char *buff)
{
    cJSON *request = cJSON_Parse(buff);
    cJSON *name = cJSON_GetObjectItemCaseSensitive(request, "name");
    cJSON *mapId = cJSON_GetObjectItemCaseSensitive(request, "mapId");

    char *response = NULL;

    if (cJSON_IsString(name) && cJSON_IsNumber(mapId))
    {
        Game game = malloc(sizeof(Game));
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
        response = create_game_response(mapId);
        if (response != NULL)
        {
            if (write(client->socket, response, strlen(response)) < 0)
            {
                perror("Error sending response");
                close(client->socket);
            }
            free(response);
        }

        nbr_partie++;
    }
    else
    {
        char *cant_create = readAndPrintJsonFromFile("../json/error_create_game.json");
        if (write(client->socket, cant_create, strlen(cant_create)) < 0)
        {
            perror("Error sending response");
            close(client->socket);
        }
        free(cant_create);
    }

    cJSON_Delete(request);
}

/**
 * @brief Handles joining a game based on the client's request.
 *
 * This function parses the client's JSON request to join a game, updates the ongoing
 * game list, and sends a response to the client indicating whether the join operation
 * was successful.
 *
 * @param client  The client requesting to join the game.
 * @param buff    The JSON request received from the client.
 */
void handle_join_game(Client client, char *buff)
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

                    response = join_game_response(tab_games[i]);

                    printf("%s", response);
                    if (write(client->socket, response, strlen(response)) < 0)
                    {
                        perror("Error sending response");
                        close(client->socket);
                    }

                    if (tab_games[i]->nbrPlayer == 2)
                    {
                        // handler_game(*tab_games[i], client->socket);
                    }
                }
                else
                {
                    char message[] = "The game is full. Unable to join.";
                    if (write(client->socket, message, strlen(message)) < 0)
                    {
                        perror("Error sending response");
                        close(client->socket);
                    }
                }
            }
        }
    }
    else
    {
        char *cant_join = readAndPrintJsonFromFile("ERROR_JOIN_GAME.json");
        if (write(client->socket, cant_join, strlen(cant_join)) < 0)
        {
            perror("Error sending response");
            close(client->socket);
        }
        free(cant_join);
    }

    cJSON_Delete(request);
}

/**
 * @brief Handles JSON requests received from a client.
 *
 * This function processes JSON requests from clients, such as GET requests for maps
 * or ongoing games, and POST requests for creating or joining a game.
 *
 * @param client  The client for which the JSON request is handled.
 * @param buff    The JSON request received from the client.
 */
void handle_json(Client client, char buff[BUFF_SIZE])
{
    char *jsonString = NULL;
    printf("%s\n", buff);

    if (strlen(buff) < 14)
    {
        // Handle simple GET requests
        if (strcmp(buff, "GET maps/list") == 0)
        {
            jsonString = read_json_file("../json/maps_list.json");
            write_to_client(client, jsonString);
            free(jsonString);
        }
        else if (strcmp(buff, "GET game/list") == 0)
        {
            jsonString = read_json_file("../json/ongoing_game.json");
            write_to_client(client, jsonString);
            free(jsonString);
        }
    }
    else
    {
        // Handle more complex POST requests with cJSON
        cJSON *request = cJSON_Parse(buff);

        if (request != NULL)
        {
            cJSON *requete = cJSON_GetObjectItemCaseSensitive(request, "request");

            if (cJSON_IsString(requete))
            {
                if (strcmp(requete->valuestring, "POST game/create") == 0)
                {
                    handle_create_game(client, cJSON_Print(request));
                }
                else if (strcmp(requete->valuestring, "POST game/join") == 0)
                {
                    handle_join_game(client, cJSON_Print(request));
                }

                cJSON_Delete(request);
            }
            else
            {
                perror("'request' key is not a string.");
            }
        }
        else
        {
            perror("JSON parsing error.");
        }
    }
}

/**
 * @brief Handles a client connection in a separate thread.
 *
 * This function is responsible for handling each connected client in a separate thread.
 * It reads the client's username, creates a client structure, and handles incoming JSON
 * messages from the client.
 *
 * @param socket_desc  The socket descriptor for the connected client.
 * @return NULL, as it is designed to be used as a thread function.
 */
void *handle_client(void *socket_desc)
{
    int socket_client = *(int *)socket_desc;
    int size_read;
    char name[BUFF_SIZE], buff[BUFF_SIZE];

    // Read the client's name
    if ((size_read = read(socket_client, name, sizeof(name))) < 0)
    {
        perror("Error reading username");
        close(socket_client);
        return NULL;
    }

    name[size_read - 1] = '\0';

    pthread_mutex_lock(&mutex);
    Client client = (struct _client *)malloc(sizeof(struct _client));

    if (client == NULL)
    {
        perror("Memory Allocation Error");
        exit(EXIT_FAILURE);
    }

    create_client(client, name, socket_client, nbr_client);
    nbr_client++;
    pthread_mutex_unlock(&mutex);

    // Read and handle JSON messages from the client
    while ((size_read = read(socket_client, buff, sizeof(buff))) > 0)
    {
        buff[size_read - 1] = '\0';
        handle_json(client, buff);
    }

    // Free memory for the client
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < nbr_client; i++)
    {
        free(tab_clients[i]->player);
        free(tab_clients[i]);
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(void)
{
    // Register Ctrl+C signal handler
    signal(SIGINT, sigint_handler);

    int socket_serv, socket_client;
    struct sockaddr_in client_addr;

    // Initialize TCP server socket
    socket_serv = initSocket(TCP_PORT, SOCK_STREAM);

    // Set up listening on the server socket
    if (listen(socket_serv, 0) == -1)
    {
        perror("Listen Error");
        close(socket_serv);
        exit(EXIT_FAILURE);
    }

    pthread_t thread;

    // Create a separate thread to handle UDP server operations
    if (pthread_create(&thread, NULL, handle_UDPserver, NULL) != 0)
    {
        perror("Error creating UDP server thread");
        exit(EXIT_FAILURE);
    }
    else
    {
        pthread_detach(thread);
    }

    while (1)
    {
        socklen_t addr_size = sizeof(client_addr);

        // Accept client connections
        if ((socket_client = accept(socket_serv, (struct sockaddr *)&client_addr, &addr_size)) < 0)
        {
            perror("Accept Error");
            close(socket_serv);
            exit(EXIT_FAILURE);
        }

        int *p_socket = (int *)malloc(sizeof(int));
        if (p_socket == NULL)
        {
            perror("Memory Allocation Error");
            exit(EXIT_FAILURE);
        }

        *p_socket = socket_client;

        pthread_t client_thread;

        // Create a separate thread for each connected client
        if ((pthread_create(&client_thread, NULL, handle_client, p_socket)) != 0)
        {
            perror("Error creating client thread");
            free(p_socket);
            close(socket_client);
            exit(EXIT_FAILURE);
        }
        else
        {
            pthread_detach(client_thread);
        }
    }

    close(socket_serv);

    return 0;
}
