#pragma once

#ifndef JSON_FUNCTION_H
#define JSON_FUNCTION_H

#include "../include/inc.h"
#include "../include/cJSON.h"
#include "../include/json_function.h"

#define MAX_GAMES 100
#define MAX_CLIENTS 100

#define BUF_SIZE 1024

struct play_
{
    int initPosX, initPosY;
    int life;
    int speed;
    int nbClassicBomb;
    int nbMine;
    int nbRemoteBomb;
    int impactDist;
    bool invincible;
};

struct Client
{
    char *pseudo;
    int socket;
    int ip;
    int port;
    struct play_ *player;
};

struct Game
{
    struct Client client[4];
    char name[BUF_SIZE];
    int nbrPlayer;
    int mapId;
};

struct Game **tab_games;
struct Client **tab_clients;

int nbr_client = 0;
int nbr_partie = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast_message(char *message, struct Game game, int socket_client);

char *readAndPrintJsonFromFile(char *filePath);

void modify_ongoing_game(cJSON *name, cJSON *mapId);

char *create_game_response(struct Client client, cJSON *mapId);

char *join_game_response(struct Game game);

#endif