# README - Tchat App'

*Copyright - Alexandre Goux & Martin Levrard*

## Procédure de compilation
Exécuter les deux commandes suivantes pour compiler le programme :

``` bash
gcc -pthread serveur.c -o serveur
gcc -pthread client.c -o client
```

## Lancer l'application
1/ Dans un terminal, lancer le serveur :
``` bash
./serveur
```
2/ Pour chaque client, ouvrir un nouveau temrinal et lancer le client :
``` bash
./client
```
## Jeu d'essai
1/ Lancer le serveur depuis un terminal
``` bash
./serveur
```

2/ Lancer les clients (un par terminal) :

``` bash
./client
Your pseudo : Ketchup
Your groupe name : Sauce
```

``` bash
./client
Your pseudo : Mayo
Your groupe name : Sauce
```

3/ Envoyer un message depuis Ketchup.

4/ Envoyer un message depuis Mayo.

5/ Quitter le client Mayo :
``` bash
!quit
```

6/ Lancer un nouveau client :
``` bash
./client
Your pseudo : Sel
Your groupe name : Epice
```

7/ Afficher l'aide :

``` bash
!help
```

8/ Afficher le nombre de client connécté sur le serveur :

``` bash
!nb
```

9/ Afficher la liste des couleurs dispos :

``` bash
!listColors
```

10/ Changer de couleur :

``` bash
!color red
```

11/ Changer de couleur :

``` bash
!color red
```













