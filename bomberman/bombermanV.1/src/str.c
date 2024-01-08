
#include "../include/inc.h"

int length(char *chain)
{
    int count = 0;
    while (chain[count] != '\0')
    {
        count++;
    }
    return count;
}

void concatane_index(char *chain_one, char *chain_two, int n)
{
    for (int i = 0; i < n; i++)
    {
        chain_one[i] = chain_two[i];
    }
    chain_one[n] = '\0';
}

char **tokenize(char *chain)
{
    int i = 0, j = 0, start = 0, size = length(chain);

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
                concatane_index(token[j], chain + start, tokenLength);
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
        concatane_index(token[j], chain + start, tokenLength);
        j++;
    }

    token[j] = NULL;
    return token;
}