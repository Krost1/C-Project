#ifndef SERVER_LIB_H
#define SERVER_LIB_H

/**
 * @brief Initialise un socket (UDP ou TCP).
 *
 * @param port  le port utilisé.
 * @param type  le type de la socket (UDP ou TCP) utilisé.
 * @param addr  un pointeur vers la structure struct sockaddr_in
 * @return le file descriptor.
 */
int initSocket(int port, enum __socket_type type);

/**
 * @brief Initialise un serveur UDP, si
 *  le client envoie la bonne réponse, alors
 *  ce dernier afficher les informations du client
 *  avant de lui envoyé un texte qui confir ca connexion.
 *
 * @param NULL Ne prend rien en paramètre.
 */
void *handle_UDPserver();

#endif // SERVER_LIB_H