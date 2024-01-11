#ifndef SERVER_LIB_H
#define SERVER_LIB_H

/**
 * @brief Initializes a socket (UDP or TCP).
 *
 * This function initializes and returns a socket with the specified port and type (UDP or TCP).
 *
 * @param port  The port to be used.
 * @param type  The type of the socket to be initialized (UDP or TCP).
 * @param addr  A pointer to the structure struct sockaddr_in.
 * @return The file descriptor of the initialized socket.
 */
int initSocket(int port, enum __socket_type type);

/**
 * @brief Initializes a UDP server, verifies client response, and sends confirmation on successful connection.
 *
 * This function initializes a UDP server. If the client sends the correct response, it displays client information
 * before sending a confirming text message to acknowledge the connection.
 *
 * @param NULL Takes no parameters.
 */
void *handle_UDPserver();

#endif // SERVER_LIB_H
