Pour compiler le programme il faut utiliser la commande "make" avec le nom du fichier sans le ".c" pour le fichier désiré.

Pour lancer les programme:

- faire la commande "./serveurX" pour lancer le serveur de la valeur X (ici 1 ou 2).
Par défaut, la commande mets le trésor aux coordonnées (4,5) mais si on l'appelle avec le paramètre "rand" les coordonnées du trésor seront aléatoire et seront affichées par le serveur à la connexion d'un client.

- faire la commande "./client @IP @Port" permet de lancer une connexion avec le client et d'initialiser une partie sur le serveur.