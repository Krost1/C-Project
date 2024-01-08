#include "../include/inc.h"

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

int length(char *chain)
{
    int count = 0;
    while (chain[count] != '\0')
    {
        count++;
    }
    return count;
}

int size_of_token(char **tokk)
{
    int size = 0;
    while (tokk[size] != NULL)
    {
        size++;
    }
    return size;
}

void concatane_index(char *chain_one, char *chain_two, int n)
{
    for (int i = 0; i < n; i++)
    {
        chain_one[i] = chain_two[i];
    }
    chain_one[n] = '\0';
}

void concatane(char *chain_one, char *chain_two)
{
    int length_chain_one = length(chain_one);
    int length_chain_two = length(chain_two);

    chain_one = realloc(chain_one, sizeof(char) * (length_chain_one + length_chain_two + 1));
    if (chain_one == NULL)
    {
        perror("Reallocation failed");
        exit(EXIT_FAILURE);
    }

    for (int j = 0; j <= length_chain_two; j++)
    {
        chain_one[length_chain_one + j] = chain_two[j];
    }
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

        if (scan_result == EOF || ch == '\n') // EOF ou nouvelle ligne
        {
            buff[i] = '\0';
            break;
        }
        else if (scan_result != 1) // Erreur de lecture
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

int research_in_token(char **token, char *word)
{
    for (int i = 0; token[i] != NULL; i++)
    {
        if (length(token[i]) == length(word))
        {
            int match = 1;
            for (int j = 0; j < length(word); j++)
            {
                if (word[j] != token[i][j])
                {
                    match = 0;
                    break;
                }
            }
            if (match)
            {
                return 0;
            }
        }
    }
    return -1;
}

int compare(char *chain_one, char *chain_two)
{
    int i = 0;
    while (chain_one[i] == chain_two[i])
    {
        if (chain_one[i] == '\0')
        {
            return 0;
        }
        i++;
    }

    return -1;
}
