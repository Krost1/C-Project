# Jeu Bomberman

## Principe du projet

Le projet consiste à développer, d'une part, le serveur de jeu lui-même et, d'autre part, le client permettant à des utilisateurs de se connecter au serveur afin de jouer les uns contre les autres.

## Contributeurs

* Matsanga Nsoulou Christ
* Cornet Yann

## À propos

Cette version comporte des améliorations ainsi que des points non abordés ou résolus lors de la première version.

Pour prendre en compte les connexions en dehors du réseau local :

---

* **Adresse IP publique :**  Nous aurons besoin de notre adresse IP publique, qui peut être celle de notre routeur ou de la machine sur laquelle notre serveur s'exécute. Nous pouvons utiliser des services tels que "WhatIsMyIP" pour connaître notre adresse IP publique.

* **Redirection de port :** Si notre serveur est derrière un routeur ou un pare-feu, nous devrons configurer une redirection de port pour diriger le trafic depuis notre adresse IP publique et le port spécifié vers l'adresse IP locale de notre serveur et le port correspondant.

* **Gestion des pare-feux :** Nous devrons nous assurer que notre pare-feu autorise le trafic sur les ports que nous utilisons. Cela s'applique à la fois au pare-feu logiciel sur notre machine et à tout pare-feu matériel sur notre réseau.

* **Utilisation d'IPv6 :** Si nous souhaitons également prendre en charge la communication avec des personnes en dehors de notre réseau local, nous pourrions envisager d'utiliser IPv6, car les adresses IPv4 publiques sont limitées et souvent partagées.

* **Sécurité :** Lorsque nous exposons un service sur Internet, la sécurité devient une préoccupation majeure. Nous devons nous assurer de mettre en œuvre des mesures de sécurité appropriées pour éviter les vulnérabilités.
