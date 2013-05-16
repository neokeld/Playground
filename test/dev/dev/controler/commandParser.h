#ifndef __COMMAND_PARSER_H_
#define __COMMAND_PARSER_H_

#include "graph.h"

/* Analyse une commande passée au controleur et
 * fait l'action associée
 * Retourne 1 si OK, 0 si l'utilisateur ou l'application veut quitter */
int analyse_cmd(struct graph * graph, char * cmd);

#endif /* commandParser.h */
