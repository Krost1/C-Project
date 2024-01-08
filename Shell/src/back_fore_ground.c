#include "../include/inc.h"

Job jobs[NOMBRE_MAX_JOBS];
int prochain_numero_job = 1;

void executer_commande(char *commande, int arriere_plan)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        if (arriere_plan)
        {
            setpgid(0, 0);
        }

        execlp("/bin/sh", "/bin/sh", "-c", commande, (char *)NULL);
        perror("échec de l'exécution");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        if (!arriere_plan)
        {
            int status;
            waitpid(pid, &status, 0);
            printf("[%d] %d terminé avec le statut %d\n", prochain_numero_job, pid, WEXITSTATUS(status));
            prochain_numero_job++;
        }
        else
        {
            printf("[%d] %d\n", prochain_numero_job, pid);
            jobs[prochain_numero_job - 1].numero_job = prochain_numero_job;
            jobs[prochain_numero_job - 1].pid = pid;
            jobs[prochain_numero_job - 1].etat = EN_COURS_EXECUTION;
            strcpy(jobs[prochain_numero_job - 1].commande, commande);
            prochain_numero_job++;
        }
    }
    else
    {
        perror("échec de la création du processus fils");
        exit(EXIT_FAILURE);
    }
}

void afficher_jobs()
{
    printf("Processus en arrière-plan :\n");

    for (int i = 0; i < NOMBRE_MAX_JOBS; i++)
    {
        if (jobs[i].pid > 0)
        {
            int statut;
            pid_t resultat = waitpid(jobs[i].pid, &statut, WNOHANG);

            // Vérifiez si le processus est terminé
            if (resultat == -1 || (resultat > 0 && WIFEXITED(statut)))
            {
                jobs[i].etat = ARRETE;
            }

            const char *etat_str = (jobs[i].etat == EN_COURS_EXECUTION) ? "En cours d'exécution" : "Terminé";
            printf("[%d] %d %s %s\n", jobs[i].numero_job, jobs[i].pid, etat_str, jobs[i].commande);
        }
    }
}

void mettre_a_jour_etat_job(pid_t pid, EtatJob etat)
{
    for (int i = 0; i < NOMBRE_MAX_JOBS; i++)
    {
        if (jobs[i].pid == pid)
        {
            jobs[i].etat = etat;
            break;
        }
    }
}

void reprendre_job(int numero_job)
{
    for (int i = 0; i < NOMBRE_MAX_JOBS; i++)
    {
        if (jobs[i].numero_job == numero_job)
        {
            if (jobs[i].etat == ARRETE)
            {
                // Utilisation de SIGCONT pour reprendre un processus stoppé
                kill(jobs[i].pid, SIGCONT);
                jobs[i].etat = EN_COURS_EXECUTION;
                printf("[%d] %s reprend en cours d'exécution\n", jobs[i].numero_job, jobs[i].commande);
            }
            else
            {
                printf("[%d] %s est déjà en cours d'exécution\n", jobs[i].numero_job, jobs[i].commande);
            }
            break;
        }
    }
}

void processus_premier_plan(int numero_job)
{
    for (int i = 0; i < NOMBRE_MAX_JOBS; i++)
    {
        if (jobs[i].numero_job == numero_job)
        {
            if (jobs[i].etat == ARRETE || jobs[i].etat == EN_COURS_EXECUTION)
            {
                int statut;
                // Utilisez WNOHANG pour vérifier si le processus est déjà terminé
                pid_t resultat = waitpid(jobs[i].pid, &statut, WNOHANG);

                if (resultat > 0 && WIFEXITED(statut))
                {
                    // Le processus est déjà terminé, mettez à jour l'état
                    printf("[%d] %s terminé en premier plan avec le statut %d\n", jobs[i].numero_job, jobs[i].commande, WEXITSTATUS(statut));
                    jobs[i].etat = ARRETE;
                }
                else
                {
                    // Le processus n'est pas encore terminé, alors attendez-le
                    waitpid(jobs[i].pid, &statut, 0);
                    printf("[%d] %s terminé en arrière plan avec le statut %d\n", jobs[i].numero_job, jobs[i].commande, WEXITSTATUS(statut));
                    jobs[i].etat = (WIFEXITED(statut)) ? ARRETE : EN_COURS_EXECUTION;
                }
            }
            else
            {
                printf("[%d] %s n'est pas en cours d'exécution ou stoppé\n", jobs[i].numero_job, jobs[i].commande);
            }
            break;
        }
    }
}

void processus_arriere_plan(int numero_job)
{
    for (int i = 0; i < NOMBRE_MAX_JOBS; i++)
    {
        if (jobs[i].numero_job == numero_job)
        {
            if (jobs[i].etat == ARRETE)
            {
                // Passage d'un job stoppé en arrière-plan
                jobs[i].etat = EN_COURS_EXECUTION;
                printf("[%d] %s reprend en arrière-plan\n", jobs[i].numero_job, jobs[i].commande);
            }
            else if (jobs[i].etat == EN_COURS_EXECUTION)
            {
                // Passage d'un job en cours d'exécution en arrière-plan
                printf("[%d] %s est déjà en arrière-plan\n", jobs[i].numero_job, jobs[i].commande);
            }
            else
            {
                printf("[%d] %s ne peut pas être mis en arrière-plan\n", jobs[i].numero_job, jobs[i].commande);
            }
            break;
        }
    }
}

void gestionnaire_SIGTSTP(int sig)
{
    (void)sig;
    // Récupérer le pid du dernier processus en arrière-plan
    pid_t pid = jobs[prochain_numero_job - 2].pid;

    if (pid > 0)
    {
        // Mettre en pause le processus en arrière-plan
        kill(pid, SIGTSTP);

        // Mettre à jour l'état du job
        jobs[prochain_numero_job - 2].etat = ARRETE;

        // Afficher le message indiquant que le processus devient le job xxx et est Stoppé
        printf("[%d] %d devient le job %d et est Stoppé, appuyer sur votre touche entrer pour continuer\n", prochain_numero_job - 1, pid, prochain_numero_job - 1);
    }
}

void back_fore_ground(char commande[BUFFER_SIZE])
{
    signal(SIGTSTP, gestionnaire_SIGTSTP);
    if (strcmp(commande, "myjobs") == 0)
    {
        afficher_jobs();
    }
    else if (strncmp(commande, "myfg", 4) == 0)
    {
        // Commande interne myfg pour passer un job en avant-plan
        int numero_job = atoi(commande + 4);
        processus_premier_plan(numero_job);
    }
    else if (strncmp(commande, "mybg", 4) == 0)
    {
        // Commande interne mybg pour passer un job en arrière-plan
        int numero_job = atoi(commande + 4);
        processus_arriere_plan(numero_job);
    }
    else
    {
        int arriere_plan = 0;
        if (commande[strlen(commande) - 1] == '&')
        {
            arriere_plan = 1;
            commande[strlen(commande) - 1] = '\0';
        }

        executer_commande(commande, arriere_plan);
    }
}
