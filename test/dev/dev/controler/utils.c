#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "utils.h"

/* Fait une conversion string vers int en faisant des vérifications */
int secure_atoi(char* str)
{
    char *end;
    int si;

    const long sl = strtol(str, &end, 10);

    if (end == str) {
	printf("Erreur : %s n'est pas un nombre\n", str);
    }
    else if (sl > INT_MAX) {
	printf("Erreur : %ld plus grand que INT_MAX\n", sl);
    }
    else if (sl < INT_MIN) {
	printf("Erreur : %ld plus petit que INT_MIN\n", sl);
    }
    else {
	si = (int)sl;
	return si;
    }
    return -1;
}

/* Sépare une chaine c en un ensemble de tokens split
 * de nombre de tokens maximum maxTokens
 * en les coupant au délimiteur delim.
 * Retourne le nombre de tokens. */
int strsplit(const char * c, char ** tokens, int maxTokens, char * delim)
{
    int i = 0; /* Nb de tokens */
    char * s; /* Token en cours */
    /* On copie la chaine c dans chaine car strtok modifie son 1er paramètre */
    char * chaine = malloc((strlen(c)+1)*sizeof(char));
    if (chaine != NULL) {
	strcpy(chaine, c);
	/* On récupère le premier token */
	s = strtok(chaine, delim);
	/* Tant qu'on trouve toujours des tokens et qu'on a pas atteint le maximum */
	while(s && i < maxTokens)
	{
	    /* On copie le dernier token trouvé dans le tableau tokens */
	    tokens[i] = malloc((strlen(s)+1)*sizeof(char));
	    strcpy(tokens[i], s);
	    s = strtok(NULL, delim);
	    i++;
	}
	free(chaine);
    }
    else
	i = -1;
    return i;
}


