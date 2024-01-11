#include "../include/inc.h"

int initSocket(int port, enum __socket_type type)
{
    int sock_server;

    // Create socket
    if ((sock_server = socket(AF_INET, type, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(sock_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("Error setting socket options");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    // Bind the socket
    if (bind(sock_server, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) != 0)
    {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }

    return sock_server;
}

void *handle_UDPserver()
{
    int rcv, comp;
    struct sockaddr_in client_addr;

    // Initialize UDP socket
    int socket_serv = initSocket(UDP_PORT, SOCK_DGRAM);
    if (socket_serv < 0)
    {
        perror("Error assigning socket");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        char buff[BUFF_SIZE];
        socklen_t addrLen = sizeof(client_addr);

        // Receive data from clients
        if ((rcv = recvfrom(socket_serv, buff, sizeof(buff), 0, (struct sockaddr *)&client_addr, &addrLen)) < 0)
        {
            perror("Receive error");
            close(socket_serv);
            exit(EXIT_FAILURE);
        }
        buff[rcv - 1] = '\0';
        printf("%s\n", buff);

        // Compare received message
        comp = strcmp(buff, "looking for bomberstudent servers");
        if (comp == 0)
        {
            printf("Received response from client: %s\n", buff);

            // Send a hello message to the client
            if (sendto(socket_serv, "hello i’m a bomberstudent server", strlen("hello i’m a bomberstudent server"), 0, (struct sockaddr *)&client_addr, addrLen) < 0)
            {
                perror("Send error");
                close(socket_serv);
                exit(EXIT_FAILURE);
            }

            printf("Client connected\n");

            // Send the TCP port as a string to the client
            char port_str[6]; // Assuming the maximum length of the TCP port string is 5
            sprintf(port_str, "%d", TCP_PORT);
            if (sendto(socket_serv, port_str, strlen(port_str), 0, (struct sockaddr *)&client_addr, addrLen) < 0)
            {
                perror("Send error");
                close(socket_serv);
                exit(EXIT_FAILURE);
            }
        }
    }

    close(socket_serv);
    return NULL;
}
