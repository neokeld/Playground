#ifndef __PARSER_H_
#define __PARSER_H_

#include "graph.h"

/* Teste si le fichier est bien un fichier de graphe valide
 * en testant la première ligne du fichier.
 * Retourne 1 si vrai, 0 sinon. */
int parse_graph_label(char * s);

/* Teste si la ligne est bien une affectation de label de noeud à un noeud
 * Retourne 1 si vrai, 0 sinon. */
int parse_node(char * s, struct graph * graph, char ** nodes_tab, int * nb_nodes);

/* Teste si la ligne est bien une affectation d'un label à une arête.
 * Ce label doit être un entier.
 * Retourne 1 si vrai, 0 sinon. */
int parse_edge(char * s, struct graph * graph, char ** nodes_tab, int * nb_nodes);

/* Lit un fichier de graphe et
 * stocke son contenu dans une structure graph.
 * Retourne 1 si réussi, 0 sinon. */
int parse(char * fichier, struct graph * graph); 

#endif /* parser.h */
