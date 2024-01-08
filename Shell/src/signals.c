#include "../include/inc.h"

void handle_exit()
{

    printf("%s\nVoulez-vous arrêter le shell ? (O/N): %s", ANSI_COLOR_RED, ANSI_RESET);

    // Utiliser getchar pour éviter les avertissements liés à scanf
    int input = getchar();

    if (input == 'o' || input == 'O')
    {
        printf("%sExit shell.%s\n", ANSI_COLOR_RED, ANSI_RESET);
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("%sContinue l'exécution.%s\n", ANSI_COLOR_GREEN, ANSI_RESET);
        signal(SIGINT, handle_exit);
    }
}
