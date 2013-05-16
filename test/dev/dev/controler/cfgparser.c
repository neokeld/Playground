#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <arpa/inet.h>

#include "cfgparser.h"

#include "utils.h"

#define VALUE_SIZE 16
#define NAME_SIZE 32

#define VARIABLE 0
#define VALUE 1 

/* Options dont on veut récupérer la valeur */
struct options{
    int controller_port; /* Numéro de port d'écoute */
    int poll_timeout_value; /* Temps entre 2 requêtes max */
};
struct options opt;

/* 1 s'il faut sauvegarder les caractères lus, 0 sinon */
int save = 1;
/* VARIABLE si la donnée lue est une variable, VALUE si c'est une valeur */
int data = VARIABLE;

/* Fonction d'affichage des options lues */
void cfgparser_data_print()
{
    printf("controller-port : %d\n",opt.controller_port);
    printf("poll-timeout-value : %d\n",opt.poll_timeout_value);
}

int get_controller_port()
{
    return opt.controller_port;
}

int get_poll_timeout_value()
{
    return opt.poll_timeout_value;
}

int cfgparser_parse(char *fileName)
{
    /* Buffer de nom de variable */
    char data_name[NAME_SIZE];
    int data_name_it = 0;
    /* Buffer de valeur */
    char data_value[VALUE_SIZE];
    int data_value_it = 0;
    /* Caractère en train d'être lu */
    char c;
    /* Compteur de caractères */
    int len;
    /* Fichier de configuration */
    FILE *file;

    memset(data_name, 0, sizeof(data_name));
    memset(data_value, 0, sizeof(data_value));

    /* Ouvrir le fichier de configuration */
    file = fopen(fileName, "r");
    if (file == NULL)
    {
	printf("Erreur : Fichier non trouvé\n");
	return EXIT_FAILURE;
    }
    /* S'il existe */
    else {
	c = 0;
	len = 0;
	/* Lire le fichier */
	while((c = fgetc(file)) != EOF)
	{
	    if (c == '#')
		save = 0; /* On arrête de sauvegarder */
	    else
	    {
		if (save == 1)
		{
		    if(c != ' ')
		    {
			len++; /* On a lu 1 caractère non espace de plus */
			if(c == '=')
			{
			    data = VALUE; /* On s'apprête à lire une valeur */
			}
			else if(c == '\n')
			{
			    if(len != 1)
			    {
				/* Terminer les chaînes de caractères */
				data_name[data_name_it] = '\0';
				data_value[data_value_it] = '\0';
				/* Remplir la variable avec la valeur lue */
				if(strcmp(data_name, "controller-port") == 0)
				{
				    if((opt.controller_port = secure_atoi(data_value))==-1)
				    {
					fclose(file);
					return EXIT_FAILURE;
				    }
				    if (opt.controller_port > 65535)
				    {
					printf("Erreur : port du routeur trop grand\n");
					fclose(file);
					return EXIT_FAILURE;
				    }
				}
				else if(strcmp(data_name, "poll-timeout-value") == 0)
				{
				    if((opt.poll_timeout_value = secure_atoi(data_value))==-1)
				    {
					fclose(file);
					return EXIT_FAILURE;
				    }
				}
				else
				{
				    printf("dn: %s\n",data_name);
				    printf("Erreur : Fichier invalide, une option est inconnue\n");
				    fclose(file);
				    return EXIT_FAILURE;
				}
			    }
			    /* Effacer les données stockées */
			    memset(data_name, 0, sizeof(data_name));
			    memset(data_value, 0, sizeof(data_value));
			    data_name_it = 0;
			    data_value_it = 0;
			    /* Remettre le compte de caractères par ligne à 0 */
			    len = 0;
			    /* On s'apprête à sauvegarder une variable */
			    data = VARIABLE;
			}
			else
			{
			    if(data == VARIABLE)
			    {
				data_name[data_name_it++] = c;
			    }
			    else if(data == VALUE)
			    {
				data_value[data_value_it++] = c;
			    }
			}
		    }
		}
		else
		{
		    if (c == '\n')
		    {
			save = 1; /* On recommence à sauvegarder */
			data = VARIABLE;
		    }
		    else if(c == '#')
		    {
			printf("Erreur : Fichier invalide, près du caractère #\n");
			fclose(file);
			return EXIT_FAILURE;
		    }
		}
	    }
	}
    }
    fclose(file);
    return EXIT_SUCCESS;
}

