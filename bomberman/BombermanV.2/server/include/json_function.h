#ifndef JSON_FUNCTION_H
#define JSON_FUNCTION_H

/**
 * @brief Reads the contents of a JSON file and returns them as a string.
 *
 * This function opens the specified file for reading, determines its size, allocates
 * memory to store its content, reads the file, and returns the content as a string.
 *
 * @param filename  The path of the JSON file to be read.
 * @return A character string containing the content of the JSON file.
 */
char *read_json_file(const char *filename);

/**
 * @brief Modifies the ongoing game data with the provided name and mapId.
 *
 * This function reads the content of the ongoing game JSON file, parses it, adds a
 * new game entry with the given name and mapId, updates the game count, and writes
 * the modified data back to the file.
 *
 * @param name  The name of the game to be added.
 * @param mapId The map ID associated with the game.
 */
void modify_ongoing_game(cJSON *name, cJSON *mapId);

/**
 * @brief Creates a JSON response for the game creation action.
 *
 * This function creates a JSON response containing information about the newly
 * created game, such as action, status, message, number of players, map ID, start
 * position, and player details.
 *
 * @param client  The client initiating the game creation.
 * @param mapId   The map ID associated with the created game.
 * @return A character string containing the JSON response.
 */
char *create_game_response(cJSON *mapId);

/**
 * @brief Creates a JSON response for the game join action.
 *
 * This function creates a JSON response containing information about the joined
 * game, such as action, status, message, number of players, map ID, player details,
 * start position, and player details for all players in the game.
 *
 * @param game  The game structure representing the joined game.
 * @return A character string containing the JSON response.
 */
char *join_game_response(Game game);

#endif // JSON_FUNCTION_H
