#include "../include/inc.h"
#include "../lib/cJSON.h"

char *read_json_file(const char *filename)
{
    FILE *fd = fopen(filename, "r");
    if (fd == NULL)
    {
        printf("Error: Unable to open the file.\n");
        return NULL;
    }

    fseek(fd, 0, SEEK_END);
    size_t file_size = ftell(fd);
    rewind(fd);

    char *buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL)
    {
        printf("Error: Memory allocation failed.\n");
        fclose(fd);
        return NULL;
    }

    size_t result = fread(buffer, 1, file_size, fd);
    if (result != file_size)
    {
        printf("Error: Reading file failed.\n");
        free(buffer);
        fclose(fd);
        return NULL;
    }

    fclose(fd);
    buffer[file_size] = '\0';

    return buffer;
}

void modify_ongoing_game(cJSON *name, cJSON *mapId)
{
    FILE *fd = fopen("../json/ongoing_game.json", "r");
    if (fd == NULL)
    {
        printf("Erreur: impossible d'ouvrire le fichier.\n");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFF_SIZE];

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

    fd = fopen("../json/ongoing_game.json", "w");
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

char *create_game_response(cJSON *mapId)
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

char *join_game_response(Game game)
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
    cJSON_AddNumberToObject(response, "nbPlayers", game->nbrPlayer);
    cJSON_AddNumberToObject(response, "mapId", game->mapId);

    cJSON *tab = cJSON_CreateArray();

    for (int i = 1; i <= game->nbrPlayer; i++)
    {
        cJSON *newGame = cJSON_CreateObject();
        cJSON_AddStringToObject(newGame, "name", game->client[i]->pseudo);
        cJSON_AddNumberToObject(newGame, "pos", game->client[i]->player->posX);
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
