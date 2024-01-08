#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <glob.h>
#include <dirent.h>
#include <fnmatch.h>

#define length(x) (sizeof(x) / sizeof((x)[0]))

void freeTokens(char **tokens)
{
    if (tokens == NULL)
        return;

    int i = 0;
    while (tokens[i] != NULL)
    {
        free(tokens[i]);
        i++;
    }
    free(tokens);
}

char *input()
{
    int capacity = 10;
    char *buff = (char *)malloc(capacity * sizeof(char));
    if (!buff)
    {
        perror("Allocation failed");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    char ch;
    int scan_result;
    while (1)
    {
        scan_result = scanf("%c", &ch);

        if (scan_result == EOF || ch == '\n')
        {
            buff[i] = '\0';
            break;
        }
        else if (scan_result != 1)
        {
            free(buff);
            perror("Erreur de lecture");
            exit(EXIT_FAILURE);
        }

        if (i >= capacity - 1)
        {
            capacity *= 2;
            buff = realloc(buff, capacity * sizeof(char));
            if (!buff)
            {
                perror("Reallocation failed");
                exit(EXIT_FAILURE);
            }
        }

        buff[i++] = ch;
    }
    return buff;
}

char **tokenize(char *chain)
{
    int i = 0, j = 0, start = 0, size = strlen(chain);

    char **token = (char **)malloc(sizeof(char *) * (size / 2 + 2));
    if (token == NULL)
    {
        perror("Allocation mémoire pour token échouée");
        exit(EXIT_FAILURE);
    }

    while (chain[i] != '\0')
    {
        if (chain[i] == ' ')
        {
            if (i != start)
            {
                int tokenLength = i - start;
                token[j] = (char *)malloc(sizeof(char) * (tokenLength + 1));
                if (token[j] == NULL)
                {
                    perror("Allocation mémoire pour token[j] échouée");
                    while (j > 0)
                    {
                        free(token[--j]);
                    }
                    free(token);
                    exit(EXIT_FAILURE);
                }
                strncpy(token[j], chain + start, tokenLength);
                token[j][tokenLength] = '\0';
                j++;
            }
            start = i + 1;
        }
        i++;
    }

    if (start != i)
    {
        int tokenLength = i - start;
        token[j] = (char *)malloc(sizeof(char) * (tokenLength + 1));
        if (token[j] == NULL)
        {
            perror("Allocation mémoire pour token[j] échouée");
            while (j > 0)
            {
                free(token[--j]);
            }
            free(token);
            exit(EXIT_FAILURE);
        }
        strncpy(token[j], chain + start, tokenLength);
        token[j][tokenLength] = '\0';
        j++;
    }

    token[j] = NULL;
    return token;
}

void functionForSemicolon(void *param)
{
    char **token = (char **)param;
    pid_t pid;
    int wstatus;
    pid = fork();

    if (pid < 0)
    {
        perror("Erreur création du fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        if (execvp(token[0], token) < 0)
        {
            perror("Erreur d'exécution de la commande");
            exit(EXIT_FAILURE);
        }

        exit(EXIT_FAILURE);
    }
    else
    {
        wait(&wstatus);
        printf("Le fils s'est terminé avec le code de sortie : %d\n", WEXITSTATUS(wstatus));
    }
    freeTokens(token);
}

int functionForAndAnd(void *param)
{
    char **commands = (char **)param;
    int wstatus;
    pid_t child1, child2;

    child1 = fork();

    if (child1 < 0)
    {
        perror("Erreur fork de la commande 1");
        exit(EXIT_FAILURE);
    }

    if (child1 == 0)
    {
        if (execvp(*commands, commands) < 0)
        {
            perror("Erreur fork de l'exécution de la commande 1");
            exit(EXIT_FAILURE);
        }
    }

    waitpid(child1, &wstatus, 0);

    if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 0)
    {
        // Deuxième fils
        return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}

int functionForOrOr(void *param)
{
    char **commands = (char **)param;
    int wstatus;
    pid_t child1, child2;

    child1 = fork();

    if (child1 < 0)
    {
        perror("Erreur fork de la commande 1");
        exit(EXIT_FAILURE);
    }

    if (child1 == 0)
    {
        if (execvp(*commands, commands) < 0)
        {
            perror("Erreur fork de l'exécution de la commande 1");
            exit(EXIT_FAILURE);
        }
    }

    waitpid(child1, &wstatus, 0);

    if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != 0)
    {
        // Deuxième fils
        return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}

void functionPipe(void *param)
{
    char ***commands = (char ***)param;
    int pipefd[2];
    pid_t child1, child2;
    int status;

    if (pipe(pipefd) == -1)
    {
        perror("Erreur pipe");
        exit(EXIT_FAILURE);
    }

    if ((child1 = fork()) == 0)
    {
        // Premier fils
        close(pipefd[0]);               // Fermer l'extrémité de lecture du pipe
        dup2(pipefd[1], STDOUT_FILENO); // Rediriger la sortie standard vers l'extrémité d'écriture du pipe
        close(pipefd[1]);               // Fermer l'extrémité d'écriture du pipe

        if (execvp(commands[0][0], commands[0]) < 0)
        {
            perror("Erreur d'exécution de la commande 1");
            exit(EXIT_FAILURE);
        }
    }
    else if ((child2 = fork()) == 0)
    {
        // Deuxième fils
        close(pipefd[1]);              // Fermer l'extrémité d'écriture du pipe
        dup2(pipefd[0], STDIN_FILENO); // Rediriger l'entrée standard depuis l'extrémité de lecture du pipe
        close(pipefd[0]);              // Fermer l'extrémité de lecture du pipe

        if (execvp(commands[1][0], commands[1]) < 0)
        {
            perror("Erreur d'exécution de la commande 2");
            exit(EXIT_FAILURE);
        }
    }

    // Processus parent
    close(pipefd[0]); // Fermer les deux extrémités du pipe dans le processus parent
    close(pipefd[1]);

    waitpid(child1, &status, 0);
    waitpid(child2, &status, 0);

    freeTokens(commands[0]);
    freeTokens(commands[1]);

    printf("Action pour |\n");
}

void functionForAsterisk(void *param)
{
    char **pattern = (char **)param;

    glob_t glob_result;
    int ret = glob(*pattern, 0, NULL, &glob_result);

    if (ret == 0)
    {
        if (glob_result.gl_pathc > 0)
        {
            for (int i = 0; i < glob_result.gl_pathc; ++i)
            {
                printf("Fichier correspondant : %s\n", glob_result.gl_pathv[i]);
            }
        }
        else
        {
            printf("Aucun fichier correspondant trouvé.\n");
        }
    }
    else if (ret == GLOB_NOMATCH)
    {
        printf("Aucune correspondance trouvée pour le motif.\n");
    }
    else
    {
        perror("Erreur lors de la recherche de fichiers");
    }

    globfree(&glob_result);
}

void functionForQuestionMark(void *param)
{
    char **pattern = (char **)param;

    glob_t glob_result;
    int ret = glob(*pattern, 0, NULL, &glob_result);

    if (ret == 0)
    {
        if (glob_result.gl_pathc > 0)
        {
            for (int i = 0; i < glob_result.gl_pathc; ++i)
            {
                printf("Fichier correspondant : %s\n", glob_result.gl_pathv[i]);
            }
        }
        else
        {
            printf("Aucun fichier correspondant trouvé.\n");
        }
    }
    else if (ret == GLOB_NOMATCH)
    {
        printf("Aucune correspondance trouvée pour le motif.\n");
    }
    else
    {
        perror("Erreur lors de la recherche de fichiers");
    }

    globfree(&glob_result);
}

void functionForParentheses(const char *pattern)
{
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(".")) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (fnmatch(pattern, ent->d_name, FNM_NOESCAPE) == 0)
            {
                printf("%s\n", ent->d_name);
            }
        }
        closedir(dir);
    }
    else
    {
        perror("Erreur lors de l'ouverture du répertoire");
    }
}

void processPattern(const char *pattern)
{
    if (strchr(pattern, '*') || strchr(pattern, '[') || strchr(pattern, '~'))
    {
        void *param = (void *)&pattern;
        functionForAsterisk(param);
    }
    else
    {
        functionForParentheses(pattern);
    }
}

void executeCommand(char **commands)
{
    pid_t pid;
    int wstatus;

    if ((pid = fork()) == 0)
    {
        if (execvp(commands[0], commands) < 0)
        {
            perror("Erreur d'exécution de la commande");
            exit(EXIT_FAILURE);
        }
    }
    else if (pid > 0)
    {
        waitpid(pid, &wstatus, 0);
        printf("Le fils s'est terminé avec le code de sortie : %d\n", WEXITSTATUS(wstatus));
    }
    else
    {
        perror("Erreur création du fork");
        exit(EXIT_FAILURE);
    }
}
