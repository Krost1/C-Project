#ifndef VARIABLE_H
#define VARIABLE_H

void initialiserEnvironnementShell(EnvironnementShell *shell_env, char *envp[]);

void afficherEnvironnement(const EnvironnementShell *shell_env);

void ajouterVariable(Variable **variables, size_t *count, const char *nom, const char *valeur);

void supprimerVariable(Variable **variables, size_t *count, const char *nom);

void supprimerVariableEnvironnement(EnvironnementShell *shell_env, const char *nom);

void supprimerVariableLocale(EnvironnementShell *shell_env, const char *nom);

void ajouterVariableEnvironnement(EnvironnementShell *shell_env, const char *nom, const char *valeur);

void ajouterVariableLocale(EnvironnementShell *shell_env, const char *nom, const char *valeur);

void detruireEnvironnementShell(EnvironnementShell *shell_env);

char *obtenirVariableLocale(const EnvironnementShell *shell_env, const char *nom);

char *obtenirVariableEnvironnement(const EnvironnementShell *shell_env, const char *nom);

void setInterne(EnvironnementShell *shell_env, const char *arg);

void setenvInterne(EnvironnementShell *shell_env, const char *arg);

void unsetInterne(EnvironnementShell *shell_env, const char *nom);

void unsetenvInterne(EnvironnementShell *shell_env, const char *nom);

void env_var(char *envp[]);
#endif // VARIABLE_H
