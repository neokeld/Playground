#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "parser.h"

#define MAX 100
/* Longueur maximum d'une ligne */
#define MAX_NODES 10000
/* Nombre maximum de nodes */

/* Teste si le fichier est bien un fichier de graphe valide
 * en testant la première ligne du fichier.
 * Retourne 1 si vrai, 0 sinon. */
int parse_graph_label(char * s)
{
    int returnValue, nbTokens, i;
    int maxTokens = 3;
    char ** tokens = malloc(maxTokens*sizeof(char*));
    nbTokens = strsplit(s, tokens, maxTokens, " ");
    /* s1 doit contenir 3 tokens.
     * Le 1er token doit être "graph" et
     * le 3ième token doit être une "{" suivi d'un retour à la ligne */
    returnValue = (nbTokens == 3 && strcmp(tokens[0], "graph") == 0 && strcmp(tokens[2], "{\n") == 0);
    for(i=0; i < nbTokens; i++)
	free(tokens[i]);
    free(tokens);
    return returnValue;
}

/* Teste si la ligne est bien une affectation de label de noeud à un noeud
 * Retourne 1 si vrai, 0 sinon. */
int parse_node(char * s, struct graph * graph, char ** nodes_tab, int * nb_nodes)
{
    int returnValue, i, nbTokens, nbTokens2;
    int maxTokens = 3;
    char ** tokens = malloc(maxTokens*sizeof(char*));
    char ** tokens2 = malloc(maxTokens*sizeof(char*));
    /* On teste si on a bien deux tokens sur la ligne */
    if((nbTokens = strsplit(s, tokens, maxTokens, " ")) != 2)
	returnValue = 0;
    else {
	/* On teste si le 2ème tokens se splite en 3 tokens */
	if((nbTokens2 = strsplit(tokens[1], tokens2, maxTokens, "\"")) != 3)
	    returnValue = 0;
	else {
	    /* On vérifie la forme du token */
	    returnValue = (strcmp(tokens2[0], "[label=") == 0 && strcmp(tokens2[2], "];\n") == 0);
	    if(returnValue) {
		/* On stocke la correspondance nom du noeud / label du noeud */
		nodes_tab[*nb_nodes] = malloc((strlen(tokens[0])+1)*sizeof(char));
		strcpy(nodes_tab[*nb_nodes], tokens[0]);
		*nb_nodes = *nb_nodes +1;
		nodes_tab[*nb_nodes] = malloc((strlen(tokens2[1])+1)*sizeof(char));
		strcpy(nodes_tab[*nb_nodes], tokens2[1]);
		*nb_nodes = *nb_nodes +1;
		/* On ajoute le noeud au graphe */
		ajouter_Noeud(graph, tokens2[1]);
	    }
	}
	for(i=0; i < nbTokens2; i++)
	    free(tokens2[i]);
    }
    for(i=0; i < nbTokens; i++)
	free(tokens[i]);
    free(tokens);
    free(tokens2);
    return returnValue;
}

/* Teste si la ligne est bien une affectation d'un label à une arête.
 * Ce label doit être un entier.
 * Retourne 1 si vrai, 0 sinon. */
int parse_edge(char * s, struct graph * graph, char ** nodes_tab, int * nb_nodes)
{
    int i, node1, node2, returnValue, nbTokens, nbTokens2;
    int maxTokens = 4;
    char ** tokens = malloc(maxTokens*sizeof(char*));
    char ** tokens2 = malloc(maxTokens*sizeof(char*));
    /* On teste si on a 4 tokens sur la ligne */
    if((nbTokens = strsplit(s, tokens, maxTokens, " ")) != 4)
	returnValue = 0;
    else if (strcmp(tokens[1], "--") != 0)
	returnValue = 0;
    else {
	/* On teste si le 4ème token contient 3 sous-tokens */
	if((nbTokens2 = strsplit(tokens[3], tokens2, maxTokens, "\"")) != 3)
	    returnValue = 0;
	else if (strcmp(tokens2[0], "[label=") == 0
		&& strcmp(tokens2[2], "];\n") == 0
		&& secure_atoi(tokens2[1]) >= 0) {
	    returnValue = 1;
	    i = 0, node1 = 0, node2 = 0;
	    /* On cherche dans le tableau de nodes le label correspondant au nom du noeud */
	    while(i < *nb_nodes)
	    {
		if(strcmp(tokens[0], nodes_tab[i]) == 0)
		    node1 = i;
		if(strcmp(tokens[2], nodes_tab[i]) == 0)
		    node2 = i;
		i = i+2;
	    }
	    /* On ajoute l'arête au graphe */
	    ajouter_Lien(graph, nodes_tab[node1+1], nodes_tab[node2+1], secure_atoi(tokens2[1]));
	}
	for(i=0; i < nbTokens2; i++)
	    free(tokens2[i]);
    }
    for(i=0; i < nbTokens; i++)
	free(tokens[i]); 
    free(tokens);
    free(tokens2);
    return returnValue;
}

/* Lit un fichier de graphe et
 * stocke son contenu dans une structure graph.
 * Retourne 1 si réussi, 0 sinon. */
int parse(char * fichier, struct graph * graph)
{
    FILE * f;
    f = fopen(fichier, "r");
    if(!f)
    {
	printf("Le fichier \"%s\" n'existe pas.\n", fichier);
	return 0;
    }
    else
    {
	char chaine[MAX] = "";
	fgets(chaine, MAX, f);
	/* On vérifie que l'on a bien un graphe en essayant de parser son label */
	if(parse_graph_label(chaine) != 0)
	{
	    int i;
	    char ** nodes_tab = malloc(2*MAX_NODES*sizeof(char *));
	    int * nb_nodes = malloc(sizeof(int));
	    *nb_nodes = 0;

	    fgets(chaine, MAX, f);
	    /* On parse les lignes contenant les nodes une à une */
	    while(parse_node(chaine, graph, nodes_tab, nb_nodes) != 0)
	    {
		fgets(chaine, MAX, f);
	    }
	    /* Ensuite on parse les lignes contenant les arêtes une à une */
	    while(parse_edge(chaine, graph, nodes_tab, nb_nodes) != 0)
	    {
		fgets(chaine, MAX, f);
	    }

	    /* Libérer les données allouées */
	    for(i = 0; i < *nb_nodes; i++)
		free(nodes_tab[i]);
	    free(nodes_tab);
	    free(nb_nodes);
	}
	else
	{
	    printf("Le fichier de graphe est erroné. \n");
	    return 0;
	}
    }

    fclose(f);
    return 1;
}
