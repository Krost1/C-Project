#include "../include/inc.h"
#include "../include/signals.h"
#include "../include/strings.h"
#include "../include/back_fore_ground.h"
#include "../include/variable.h"

// int argc, char const *argv[]
int est_dans_ensemble(const char *chaine, const char *ensemble[], int taille_ensemble)
{
    for (int i = 0; i < taille_ensemble; ++i)
    {
        if (strcmp(chaine, ensemble[i]) == 0)
        {
            return 1; // La chaîne est dans l'ensemble
        }
    }
    return 0; // La chaîne n'est pas dans l'ensemble
}

void handle_input(char *buffer, char *envp[])
{
    const char *liste_mots[] = {"myjobs", "myfg", "mybg", "myls", "variable"};
    pid_t pid;
    char **token;

    if (est_dans_ensemble(buffer, liste_mots, sizeof(liste_mots) / sizeof(liste_mots[0])) == 0)
    {

        pid = fork();
        token = tokenize(buffer);
        if (pid < 0)
        {
            perror("Erreur fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0)
        {
            execvp(*token, token);
            freeTokens(token);
            exit(EXIT_FAILURE);
        }
        freeTokens(token);
    }
    else
    {
        if (strcmp(buffer, "variable") == 0)
        {
            env_var(envp);
        }
        back_fore_ground(buffer);
    }
}

int main(int argc, char const *argv[], char *envp[])
{

    (void)argc;
    (void)argv;

    char cwd[BUFFER_SIZE], hostname[256], *buff;

    printf("%s", ANSI_CLEAR_PROMPT);
    signal(SIGINT, handle_exit);
    for (;;)
    {
        if (gethostname(hostname, sizeof(hostname)) == 0)
        {
            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                printf("%s%s%s:%s%s%s$", ANSI_COLOR_CYAN, hostname, ANSI_RESET, ANSI_COLOR_MAGENTA, cwd, ANSI_RESET);
            }
        }

        buff = input();
        handle_input(buff, envp);
        free(buff);
    }

    return 0;
}