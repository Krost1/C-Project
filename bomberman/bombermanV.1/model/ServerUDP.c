#include "../include/in.h"

struct _client
{
    int num;
};

int main(int argc, char const *argv[])
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    char buf[1024];
    struct _client *cli = (struct _client *)malloc(sizeof(struct _client) * 100);

    if (sock == -1)
    {
        perror("Erreur création socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("Erreur bind");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("Erreur setsockopt");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        int len = sizeof(addr);
        if (recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&addr, &len) == -1)
        {
            perror("Erreur receive");
            exit(EXIT_FAILURE);
        }

        printf("Message reçu : %.*s\n", len, buf);
        printf("Message reçu de %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

        int clientExists = 0;

        for (int i = 0; i < 100; i++)
        {
            if (cli[i].num == ntohs(addr.sin_port))
            {
                clientExists = 1;
                break;
            }
        }

        if (!clientExists)
        {
            for (int i = 0; i < 100; i++)
            {
                if (cli[i].num == 0)
                {
                    cli[i].num = ntohs(addr.sin_port);
                    printf("Nouveau client ajouté : %d\n", cli[i].num);
                    break;
                }
            }
        }

        for (int i = 0; i < 100; i++)
        {
            if (cli[i].num != 0)
            {
                addr.sin_port = htons(cli[i].num);
                if (sendto(sock, buf, len, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0)
                {
                    perror("Erreur lors de l'envoi");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    free(cli);
    close(sock);
    return 0;
}
