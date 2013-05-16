#include <stdlib.h>
#include <stdio.h>

#include "parser.h"

int main(int argc, char** argv) {
    struct graph * graph;
    /* Si les arguments ne sont pas bons, rappeler l'usage */
    if(argc != 2) {
	printf("Usage : %s fichier\n", argv[0]);
	return -1;
    }
    /* Initialiser la structure graph */
    graph = initialiser_Graph();

    /* Remplir la structure graph et afficher son contenu */
    if (parse(argv[1], graph))
	show_Topology(graph);

    /* Effacer la structure */
    free_graph(graph);
    return 0;
}
