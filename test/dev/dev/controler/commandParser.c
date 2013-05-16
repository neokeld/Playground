#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "commandParser.h"

#include "utils.h"
#include "parser.h"

#define MAX_LEN_LINE 256
/* Longueur max d'une commande */
#define MAX_NB_TOKENS 5
/* Nb max de tokens par commande */

int is_graph_load = 0;
/* vaut 1 si le graph est chargé, 0 sinon */

/* Analyse une commande passée au controleur et
 * fait l'action associée
 * Retourne 1 si OK, 0 si l'utilisateur ou l'application veut quitter */
int analyse_cmd(struct graph * graph, char * cmd)
{
    int returnValue = 1;
    char ** tokens;
    int i, nbTokens;
    tokens = malloc(MAX_NB_TOKENS*sizeof(char*));
    nbTokens = strsplit(cmd, tokens, MAX_LEN_LINE, " ");
    if(nbTokens > 0) {
	if (strcmp(tokens[0], "help") == 0) {
	    printf("Rappel des commandes :\n\n load fichier :\n Chargement d'une topologie.\n\n show topology :\n Afficher la topologie actuelle.\n\n add link N1 N2 cout :\n Ajouter un lien entre deux nœuds en spécifiant le coût du lien (entier).\n\n del link N1 N2 :\n Supprimer un lien entre deux routeurs.\n\n");
	    printf(" disconnect N :\n Supprimer tous les liens à partir d'un routeur. Dans ce cas, le routeur est toujours actif mais il se retrouve déconnecté du réseau.\n\n update link N1 N2 cout :\n Modifier le coût d'un lien dans le réseau.\n\n save fichier :\n Enregistrer / exporter la topologie actuelle dans un fichier.\n\n help :\n Affiche cette aide.\n\n quit : Quitte le programme.\n");
	}
	else if (strcmp(tokens[0], "quit") == 0) {
	   returnValue = 0; 
	}
	else if (nbTokens == 2 && strcmp(tokens[0], "load") == 0) {
	    if(is_graph_load == 0) {
		if(parse(tokens[1], graph) == 0)
		{
		    free_graph(graph);
		}
		else {
		    printf("    -> Topology loaded (%d nodes in the network) !\n", graph->nombre_Noeud);
		    is_graph_load = 1;
		}
	    } else
		printf("Une topologie est déjà ajoutée.\n");
	}
	else if (is_graph_load == 1) {
	    if (strcmp(tokens[0], "show") == 0)
	    {
		show_Topology(graph);
	    }
	    else if (nbTokens == 2 && strcmp(tokens[0], "save") == 0)
	    {
		sauvegarder_Topology(graph, tokens[1]);
		printf("    -> Topology loaded (%d nodes in the network) !\n", graph->nombre_Noeud);
	    }
	    else if (nbTokens == 5 && strcmp(tokens[0], "add") == 0 && strcmp(tokens[1], "link") == 0) {
		int cost = secure_atoi(tokens[4]);
		if (cost < 0)
		    printf("Votre coût n'est pas un entier positif.\n");
		else {
		    ajouter_Lien(graph, tokens[2], tokens[3], cost);
		    printf("    -> Link created between %s and %s. Link cost = %d.\n", tokens[2], tokens[3], cost);
		}
	    } else if (nbTokens == 4 && strcmp(tokens[0], "del") == 0 && strcmp(tokens[1], "link") == 0) {
		supprimer_Lien(graph, tokens[2], tokens[3]);
		printf("    -> Link deleted between %s and %s.\n", tokens[2], tokens[3]);
	    }
	    else if (nbTokens == 5 && strcmp(tokens[0], "update") == 0 && strcmp(tokens[1], "link") == 0) {
		int cost = secure_atoi(tokens[4]);
		if(cost < 0)
		    printf("Votre coût n'est pas un entier positif.\n");
		else {
		    modifier_cout(graph, tokens[2], tokens[3], cost);
		    printf("    -> Link updated between %s and %s. Link cost = %d.\n", tokens[2], tokens[3], cost);
		}
	    } else if (nbTokens == 3 && strcmp(tokens[0], "add") == 0 && strcmp(tokens[1], "node") == 0) {   
		    ajouter_Noeud(graph, tokens[2]);
		    printf("    -> Node %s created.\n", tokens[2]);
		}
	    else if (nbTokens == 2 && strcmp(tokens[0], "disconnect") == 0) {
		deconnecter_Routeur(graph, tokens[1]);
		printf("    -> Node %s is disconnected !\n", tokens[1]);
	    }
	    else
		printf("This command does not exist !\n");
	}
	else
	    printf("La topologie du réseau n'a pas encore été chargée.\n Tapez help pour un rappel des commandes.\n");
    } else
	printf("Veuillez rentrer une commande.\n Tapez help pour un rappel des commandes.\n");


    for(i=0; i < nbTokens; i++)
	free(tokens[i]);
    free(tokens);

    return returnValue;
}

