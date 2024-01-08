#include "../include/inc.h"
#include "../include/strings.h"

// Fonction pour initialiser l'environnement du shell à partir du tableau envp
void initialiserEnvironnementShell(EnvironnementShell *shell_env, char *envp[])
{
    shell_env->local_variables = NULL;
    shell_env->local_count = 0;
    shell_env->environment_variables = NULL;
    shell_env->environment_count = 0;

    // Parcourir le tableau envp et ajouter chaque variable à la liste
    for (size_t i = 0; envp[i] != NULL; i++)
    {
        char *variable = strdup(envp[i]);
        char *nom = strtok(variable, "=");
        char *valeur = strtok(NULL, "=");

        // Ajouter la variable à la liste des variables d'environnement
        if (nom != NULL && valeur != NULL)
        {
            shell_env->environment_variables = realloc(shell_env->environment_variables,
                                                       (shell_env->environment_count + 1) * sizeof(Variable));
            shell_env->environment_variables[shell_env->environment_count].name = strdup(nom);
            shell_env->environment_variables[shell_env->environment_count].value = strdup(valeur);
            shell_env->environment_count++;
        }
        free(variable);
    }
}

// Fonction pour afficher les variables d'environnement
void afficherEnvironnement(const EnvironnementShell *shell_env)
{
    printf("Variables d'environnement :\n");
    for (size_t i = 0; i < shell_env->environment_count; i++)
    {
        printf("%s=%s\n", shell_env->environment_variables[i].name, shell_env->environment_variables[i].value);
    }
    printf("Variables locales :\n");
    for (size_t i = 0; i < shell_env->local_count; i++)
    {
        printf("%s=%s\n", shell_env->local_variables[i].name, shell_env->local_variables[i].value);
    }
}

// Fonction pour ajouter une nouvelle variable
void ajouterVariable(Variable **variables, size_t *count, const char *nom, const char *valeur)
{
    *variables = realloc(*variables, (*count + 1) * sizeof(Variable));
    (*variables)[*count].name = strdup(nom);
    (*variables)[*count].value = strdup(valeur);
    (*count)++;
}

// Fonction pour supprimer une variable
void supprimerVariable(Variable **variables, size_t *count, const char *nom)
{
    for (size_t i = 0; i < *count; i++)
    {
        if (strcmp((*variables)[i].name, nom) == 0)
        {
            free((*variables)[i].name);
            free((*variables)[i].value);

            // Déplacer les variables suivantes pour remplir l'emplacement supprimé
            for (size_t j = i; j < *count - 1; j++)
            {
                (*variables)[j] = (*variables)[j + 1];
            }
            (*count)--;
            *variables = realloc(*variables, *count * sizeof(Variable));
            break;
        }
    }
}

// Fonction pour supprimer une variable d'environnement
void supprimerVariableEnvironnement(EnvironnementShell *shell_env, const char *nom)
{
    supprimerVariable(&shell_env->environment_variables, &shell_env->environment_count, nom);
}

// Fonction pour supprimer une variable locale
void supprimerVariableLocale(EnvironnementShell *shell_env, const char *nom)
{
    supprimerVariable(&shell_env->local_variables, &shell_env->local_count, nom);
}

// Fonction pour ajouter une nouvelle variable d'environnement
void ajouterVariableEnvironnement(EnvironnementShell *shell_env, const char *nom, const char *valeur)
{
    ajouterVariable(&shell_env->environment_variables, &shell_env->environment_count, nom, valeur);
    supprimerVariableLocale(shell_env, nom);
}

// Fonction pour ajouter une nouvelle variable locale
void ajouterVariableLocale(EnvironnementShell *shell_env, const char *nom, const char *valeur)
{
    ajouterVariable(&shell_env->local_variables, &shell_env->local_count, nom, valeur);
    supprimerVariableEnvironnement(shell_env, nom);
}

// Fonction pour détruire l'environnement du shell
void detruireEnvironnementShell(EnvironnementShell *shell_env)
{
    for (size_t i = 0; i < shell_env->environment_count; i++)
    {
        free(shell_env->environment_variables[i].name);
        free(shell_env->environment_variables[i].value);
    }
    free(shell_env->environment_variables);

    for (size_t i = 0; i < shell_env->local_count; i++)
    {
        free(shell_env->local_variables[i].name);
        free(shell_env->local_variables[i].value);
    }
    free(shell_env->local_variables);
    shell_env->environment_count = 0;
    shell_env->local_count = 0;
}

// Recherche une variable locale par nom
char *obtenirVariableLocale(const EnvironnementShell *shell_env, const char *nom)
{
    for (size_t i = 0; i < shell_env->local_count; i++)
    {
        if (strcmp(shell_env->local_variables[i].name, nom) == 0)
        {
            return shell_env->local_variables[i].value;
        }
    }
    return NULL;
}

// Recherche une variable d'environnement par nom
char *obtenirVariableEnvironnement(const EnvironnementShell *shell_env, const char *nom)
{
    for (size_t i = 0; i < shell_env->environment_count; i++)
    {
        if (strcmp(shell_env->environment_variables[i].name, nom) == 0)
        {
            return shell_env->environment_variables[i].value;
        }
    }
    return NULL;
}

// Fonction pour définir ou afficher une variable locale
void setInterne(EnvironnementShell *shell_env, const char *arg)
{
    if (arg == NULL)
    {
        // Afficher toutes les variables locales
        printf("Variables locales :\n");
        for (size_t i = 0; i < shell_env->local_count; i++)
        {
            printf("%s=%s\n", shell_env->local_variables[i].name, shell_env->local_variables[i].value);
        }
    }
    else
    {
        // Définir une nouvelle variable locale ou mettre à jour une existante
        char *arg_copy = strdup(arg);
        char *nom = strtok(arg_copy, "=");
        char *valeur = strtok(NULL, "=");

        // Vérifier si la variable existe déjà, sinon la créer
        size_t i;
        for (i = 0; i < shell_env->local_count; i++)
        {
            if (strcmp(shell_env->local_variables[i].name, nom) == 0)
            {
                free(shell_env->local_variables[i].value);
                shell_env->local_variables[i].value = strdup(valeur);
                break;
            }
        }
        if (i == shell_env->local_count)
        {
            // La variable n'existe pas alors on l'ajoute
            ajouterVariableLocale(shell_env, nom, valeur);
        }

        // Si la valeur commence par '$', remplacez-la par la valeur de la variable locale correspondante
        if (valeur[0] == '$')
        {
            char *valeur_referencee = obtenirVariableLocale(shell_env, valeur + 1);
            if (valeur_referencee != NULL)
            {
                free(shell_env->local_variables[i].value);
                shell_env->local_variables[i].value = strdup(valeur_referencee);
            }
        }
        free(arg_copy);
    }
}

// Fonction pour définir ou afficher une variable d'environnement
void setenvInterne(EnvironnementShell *shell_env, const char *arg)
{
    if (arg == NULL)
    {
        // Afficher toutes les variables d'environnement
        printf("Variables d'environnement :\n");
        for (size_t i = 0; i < shell_env->environment_count; i++)
        {
            printf("%s=%s\n", shell_env->environment_variables[i].name, shell_env->environment_variables[i].value);
        }
    }
    else
    {
        // Définir une nouvelle variable d'environnement ou mettre à jour une existante
        char *arg_copy = strdup(arg);
        char *nom = strtok(arg_copy, "=");
        char *valeur = strtok(NULL, "=");

        // Vérifier si la variable existe déjà, sinon la créer
        size_t i;
        for (i = 0; i < shell_env->environment_count; i++)
        {
            if (strcmp(shell_env->environment_variables[i].name, nom) == 0)
            {
                free(shell_env->environment_variables[i].value);
                shell_env->environment_variables[i].value = strdup(valeur);
                break;
            }
        }
        if (i == shell_env->environment_count)
        {
            // La variable n'existe pas alors on l'ajoute
            ajouterVariableEnvironnement(shell_env, nom, valeur);
        }

        // Si la valeur commence par '$', remplacez-la par la valeur de la variable locale correspondante
        if (valeur[0] == '$')
        {
            char *valeur_referencee = obtenirVariableLocale(shell_env, valeur + 1);
            if (valeur_referencee != NULL)
            {
                free(shell_env->environment_variables[i].value);
                shell_env->environment_variables[i].value = strdup(valeur_referencee);
            }
        }
        free(arg_copy);
    }
}

// Commande interne pour supprimer une variable locale
void unsetInterne(EnvironnementShell *shell_env, const char *nom)
{
    if (nom != NULL)
    {
        if (nom[0] == '$')
        {
            // Supprimer la variable en enlevant le '$'
            supprimerVariableLocale(shell_env, nom + 1);
        }
        else
        {
            // Supprimer la variable normalement
            supprimerVariableLocale(shell_env, nom);
        }
    }
}

// Commande interne pour supprimer une variable d'environnement
void unsetenvInterne(EnvironnementShell *shell_env, const char *nom)
{
    if (nom != NULL)
    {
        if (nom[0] == '$')
        {
            // Supprimer la variable en enlevant le '$'
            supprimerVariableEnvironnement(shell_env, nom + 1);
        }
        else
        {
            // Supprimer la variable normalement
            supprimerVariableEnvironnement(shell_env, nom);
        }
    }
}

void env_var(char *envp[])
{

    // Initialiser l'environnement du shell
    EnvironnementShell shell_env;
    initialiserEnvironnementShell(&shell_env, envp);

    // Boucle principale du shell
    while (1)
    {
        char *saisie;
        printf("∼>");
        saisie = input();

        // Supprimer le caractère de nouvelle ligne de la saisie
        size_t longueur_saisie = strlen(saisie);
        if (longueur_saisie > 0 && saisie[longueur_saisie - 1] == '\n')
        {
            saisie[longueur_saisie - 1] = '\0';
        }

        // Séparer la commande et l'argument
        char *commande = strtok(saisie, " ");
        char *argument = strtok(NULL, " ");

        // Traitement des commandes internes
        if (strcmp(commande, "set") == 0)
        {
            setInterne(&shell_env, argument);
        }
        else if (strcmp(commande, "setenv") == 0)
        {
            setenvInterne(&shell_env, argument);
        }
        else if (strcmp(commande, "unset") == 0)
        {
            unsetInterne(&shell_env, argument);
        }
        else if (strcmp(commande, "unsetenv") == 0)
        {
            unsetenvInterne(&shell_env, argument);
        }
        else if (strcmp(commande, "echo") == 0)
        {
            // Afficher la valeur de la variable spécifiée
            if (argument != NULL && argument[0] == '$')
            {
                argument++; // Ignorer le symbole '$'
                // Chercher la variable dans les variables locales
                for (size_t i = 0; i < shell_env.local_count; i++)
                {
                    if (strcmp(shell_env.local_variables[i].name, argument) == 0)
                    {
                        printf("%s\n", shell_env.local_variables[i].value);
                        break;
                    }
                }
                // Si la variable n'a pas été trouvée dans les variables locales, chercher dans les variables d'environnement
                for (size_t i = 0; i < shell_env.environment_count; i++)
                {
                    if (strcmp(shell_env.environment_variables[i].name, argument) == 0)
                    {
                        printf("%s\n", shell_env.environment_variables[i].value);
                        break;
                    }
                }
            }
            else
            {
                // Si l'argument n'est pas une variable, afficher le message d'erreur approprié
                printf("Erreur : L'argument de la commande echo doit commencer par '$'.\n");
            }
        }
        else if (strcmp(commande, "exit") == 0)
        {
            break;
        }
        else
        {
            afficherEnvironnement(&shell_env);
        }
        free(saisie);
    }
    // Détruire l'environnement du shell
    detruireEnvironnementShell(&shell_env);
}