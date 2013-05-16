#ifndef __UTILS_H_
#define __UTILS_H_

/* Bibliothèque interne fournissant des outils pour tout le projet */

/* Fait une conversion string vers int en faisant des vérifications */
int secure_atoi(char* str);

/* Sépare une chaine c en un ensemble de tokens split
 * de nombre de tokens maximum maxTokens
 * en les coupant au délimiteur delim.
 * Retourne le nombre de tokens. */
int strsplit(const char * c, char ** tokens, int maxTokens, char * delim);

#endif /* utils.h */
