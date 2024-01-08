#ifndef STRINGS_H
#define STRINGS_H

/**
 *  @brief Fonction qui libère la mémoire d'un token passé en paramètre.
 *
 *  @param tokens token.
 */
void freeTokens(char **tokens);

/**
 *  @brief Fonction qui calcule la taille d'une chaîne de caractères.
 *
 *  @param chain chaîne de caractères.
 *  @return la taille de la chaîne passée en paramètre.
 */
int length(char *chain);

/**
 *  @brief Fonction qui calcule la taille d'un jeton.
 *
 *  @param tokk chaîne de caractères.
 *  @return la taille d'un jeton.
 */
int size_of_token(char **tokk);

/**
 *  @brief Fonction qui concatène deux chaînes de caractères à partir d'un index.
 *
 *  @param chain_one première chaîne de caractères.
 *  @param chain_two deuxième chaîne de caractères.
 *  @param n index.
 */
void concatane_index(char *chain_one, char *chain_two, int n);

/**
 *  @brief Fonction qui concatène deux chaînes de caractères.
 *
 *  @param chain_one première chaîne de caractères.
 *  @param chain_two deuxième chaîne de caractères.
 */
void concatane(char *chain_one, char *chain_two);

/**
 *  @brief Fonction qui permet une saisie caractère par caractère.
 *
 *  @return une chaîne de caractères.
 */
char *input();

/**
 *  @brief Fonction qui découpe une chaîne de caractères en jetons.
 *
 *  @param chain chaîne de caractères.
 *  @return un tableau de chaînes de caractères.
 */
char **tokenize(char *chain);

/**
 *  @brief Fonction qui recherche une chaîne de caractères dans un jeton.
 *
 *  @param tokk jeton.
 *  @param word chaîne de caractères.
 *  @return la taille du jeton.
 */
int research_in_token(char **token, char *word);

/**
 *  @brief Fonction qui compare deux chaînes de caractères.
 *
 *  @param chain_one première chaîne de caractères.
 *  @param chain_two deuxième chaîne de caractères.
 *  @return Retourne 0 si les deux chaînes sont égales, et -1 sinon.
 */
int compare(char *chain_one, char *chain_two);

#endif // STRINGS_H
