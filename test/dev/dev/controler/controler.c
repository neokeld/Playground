#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "controler.h"

#include "utils.h"
#include "client.h"
#include "commandParser.h"
#include "cfgparser.h"
#include "graph.h"

#define MAX_LEN_LINE 512
/* Longueur max d'une commande */
#define MAX_NB_TOKENS 5
/* Nb max de tokens par commande */

/* Fonction interne :
 * Lit une chaîne de caractère contenant une requête d'un routeur
 * et vérifie que c'est un poll*
 * et si c'est le cas lui répond avec les bonnes informations.
 * Retourne 1 si tout se passe bien, 0 si le router veut quitter. */
int router_poll(struct graph* graph, Client * router, const char *c, Client * clients);

/* Fonction interne : */
int neighbors_list(struct graph* graph, char * label, char * neighbors_list, Client * clients);

/* position dans le tableau de Clients */
int actual = 0;

static void app(void)
{
    SOCKET sock = init_connection();
    char buffer[BUF_SIZE];
    int max = sock;
    /* tableau des clients */
    Client clients[MAX_CLIENTS];

    fd_set rdfs;

    /* Topologie */
    struct graph * graph = initialiser_Graph();

    while(1)
    {
	int i = 0;
	FD_ZERO(&rdfs);

	/* ajouter STDIN_FILENO */
	FD_SET(STDIN_FILENO, &rdfs);

	/* ajouter le socket de connexion */
	FD_SET(sock, &rdfs);

	/* ajouter un socket pour chaque client */
	for(i = 0; i < actual; i++)
	{
	    FD_SET(clients[i].sock, &rdfs);
	}

	if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
	{
	    perror("select()");
	    exit(errno);
	}

	/* quelque chose qui rentre depuis le clavier */
	if(FD_ISSET(STDIN_FILENO, &rdfs))
	{
	    /* char message[BUF_SIZE]; */
	    fgets(buffer, BUF_SIZE - 1, stdin);
	    {
		char *p = NULL;
		p = strstr(buffer, "\n");
		if(p != NULL)
		{
		    *p = 0;
		}
		else
		{
		    /* fclean */
		    buffer[BUF_SIZE - 1] = 0;
		}
	    }
	    /* ===Pour écrire à tous les clients===
	       message[0] = 0;
	       strncpy(message, "serveur : ", 0);
	       strncat(message, buffer, 10);
	       for(i = 0; i < actual; i++)
	       {
	       write_client(clients[i].sock, message);
	       } */

	    /* On analyse ce que l'utilisateur tape et on agit en conséquence */
	    if(analyse_cmd(graph, buffer) == 0)
		break; /* L'utilisateur veut quitter */
	}
	else if(FD_ISSET(sock, &rdfs))
	{
	    /* nouveau client */
	    SOCKADDR_IN csin = { 0 };
	    socklen_t sinsize = sizeof csin;
	    int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
	    if(csock == SOCKET_ERROR)
	    {
		perror("accept()");
		continue;
	    }

	    /* après s'être connecté le client envoie son nom */
	    if(read_client(csock, buffer) == -1)
	    {
		/* déconnecté */
		continue;
	    }

	    /* quel est le nouveau fd maximum ? */
	    max = csock > max ? csock : max;

	    FD_SET(csock, &rdfs); /* On ajoute le socket à la liste d'écoute */

	    int nbTokens;
	    char **tokens = malloc(MAX_NB_TOKENS*sizeof(char*));
	    nbTokens = strsplit(buffer, tokens, MAX_LEN_LINE, " ");
	    printf("ENVOYE : %s, NB TOKENS : %d\n", buffer, nbTokens);
	    if (nbTokens == 6 && strcmp(tokens[0], "log") == 0 && strcmp(tokens[1], "in") == 0 && strcmp(tokens[2], "as") == 0 && strcmp(tokens[4], "port") == 0)
	    {
		printf("nbTokens == 6\n");
		int port;
		tokens[5][strlen(tokens[5])-1] = '\0';
		port = secure_atoi(tokens[5]);
		/* On cherche si un client utilise déjà le nom demandé
		 * si le nom existe
		 * et si le port est bon*/
		if (port <= 65535 && port >= 0) {
		    printf("port OK\n");
		    int indice_noeud, indice_noeud_gen;
		    Client c;
		    c.sock = csock;
		    c.port_number = port;
		    c.address = inet_ntoa(csin.sin_addr);
		    indice_noeud = exist_Noeud(graph, tokens[3]);
		    indice_noeud_gen = obtenir_Indice_Noeud_Non_Actif(graph);
		    if (indice_noeud < actual  && indice_noeud >= 0 && est_actif_noeud(graph, indice_noeud)== 0) {
			printf("indice noeud OK, %s\n", tokens[3]);
			strcpy(c.name, tokens[3]);
			activer_Noeud(graph, indice_noeud);
			clients[actual] = c;
			actual = actual + 1;
			char bufG[BUF_SIZE] = "";
			sprintf(bufG, "greeting %s*\n", c.name);
			write_client(c.sock, bufG, clients);
		    }
		    else if(indice_noeud_gen != -1) {
			printf("indice noeud gen OK\n");
			strcpy(c.name, obtenir_Nom_Noeud(graph, indice_noeud_gen));
			activer_Noeud(graph, indice_noeud_gen);
			clients[actual] = c;
			actual = actual + 1;
			char bufG[BUF_SIZE] = "";
			sprintf(bufG, "greeting %s*\n", c.name);
			printf("bufG : %s\n", bufG);
			write_client(c.sock, bufG, clients);
			printf("écrit\n");
		    }
		    else {
			printf("Tous les noeuds sont déjà occupés !\n");
		    }

		}
		else
		    printf("Erreur de port : %s\n", tokens[5]);
	    }
	    else if (nbTokens == 4 && strcmp(tokens[0], "log") == 0 && strcmp(tokens[1], "in") == 0 && strcmp(tokens[2], "port") == 0)
	    {
		int port;
		tokens[5][strlen(tokens[5])-1] = '\0';
		port = secure_atoi(tokens[5]);
		/* On cherche si un client utilise déjà le nom demandé
		 * si le nom existe
		 * et si le port est bon*/
		if (port <= 65535 && port >= 0) {
		    int indice_noeud_gen;
		    Client c;
		    c.sock = csock;
		    c.port_number = port;
		    c.address = inet_ntoa(csin.sin_addr);
		    indice_noeud_gen = obtenir_Indice_Noeud_Non_Actif(graph);
		    if(indice_noeud_gen != -1) {
			strcpy(c.name, obtenir_Nom_Noeud(graph, indice_noeud_gen));
			activer_Noeud(graph, indice_noeud_gen);
			clients[actual] = c;
			actual = actual + 1;
			char bufG[BUF_SIZE] = "";
			sprintf(bufG, "greeting %s*\n", c.name);
			/* printf("bufG : %s\n", bufG); */
			write_client(c.sock, bufG, clients);
			/* printf("écrit\n"); */
		    }
		    else {
			printf("Tous les noeuds sont déjà occupés !\n");
		    }
		}
		else
		    printf("Erreur de port : %s\n", tokens[5]);
	    }
	   
	    /*
	    for(i=0; i < nbTokens; i++) {
		free(tokens[i]);
	    }
	    free(tokens);
	    */
	}
	else
	{
	    int i;
	    for(i = 0; i < actual; i++)
	    {
		/* un client parle */
		if(FD_ISSET(clients[i].sock, &rdfs))
		{
		    Client client = clients[i];
		    int c = read_client(clients[i].sock, buffer);
		    /* client déconnecté */
		    if(c == 0)
		    {
			closesocket(clients[i].sock);
			remove_client(clients, i, &actual);
			strncpy(buffer, client.name, BUF_SIZE - 1);
			strncat(buffer, " déconnecté !", BUF_SIZE - strlen(buffer) - 1);
			send_message_to_all_clients(clients, client, actual, buffer, 1);
		    }
		    else
		    {
			/* On analyse la requête du client stockée dans buffer et on lui répond */
			printf("router_poll : %s\n", buffer);
			if(router_poll(graph, &clients[i], buffer, clients) == 0) {
			    /* Si le routeur veut être déconnecté */
			    closesocket(clients[i].sock);
			    remove_client(clients, i, &actual);
			}
			/* ====Pour envoyer un message à tous les routeurs====
			   send_message_to_all_clients(clients, client, actual, buffer, 0); */
		    }
		    break;
		}
	    }
	}
    }

    /* libération des données */
    free_graph(graph);
    clear_clients(clients, actual);
    end_connection(sock);
}

static void clear_clients(Client *clients, int actual)
{
    int i = 0;
    for(i = 0; i < actual; i++)
    {
	closesocket(clients[i].sock);
    }
}

static void remove_client(Client *clients, int to_remove, int *actual)
{
    /* on enlève le client du tableau */
    memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
    /* nombre de clients - 1 */
    (*actual)--;
}

static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer, char from_server)
{
    int i = 0;
    char message[BUF_SIZE];
    message[0] = 0;
    for(i = 0; i < actual; i++)
    {
	/* on n'envoie pas de message à l'envoyeur */
	if(sender.sock != clients[i].sock)
	{
	    if(from_server == 0)
	    {
		strncpy(message, sender.name, BUF_SIZE - 1);
		strncat(message, " : ", sizeof message - strlen(message) - 1);
	    }
	    strncat(message, buffer, sizeof message - strlen(message) - 1);
	    write_client(clients[i].sock, message, clients);
	}
    }
}

static int init_connection(void)
{
    /* On initialise les sockets */
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN sin = { 0 };

    if(sock == INVALID_SOCKET)
    {
	perror("socket()");
	exit(errno);
    }

    /* On lit le fichier de configuration du controleur */
    cfgparser_parse("test.cfg");

    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(get_controller_port());
    sin.sin_family = AF_INET;

    if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
    {
	perror("bind()");
	exit(errno);
    }

    if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
    {
	perror("listen()");
	exit(errno);
    }

    return sock;
}

static void end_connection(int sock)
{
    closesocket(sock);
}

static int read_client(SOCKET sock, char *buffer)
{
    int n = 0;

    if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
    {
	perror("recv()");
	/* si erreur de réception alors on déconnecte le client */
	n = 0;
    }

    buffer[n] = 0;

    return n;
}

static void write_client(SOCKET sock, const char *buffer, Client * clients)
{
    printf("buffer envoyé : %s\n", buffer);
    if(send(sock, buffer, strlen(buffer), 0) < 0)
    {
	perror("send()");
	exit(errno);
    }
    int i;
    for(i = 0; i < actual; i++)
    {
	if(sock == clients[i].sock)
	     printf("sock : %s\n", clients[i].name );
    } 
}

/* Fonction interne */
int router_poll(struct graph* graph, Client * router, const char *c, Client * clients)
{
    int i, nbTokens, returnValue;
    char **tokens = malloc(MAX_NB_TOKENS*sizeof(char*));
    nbTokens = strsplit(c, tokens, MAX_LEN_LINE, " ");
    returnValue = 1;
    printf("nbTokens : %d\n", nbTokens);
    printf("avant strcmp : %s\n", tokens[0]);
    printf("tokens[0][0] : %c\n", tokens[0][0]);
    if (nbTokens == 1 && tokens[0][0] == 'p' && tokens[0][1] == 'o')
    {
	printf("tokens[0] : %s", tokens[0]);
	router->last_time = time(NULL);
	if(!router->neighbors_list)
	{
	    /* Cas 0 : le routeur ne connaît pas ses voisins */
	    router->neighbors_list = malloc(MAX_LEN_LINE*sizeof(char));
	    neighbors_list(graph, router->name, router->neighbors_list, clients);

	    printf("write_client neighbors_list : %s\n", router->neighbors_list);
	    write_client(router->sock, router->neighbors_list, clients);
	    printf("après write_client\n");
	}
	else {
	    /* Sauvegarde de la liste de voisins */
	    char * oldlist = malloc((strlen(router->neighbors_list)+1)*sizeof(char));
	    strcpy(oldlist, router->neighbors_list);
	    /* MaJ de la liste */
	    neighbors_list(graph, router->name, router->neighbors_list, clients);
	    /* Cas 1 : le routeur connaît ses voisins (ils n'ont pas changé) */
	    if (strcmp(oldlist, router->neighbors_list) == 0)
		write_client(router->sock, "neighborhood ok*", clients);
	    else
	    {
		/* Cas 2 : il faut redonner au routeur ses voisins */
		write_client(router->sock, router->neighbors_list, clients);
	    }
	    free(oldlist);
	}
    }
    else if (nbTokens == 2 && strcmp(tokens[0], "log") == 0 && strcmp(tokens[1], "out*\n") == 0)
    {
	write_client(router->sock, "bye", clients);
	deconnecter_Routeur(graph, router->name);
	free(router->neighbors_list);
	free(router->address);
	returnValue = 0;
    } 
    else
	printf("Reçu une commande erronée d'un routeur : %s\n", c);

    for(i=0; i < nbTokens; i++)
	free(tokens[i]);
    free(tokens);
    return returnValue;
}

/* Obtenir la liste des voisins d'un routeur sous forme d'une string
 * a partir d'une topologie graph et du label d'un routeur.
 * Retourne 0 si le routeur n'existe pas, 1 sinon. */
int neighbors_list(struct graph* graph, char * label, char * neighbors_list, Client * clients)
{
    int i = exist_Noeud(graph, label);
    int premier = 1;
    if ( i >= 0 ){
	int j;
	char tmp[MAX_LEN_LINE];
	strcpy(neighbors_list, "neighborhood newlist [");
	for ( j = 0 ; j < graph->liste_noeud[i]->nombre_Voisin ; j++ ) {
	    if(est_actif_noeud(graph, graph->liste_noeud[i]->voisin[j]->id -1) == 1)
	    {
		int k = 0;
		while (k < actual)
		{
		    if(strcmp(graph->liste_noeud[i]->voisin[j]->label, clients[k].name) == 0) {
			if(premier != 1)
			    strcat(neighbors_list, "; ");
			sprintf(tmp, "%s,%d,%s:%d", graph->liste_noeud[i]->voisin[j]->label, graph->liste_noeud[i]->voisin[j]->cout, clients[k].address, clients[k].port_number);
			strcat(neighbors_list, tmp);
			premier = 0;
		    }
		    k++;
		}
		if (k > actual)
		    return 0;

	    }
	}
	strcat(neighbors_list, "]*\n");
	return 1;
    }else{
	fprintf(stderr, "Erreur: noeud introuvable !\n");
	return 0;
    }
}


int main(void)
{
    app();

    return EXIT_SUCCESS;
}

