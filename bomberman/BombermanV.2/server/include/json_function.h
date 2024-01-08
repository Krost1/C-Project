#ifndef JSON_FUNCTION_H
#define JSON_FUNCTION_H

/**
 * @brief lit un fichier dont le chemin est passé en paramètre.
 *
 * @param filename  Le chemin du fichier.
 * @return une chaine de caratère (le contenue du fichier).
 */
char *read_json_file(const char *filename);
#endif // JSON_FUNCTION_H
