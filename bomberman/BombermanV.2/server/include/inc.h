#ifndef INC_H
#define INC_H

/*
 *  Include
 */
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
 *  Server ports
 */
#define UDP_PORT 48069
#define TCP_PORT 12345

/*
 *  Max size of buffer
 */
#define BUFF_SIZE 1024
#define MAX_CLIENT 100

/*
 *  Mutex declaration
 */
extern pthread_mutex_t mutex;

/*
 *  Struct of Data of player
 */
typedef struct _player
{
    int posX, posY;
    int life;
    int speed;
    int nbClassicBomb;
    int nbMine;
    int nbRemoteBomb;
    int impactDist;
    bool invincible;
} *Player;

/*
 *  Struct of Data client
 */
typedef struct _client
{
    char pseudo[BUFF_SIZE];
    int socket;
    Player player;
} *Client;

/*
 *   Table of client
 */
extern Client tab_clients[MAX_CLIENT];

#endif // INC_H
