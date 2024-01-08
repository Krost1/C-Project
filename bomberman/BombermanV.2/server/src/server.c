#include "../include/inc.h"

int initSocket(int port, enum __socket_type type)
{
    int socke_server;
    if ((socke_server = socket(AF_INET, type, 0)) == -1)
    {
        perror("Échec de la création de la socket");
        exit(EXIT_FAILURE);
    }

    // set socke
    int opt = 1;
    if (setsockopt(socke_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("Erreur setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    // Bind
    if (bind(socke_server, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) != 0)
    {
        perror("Erreur Bind ");
        exit(EXIT_FAILURE);
    }

    return socke_server;
}

void *handle_UDPserver()
{
    int rcv, comp;
    struct sockaddr_in server_addr;
    int socket_serv = initSocket(UDP_PORT, SOCK_DGRAM);
    if (socket_serv < 0)
    {
        perror("Erreur d'attribution de la socket");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        char buff[BUFF_SIZE];
        socklen_t addrLen = sizeof(server_addr);

        if ((rcv = recvfrom(socket_serv, buff, sizeof(buff), 0, (struct sockaddr *)&server_addr, &addrLen)) < 0)
        {
            perror("Erreur reception");
            close(socket_serv);
            exit(EXIT_FAILURE);
        }
        buff[rcv - 1] = '\0';
        printf("%s\n", buff);
        comp = strcmp(buff, "looking for bomberstudent servers");
        if (comp == 0)
        {
            printf("Réponse reçue du client : %s\n", buff);

            if (sendto(socket_serv, "hello i’m a bomberstudent server", strlen("hello i’m a bomberstudent server"), 0, (struct sockaddr *)&server_addr, addrLen) < 0)
            {
                perror("Erreur send");
                close(socket_serv);
                exit(EXIT_FAILURE);
            }
        }
    }

    close(socket_serv);
    return NULL;
}
