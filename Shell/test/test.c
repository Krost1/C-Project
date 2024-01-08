#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 100

typedef struct
{
    char stack[MAX_SIZE];
    int top;
} Stack;

void initialize(Stack *s)
{
    s->top = -1;
}

int isEmpty(Stack *s)
{
    return s->top == -1;
}

int isFull(Stack *s)
{
    return s->top == MAX_SIZE - 1;
}

void push(Stack *s, char element)
{
    if (!isFull(s))
    {
        s->stack[++s->top] = element;
    }
    else
    {
        printf("Stack overflow\n");
        exit(EXIT_FAILURE);
    }
}

char pop(Stack *s)
{
    if (!isEmpty(s))
    {
        return s->stack[s->top--];
    }
    else
    {
        printf("Stack underflow\n");
        exit(EXIT_FAILURE);
    }
}

char *peek(Stack *s)
{
    static char str[2];
    str[0] = s->stack[s->top];
    str[1] = '\0';
    return str;
}

int getPriority(char *operator)
{
    if (strcmp(operator, "(") == 0 || strcmp(operator, ")") == 0)
        return 0;
    else if (strcmp(operator, "!") == 0)
        return 1;
    else if (strcmp(operator, "*") == 0 || strcmp(operator, "?") == 0 || strcmp(operator, "[") == 0)
        return 2;
    else if (strcmp(operator, "-") == 0 || strcmp(operator, "+") == 0 || strcmp(operator, "^") == 0)
        return 3;
    else if (strcmp(operator, "/") == 0 || strcmp(operator, "%") == 0)
        return 4;
    else if (strcmp(operator, "<") == 0 || strcmp(operator, ">") == 0 || strcmp(operator, "<=") == 0 || strcmp(operator, ">=") == 0)
        return 5;
    else if (strcmp(operator, "==") == 0 || strcmp(operator, "!=") == 0)
        return 6;
    else if (strcmp(operator, "&") == 0)
        return 7;
    else if (strcmp(operator, "^") == 0)
        return 8;
    else if (strcmp(operator, "|") == 0)
        return 9;
    else if (strcmp(operator, "&&") == 0)
        return 10;
    else if (strcmp(operator, "||") == 0)
        return 11;
    else if (strcmp(operator, ";") == 0 || strcmp(operator, "&") == 0)
        return 12;
    else
        return -1; // Opérateur non pris en charge
}

void createCommandStack(char *expression)
{
    Stack operatorStack;
    initialize(&operatorStack);

    for (int i = 0; expression[i] != '\0'; ++i)
    {
        char currentChar = expression[i];
        char nextChar = expression[i + 1];
        char temp[3] = {currentChar, nextChar, '\0'}; // Convertir les caractères en chaîne

        if (currentChar == '(')
        {
            push(&operatorStack, currentChar);
        }
        else if (currentChar == ')')
        {
            while (!isEmpty(&operatorStack) && strcmp(peek(&operatorStack), "(") != 0)
            {
                printf("%c ", pop(&operatorStack));
            }
            pop(&operatorStack); // Pop '('
        }
        else
        {
            while (!isEmpty(&operatorStack) && getPriority(temp) <= getPriority(peek(&operatorStack)))
            {
                printf("%c ", pop(&operatorStack));
            }
            push(&operatorStack, currentChar);
            if (getPriority(temp) != -1) // Si l'opérateur a deux caractères
            {
                push(&operatorStack, nextChar);
                i++; // Passer le caractère suivant car il a déjà été traité
            }
        }
    }

    while (!isEmpty(&operatorStack))
    {
        printf("%c ", pop(&operatorStack));
    }
}

int main()
{
    char *expression = "find /home/user -type f ( -name *.txt -o -name *.log ) -exec cp {} /backup && (grep \"error\" /var/log/syslog || echo \"No errors found\") ; (ps aux | grep \"process\" | awk '{print $2}' | xargs kill -9) || echo \"Process not found\"";

    printf("Pile de commandes : ");
    createCommandStack(expression);

    return 0;
}
