#ifndef CLIENT_INC_H
#define CLIENT_INC_H

/**
 * @brief Initialise les données d'un joueur.
 *
 * @param player  la structure.
 * @return NUL.
 */
void initialize_player(struct _player *player);

/**
 * @brief Initialise les données d'un joueur.
 *
 * @param client  la structure.
 * @param name  le pseudo du client.
 * @param socket_client la socket du client
 * @param nbr_client l'id dans lequel il se trouve dans le tableau
 * @return NUL.
 */
void create_client(Client client, char name[BUFF_SIZE], int socket_client, int nbr_client);

/**
 * @brief affiche les joueur connecté au serveur TCP.
 *
 * @param nbr_client  l'indice du client.
 * @return NUL.
 */
void print_all_clients(int nbr_client);

/**
 * @brief Envoie un message au client.
 *
 * @param client  client.
 * @param buff le message
 * @return NUL.
 */
void write_to_client(Client client, char buff[BUFF_SIZE]);
#endif // CLIENT_INC_H
