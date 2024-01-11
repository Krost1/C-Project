#ifndef CLIENT_INC_H
#define CLIENT_INC_H

/**
 * @brief Initializes the data structure for a player.
 *
 * This function takes a player structure as input and initializes its data.
 *
 * @param player  The structure representing the player.
 * @return NULL.
 */
void initialize_player(struct _player *player);

/**
 * @brief Creates and initializes the data structure for a client.
 *
 * This function creates a client with the specified name, socket, and position
 * in the array of clients.
 *
 * @param client  The structure representing the client.
 * @param name  The nickname of the client.
 * @param socket_client The socket associated with the client.
 * @param nbr_client The ID indicating the client's position in the client array.
 * @return NULL.
 */
void create_client(Client client, char name[BUFF_SIZE], int socket_client, int nbr_client);

/**
 * @brief Displays the list of players connected to the TCP server.
 *
 * This function prints information about the connected players using their index.
 *
 * @param nbr_client  The index of the client.
 * @return NULL.
 */
void print_all_clients(int nbr_client);

/**
 * @brief Sends a message to a specific client.
 *
 * This function sends a message to the specified client using its data structure.
 *
 * @param client  The structure representing the client.
 * @param buff The message to be sent.
 * @return NULL.
 */
void write_to_client(Client client, char buff[BUFF_SIZE]);

#endif // CLIENT_INC_H
