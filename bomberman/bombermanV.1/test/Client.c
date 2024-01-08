#include "../include/inc.h"
#include "../include/cJSON.h"

// Fonction pour remplacer les espaces par des underscores dans une chaîne
void replaceSpacesWithUnderscores(char *str)
{
    while (*str)
    {
        if (*str == ' ')
            *str = '_';
        str++;
    }
}

int main(int argc, char const *argv[])
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1)
    {
        perror("Erreur création socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(42069);

    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0)
    {
        perror("Erreur conversion adresse IP");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Connexion au serveur
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Erreur connexion Client");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Saisie du pseudo
    char pseudo[1024];
    printf("Entrez votre pseudo : ");
    if (fgets(pseudo, sizeof(pseudo), stdin) == NULL)
    {
        perror("Erreur de lecture du pseudo");
        close(sock);
        exit(EXIT_FAILURE);
    }
    pseudo[strcspn(pseudo, "\n")] = '\0';
    int pseudo_len = strlen(pseudo);

    // Envoi du pseudo au serveur
    if (write(sock, pseudo, pseudo_len) < 0)
    {
        perror("Erreur d'écriture du pseudo");
        close(sock);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // Menu des commandes
        printf("Choisissez une commande:\n");
        printf("1. Créer une partie\n");
        printf("2. Autre commande\n");

        int choice;
        if (scanf("%d", &choice) != 1)
        {
            perror("Erreur de lecture de la commande");
            close(sock);
            exit(EXIT_FAILURE);
        }

        while (getchar() != '\n')
            ; // Nettoyer le tampon d'entrée

        switch (choice)
        {
        case 1:
        {
            // Création d'une partie
            char mesg[1024];
            strcpy(mesg, "POST game/create");
            mesg[strlen(mesg)] = '\0';

            char name[1024];
            int id;

            printf("Entrez le nom de la game : ");
            if (fgets(name, sizeof(name), stdin) == NULL || strlen(name) == 1)
            {
                perror("Erreur de lecture du nom de la partie");
                close(sock);
                exit(EXIT_FAILURE);
            }
            name[strcspn(name, "\n")] = '\0';
            replaceSpacesWithUnderscores(name);

            printf("Entrez l'id de la game : ");
            if (scanf("%d", &id) != 1 || id < 0)
            {
                perror("Erreur de lecture de l'id de la partie");
                close(sock);
                exit(EXIT_FAILURE);
            }

            // Création d'une requête JSON
            cJSON *request = cJSON_CreateObject();
            cJSON_AddStringToObject(request, "request", mesg);
            cJSON_AddStringToObject(request, "name", name);
            cJSON_AddNumberToObject(request, "mapId", id);
            char *json_str = cJSON_Print(request);

            // Envoi de la requête au serveur
            if (write(sock, json_str, strlen(json_str)) < 0)
            {
                perror("Erreur d'écriture message");
                close(sock);
                cJSON_Delete(request);
                free(json_str);
                exit(EXIT_FAILURE);
            }

            cJSON_Delete(request);
            free(json_str);

            break;
        }
        case 2:
        {
            // Rejoindre une partie
            char mesg[1024];
            strcpy(mesg, "POST game/join");
            mesg[strlen(mesg)] = '\0';

            char name[1024];

            printf("Entrez le nom de la game : ");
            if (fgets(name, sizeof(name), stdin) == NULL || strlen(name) == 1)
            {
                perror("Erreur de lecture du nom de la partie");
                close(sock);
                exit(EXIT_FAILURE);
            }
            name[strcspn(name, "\n")] = '\0';
            replaceSpacesWithUnderscores(name);

            // Création d'une requête JSON
            cJSON *request = cJSON_CreateObject();
            cJSON_AddStringToObject(request, "request", mesg);
            cJSON_AddStringToObject(request, "name", name);
            char *json_str = cJSON_Print(request);

            // Envoi de la requête au serveur
            if (write(sock, json_str, strlen(json_str)) < 0)
            {
                perror("Erreur d'écriture message");
                close(sock);
                cJSON_Delete(request);
                free(json_str);
                exit(EXIT_FAILURE);
            }

            cJSON_Delete(request);
            free(json_str);
            break;
        }
        default:
            printf("Commande non reconnue. Veuillez choisir une option valide.\n");
        }

        // Lecture de la réponse du serveur
        char lire[1024];
        int bytesRead = read(sock, lire, sizeof(lire) - 1);
        if (bytesRead < 0)
        {
            perror("Erreur lecture");
            close(sock);
            exit(EXIT_FAILURE);
        }

        lire[bytesRead] = '\0';
        printf("%s\n", lire);
    }

    // Fermeture de la socket
    close(sock);
    return 0;
}
