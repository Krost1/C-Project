#include "../include/inc.h"
#include "../include/cJSON.h"

int createAndWriteJsonToFile(const char *filePath)
{
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "name", "John Doe");
    cJSON_AddNumberToObject(json, "age", 30);
    cJSON_AddStringToObject(json, "email", "john.doe@example.com");

    char *json_str = cJSON_Print(json);

    FILE *fp = fopen(filePath, "w");
    if (fp == NULL)
    {
        printf("Error: Unable to open the file.\n");
        cJSON_free(json_str);
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    }

    printf("%s\n", json_str);
    fputs(json_str, fp);
    fclose(fp);

    cJSON_free(json_str);
    cJSON_Delete(json);
    return 0;
}

int readAndPrintJsonFromFile2(const char *filePath)
{
    FILE *fp = fopen(filePath, "r");
    if (fp == NULL)
    {
        printf("Error: Unable to open the file.\n");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    int len = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

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

    cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "name");
    if (cJSON_IsString(name) && (name->valuestring != NULL))
    {
        printf("Name: %s\n", name->valuestring);
    }

    cJSON_Delete(json);
    return 0;
}

int modifyAndWriteJsonToFile(const char *filePath)
{
    FILE *fp = fopen(filePath, "r");
    if (fp == NULL)
    {
        printf("Error: Unable to open the file.\n");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    int len = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

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

    cJSON_ReplaceItemInObjectCaseSensitive(json, "name", cJSON_CreateString("Jane Doe"));
    cJSON_AddNumberToObject(json, "age", 32);
    cJSON_AddStringToObject(json, "phone", "555-555-5555");

    char *json_str = cJSON_Print(json);

    fp = fopen(filePath, "w");
    if (fp == NULL)
    {
        printf("Error: Unable to open the file.\n");
        cJSON_free(json_str);
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    }

    printf("%s\n", json_str);
    fputs(json_str, fp);
    fclose(fp);

    cJSON_free(json_str);
    cJSON_Delete(json);
    return 0;
}

void testWriteJsonFile()
{
    cJSON *json;

    cJSON *request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "action", "game/join");
    cJSON_AddStringToObject(request, "statut", "201");
    cJSON_AddStringToObject(request, "message", "game joined");
    cJSON_AddNumberToObject(request, "nbPlayer", 2);

    cJSON *fields = cJSON_CreateArray();
    cJSON_AddItemToArray(fields, cJSON_CreateObject()); // Ajout d'un objet au tableau
    cJSON_AddItemToArray(fields, cJSON_CreateObject()); // Ajout d'un objet au tableau
    cJSON_AddItemToArray(fields, cJSON_CreateObject()); // Ajout d'un objet au tableau

    cJSON *play = cJSON_GetArrayItem(fields, 0);
    cJSON_AddStringToObject(play, "name", "player1");
    cJSON_AddStringToObject(play, "pos", "0,0");

    cJSON *play1 = cJSON_GetArrayItem(fields, 1);
    cJSON_AddStringToObject(play1, "name", "player2");
    cJSON_AddStringToObject(play1, "pos", "0,79");

    cJSON *play3 = cJSON_GetArrayItem(fields, 2);
    cJSON_AddStringToObject(play3, "name", "player3");
    cJSON_AddStringToObject(play3, "pos", "0,79");

    cJSON_AddItemToObject(request, "players", fields);

    cJSON_AddStringToObject(request, "startPos", "5,3");

    cJSON *playerJson = cJSON_CreateObject();

    cJSON_AddNumberToObject(playerJson, "life", 100);
    cJSON_AddNumberToObject(playerJson, "speed", 1);

    cJSON_AddItemToObject(request, "player", playerJson);

    char *json_str = cJSON_Print(request);
    printf("%s\n", json_str);
    FILE *fp = fopen("data.json", "w");
    if (fp == NULL)
    {
        printf("Error: Unable to open the file.\n");
        exit(EXIT_FAILURE);
    }

    fputs(json_str, fp);

    cJSON_Delete(request);
    cJSON_Delete(json);
}

void delet()
{

    FILE *fp = fopen("data.json", "r");
    if (fp == NULL)
    {
        printf("Error: Unable to open the file.\n");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    int len = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

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

    cJSON_DeleteItemFromObject(json, "players");

    char *json_str = cJSON_Print(json);

    fp = fopen("data.json", "w");
    if (fp == NULL)
    {
        printf("Error: Unable to open the file.\n");
        cJSON_free(json_str);
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    }

    printf("%s\n", json_str);
    fputs(json_str, fp);
    fclose(fp);

    cJSON_free(json_str);
    cJSON_Delete(json);
}

char *create_game_response()
{

    cJSON *response = cJSON_CreateObject();

    cJSON_AddStringToObject(response, "action", "game/create");
    cJSON_AddStringToObject(response, "status", "201");
    cJSON_AddStringToObject(response, "message", "game created");
    cJSON_AddNumberToObject(response, "nbPlayers", 0);
    cJSON_AddNumberToObject(response, "mapId", 0);
    cJSON_AddStringToObject(response, "startPos", "3,2");

    cJSON *player = cJSON_CreateObject();
    if (player == NULL)
    {
        fprintf(stderr, "Erreur lors de la création de l'objet joueur JSON.\n");
        cJSON_Delete(response);
        return NULL;
    }

    cJSON_AddNumberToObject(player, "life", 100);
    cJSON_AddNumberToObject(player, "speed", 2);
    cJSON_AddNumberToObject(player, "nbClassicBomb", 4);
    cJSON_AddNumberToObject(player, "nbMine", 4);
    cJSON_AddNumberToObject(player, "nbRemoteBomb", 4);
    cJSON_AddNumberToObject(player, "impactDist", 4);
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

int main()
{
    printf("%s", readAndPrintJsonFromFile("../JSON/MAPS_LIST.json"));

    return 0;
}
