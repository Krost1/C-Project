#ifndef INC_H
#define INC_H

/*
 * Include
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
 * Server ports
 */
#define UDP_PORT 48069
#define TCP_PORT 12345

/*
 * Max size of buffer
 */
#define BUFF_SIZE 1024
#define MAX_CLIENT 100
#define MAX_GAME 25

/*
 * Mutex declaration
 */
extern pthread_mutex_t mutex;

/*
 * Struct representing player data
 */
typedef struct _player
{
    int posX, posY;    // Player position coordinates
    int life;          // Player life count
    int speed;         // Player movement speed
    int nbClassicBomb; // Number of regular bombs held by the player
    int nbMine;        // Number of mine bombs held by the player
    int nbRemoteBomb;  // Number of remote bombs held by the player
    int impactDist;    // Bomb impact distance
    bool invincible;   // Flag indicating player invincibility status
} *Player;

/*
 * Struct representing client data
 */
typedef struct _client
{
    char pseudo[BUFF_SIZE]; // Client's nickname
    int socket;             // Client's socket
    Player player;          // Player data associated with the client
} *Client;

/*
 * Struct representing game data
 */
typedef struct
{
    Client client[4];     // Array of clients in a game
    char name[BUFF_SIZE]; // Name of the game
    int nbrPlayer;        // Number of players in the game
    int mapId;            // ID of the game map
} *Game;

/*
 * Table of clients
 */
extern Client tab_clients[MAX_CLIENT]; // Array to store client data
extern Game tab_games[MAX_GAME];       // Array to store game data

#endif // INC_H
