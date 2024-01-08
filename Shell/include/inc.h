#ifndef INC_H
#define INC_H

/*
 *  Include nécessaires pour le bon
 *  fonctionnement du code
 */
#include <pwd.h>
#include <grp.h>
#include <glob.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <string.h>
#include <fnmatch.h>
#include <termios.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

/*
 *   DEFINITION
 */

/*
 *  Set de couleurs
 */
#define ANSI_COLOR_RED "\033[0;31m"
#define ANSI_COLOR_GREEN "\033[0;32m"
#define ANSI_COLOR_YELLOW "\033[0;33m"
#define ANSI_COLOR_BLUE "\033[0;34m"
#define ANSI_COLOR_MAGENTA "\033[0;35m"
#define ANSI_COLOR_CYAN "\033[0;36m"
#define ANSI_COLOR_WHITE "\033[0;37m"
#define ANSI_BOLD "\033[1m"
#define ANSI_CHROMA "\033[7m"
#define ANSI_RESET "\033[0m"

/*
 *   Code ANSI pour clear le prompt
 */
#define ANSI_CLEAR_PROMPT "\033[H\033[J"

/*
 *   Taille maximal d'une chaine de caractère
 */
#define BUFFER_SIZE 1024

/*
 *   Nombre max je jobs
 */
#define NOMBRE_MAX_JOBS 10

/*
 *   STRUCTURE
 */

/*
 *   Structure pour faire corespondre un symbole
 *  (un opérateur) et une fonction
 */
typedef struct
{
    char *symbol;
    void (*function)(void *);
} CommandFunctionMap;

/*
 *   Enumeration qui sert a
 *   numérer les jobs en arrières plan
 */
typedef enum
{
    EN_COURS_EXECUTION,
    ARRETE
} EtatJob;
/*
 *   Structure d'un job
 */
typedef struct
{
    int numero_job;
    pid_t pid;
    EtatJob etat;
    char commande[BUFFER_SIZE];
} Job;

/*
 *  Structure pour représenter une variable
 */
typedef struct
{
    char *name;
    char *value;
} Variable;

/*
 *  Structure pour représenter l'environnement du shell
 */
typedef struct
{
    Variable *local_variables;
    size_t local_count;
    Variable *environment_variables;
    size_t environment_count;
} EnvironnementShell;

/*
 *   Structure de libération de mémoire
 */
typedef struct
{
    void *pointeur;
} garbage_collector;

#endif // INC_H