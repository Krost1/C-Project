#include "../lib/cJSON.h"
#include "../include/inc.h"
#include "../include/server_inc.h"
#include "../include/client_inc.h"
#include "../include/json_function.h"

int nbr_client = 0;
int nbr_partie = 0;

Client tab_clients[MAX_CLIENT];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 *   Libérationd de la mémoire avant de quitté le programme avec Ctrl+C
 */
void sigint_handler(int sig)
{
    (void)sig;
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < nbr_client; i++)
    {
        free(tab_clients[i]->player);
        free(tab_clients[i]);
    }
    pthread_mutex_unlock(&mutex);
    exit(EXIT_SUCCESS);
}

void handle_json(Client client, char buff[BUFF_SIZE])
{
    char *jsonString = NULL;
    printf("%s\n", buff);
    if (strlen(buff) < 14)
    {
        if (strcmp(buff, "GET maps/list") == 0)
        {
            jsonString = read_json_file("../json/maps_list.json");
            write_to_client(client, jsonString);
            free(jsonString);
        }
        if (strcmp(buff, "GET game/list") == 0)
        {
            jsonString = read_json_file("../json/ongoing_game.json");
            write_to_client(client, jsonString);
            free(jsonString);
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

                    cJSON_DeleteItemFromObject(request, "request");
                    char *json_str = cJSON_Print(request);

                    // handle_create_game(client, json_str);
                    cJSON_free(json_str);
                }

                if (strcmp(requete->valuestring, "POST game/join") == 0)
                {
                    cJSON_DeleteItemFromObject(request, "request");
                    char *json_str = cJSON_Print(request);

                    // handle_join_game(client, json_str);
                    cJSON_free(json_str);
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
}

void *handle_client(void *socket_desc)
{
    int socket_client = *(int *)socket_desc;
    int size_read;
    char name[BUFF_SIZE], buff[BUFF_SIZE];

    if ((size_read = read(socket_client, name, sizeof(name))) < 0)
    {
        perror("Erreur de lecture du pseudo");
        close(socket_client);
        return NULL;
    }

    name[size_read - 1] = '\0';

    pthread_mutex_lock(&mutex);
    Client client = (struct _client *)malloc(sizeof(struct _client));

    if (client == NULL)
    {
        perror("Allocation Mémoire");
        exit(EXIT_FAILURE);
    }

    create_client(client, name, socket_client, nbr_client);
    nbr_client++;
    pthread_mutex_unlock(&mutex);

    while ((size_read = read(socket_client, buff, sizeof(buff))) > 0)
    {
        buff[size_read - 1] = '\0';
        handle_json(client, buff);
    }

    // Free
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
    signal(SIGINT, sigint_handler);

    int socket_serv, socket_client;
    struct sockaddr_in client_addr;

    socket_serv = initSocket(TCP_PORT, SOCK_STREAM);

    if (listen(socket_serv, 0) == -1)
    {
        perror("Erreur listen");
        close(socket_serv);
        exit(EXIT_FAILURE);
    }

    pthread_t thread;
    if (pthread_create(&thread, NULL, handle_UDPserver, NULL) != 0)
    {
        perror("Erreur création de thread UDP");
        exit(EXIT_FAILURE);
    }
    else
    {
        pthread_detach(thread);
    }

    // atexit(sigint_handler);

    while (1)
    {
        socklen_t addr_size = sizeof(client_addr);

        // accepte les connexions
        if ((socket_client = accept(socket_serv, (struct sockaddr *)&client_addr, &addr_size)) < 0)
        {
            perror("Erreur accept");
            close(socket_serv);
            exit(EXIT_FAILURE);
        }

        int *p_socket = (int *)malloc(sizeof(int));
        if (p_socket == NULL)
        {
            perror("Allocation Mémoire");
            exit(EXIT_FAILURE);
        }
        *p_socket = socket_client;

        pthread_t client_thread;
        if ((pthread_create(&client_thread, NULL, handle_client, &socket_client)) != 0)
        {
            perror("Erreur création thread client");
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
