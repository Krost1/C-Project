#ifndef BACK_FORE_GTOUND_H
#define BACK_FORE_GTOUND_H

/**
 *  @brief Une fonction qui exécute la commande éventuellement en arrière plan.
 *
 *  @commande tableau de caractères.
 *  @arriere_plan un entier pour savoir si c'est en arrière plan.
 */
void executer_commande(char *commande, int arriere_plan);

/**
 *  @brief Cette fonction affiche l'état actuel du ou des processus.
 */
void afficher_jobs();

/**
 *  @brief  La fonction sert à mettre à jour l'état d'un job.
 *
 *  @pid le pid du processus.
 *  @etat le nouvel état du job.
 */
void mettre_a_jour_etat_job(pid_t pid, EtatJob etat);

/**
 *  @brief La fonction est utilisée pour reprendre un job en cours d'exécution après qu'il ait été arrêté.
 *
 *  @numero_job Le numéro du job à reprendre.
 */
void reprendre_job(int numero_job);

/**
 *  @brief La fonction est utilisée pour faire un job en premier plan mettre à jour son état et afficher des informations.
 *
 *  @numero_job Le numéro du job à traiter en premier plan.
 */
void processus_premier_plan(int numero_job);

/**
 *  @brief  La fonction est utilisée pour faire un job en arrière-plan en le reprenant s'il était arrêté ou en affichant un message approprié ou son état actuel
 *
 *  @numero_job Le numéro du job à traiter en arrière-plan.
 */
void back_fore_ground(char commande[BUFFER_SIZE]);

#endif // BACK_FORE_GTOUND_H