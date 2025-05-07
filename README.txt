Bienvenue sur ce jeu de Tarot Africain !

Les règles de ce jeu sont basées sur celles présentées ici: http://soleil365.over-blog.com/2014/10/regles-du-tarot-africain.html
La seule modification est que lors du 6ème tour l'excuse vaut systématiquement 0.

Il existe 2 versions de ce jeu : la première se base sur un protocole TCP et l'autre sur un protocole UDP.
Vous trouverez ces versions dans les répertoires correspondants.
Pour compiler ce jeu, une fois dans un des 2 répertoires, il suffit de taper la commande "make".
Il se peut que des warnings apparaissent, mais ceux-ci n'empêchent pas la bonne exécution du jeu.
Vous pouvez ensuite installer grâce à la commande "make install".
Vous aurez alors dans un dossier bin deux exécutables : server et client.
Pour jouer, il faut que les trois joueurs soient sur le même réseau.
Il faut alors qu'un des trois joueurs exécute dans un terminal ./server.
Les trois joueurs peuvent alors exécuter ./client comme suit:
./client "IP du joueur qui fait tourner le serveur" "pseudo"

L'IP est celle obtenu avec "hostname -I"

Pour désinstaller il faut utiliser "make uninstall".
Enfin, il y a une commande "make clean" qui efface tous les fichiers .o ainsi que les exécutables.
