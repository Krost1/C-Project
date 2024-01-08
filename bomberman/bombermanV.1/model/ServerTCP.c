
#include "../include/in.h"

#define MAX_CLIENTS 10
#define BUF_SIZE 1024

int nbr_client = 0;

struct Client
{
    char *pseudo;
    int socket;
    int ip;
    int port;
};

struct Client clients[MAX_CLIENTS];

void broadcast_message(char *message, int socket_client)
{
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
}

void *handle_client(void *socket_desc)
{
    int socket_client = *(int *)socket_desc;
    char buf[BUF_SIZE];

    int message_len;
    while ((message_len = read(socket_client, buf, BUF_SIZE - 1)) > 0)
    {
        buf[message_len] = '\0';
        broadcast_message(buf, socket_client);
    }

    // Suppression du client
    for (int i = 0; i < nbr_client; i++)
    {
        if (clients[i].socket == socket_client)
        {
            for (int j = i; j < nbr_client - 1; j++)
            {
                clients[j] = clients[j + 1];
            }
            nbr_client--;
            break;
        }
    }

    close(socket_client);
    return NULL;
}

void sigint_handler(int sig)
{

    printf("killing process %d\n", getpid());
    exit(0);
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

        if (nbr_client < MAX_CLIENTS)
        {
            clients[nbr_client].socket = socket_client;
            clients[nbr_client].ip = client_addr.sin_addr.s_addr;
            clients[nbr_client].port = ntohs(client_addr.sin_port);

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
            nbr_client++;
        }
        else
        {
            char *message = "Server is full";
            write(socket_client, message, strlen(message));
            close(socket_client);
        }
    }

    close(socket_server);
    return 0;
}
