#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "graph.h"

#define MAX_BUF 512

/* Initialiser un graphe : on renvoie un pointeur vers la zone allouée */

struct graph* initialiser_Graph (void) {

    struct graph* graph = malloc(sizeof(struct graph));
    if(graph == NULL){
	perror("malloc");
	exit(-1);
    }else{
	graph->nombre_Noeud = 0;
	graph->liste_noeud = malloc(sizeof(struct noeud *));

	return graph;
    }
}

/* Initialiser un noeud : on passe le label comme chaine de caractère, 
   on renvoie un pointeur vers la zone allouée*/

struct noeud* initialiser_Noeud (char* label) {

    struct noeud* new_Noeud = malloc(sizeof(struct noeud));

    new_Noeud->label = strdup(label);
    new_Noeud->id = 0;
    new_Noeud->nombre_Voisin = 0;
    new_Noeud->cout = 0;
    new_Noeud->voisin = malloc(sizeof(struct noeud *));
    new_Noeud->activated = 0; /* Noeud désactivé par défaut */

    return new_Noeud;
}


/* Ajoute un noeud dans le graphe s'il n'y est pas déjà présent */

void ajouter_Noeud(struct graph* graph, char* label){

    struct noeud* node = initialiser_Noeud(label);  
    /* Le cas où le graphe est vide */
    if ( graph->nombre_Noeud == 0 ){
	graph->liste_noeud[0] = node;
	graph->liste_noeud[0]->id = 1;
	graph->nombre_Noeud++;
    }else{
	if (exist_Noeud (graph, label)>=0 ){
	    fprintf(stderr, "Erreur : noeud déjà existant ! \n");
	}else{
	    /*On ajoute le nouveau noeud à la fin du tableau */
	    graph->liste_noeud = realloc(graph->liste_noeud,
		    (graph->nombre_Noeud+1)*sizeof(struct noeud *));

	    graph->nombre_Noeud ++;
	    graph->liste_noeud[graph->nombre_Noeud-1] = node;
	    graph->liste_noeud[graph->nombre_Noeud-1]->id = graph->nombre_Noeud;
	}
    }
}

/* On vérifie si le noeud est présent dans la liste */
int exist_Noeud ( struct graph* graph, const char* label ) {
    int i ;
    for (i = 0 ; i< graph->nombre_Noeud ; i ++ ) {
	if (strcmp (graph->liste_noeud[i]->label, label) == 0 )
	    return i;
    }
    return -1;
}

char * obtenir_Nom_Noeud( struct graph* graph, int i ) {
    return graph->liste_noeud[i]->label;
}

/* Retourne 1 si le noeud est actif, 0 sinon */
int est_actif_noeud( struct graph* graph, int i ) {
    return graph->liste_noeud[i]->activated;
}

/* On active un noeud présent dans la liste */
void activer_Noeud ( struct graph* graph, int indice) {
    graph->liste_noeud[indice]->activated = 1;
}

/* On désactive un noeud présent dans la liste */
void desactiver_Noeud ( struct graph* graph, int indice) {
    graph->liste_noeud[indice]->activated = 0;
}

/* Obtient l'indice d'un noeud non actif
 * S'il n'y en a pas retourne -1 */
int obtenir_Indice_Noeud_Non_Actif ( struct graph* graph) {
    int i = 0;
    while (i < graph->nombre_Noeud) {
	if(graph->liste_noeud[i]->activated == 0)
	    return i;
	else
	    i++;
    }
    return -1;
}

/*  Créer une copie du noeud passé en paramètre, en copiant que les attributs et mettant les pointeurs à NULL
    cela sert quand on voudra ajouter un noeud successeur
    */
struct noeud* copie_Noeud(struct noeud* noeud){

    struct noeud* copie = malloc(sizeof(struct noeud));
    copie->label = strdup(noeud->label);
    strcpy(copie->label, noeud->label);
    copie->cout = noeud->cout ;
    copie->nombre_Voisin = noeud->nombre_Voisin ;
    copie->id = noeud->id;
    copie->voisin = NULL ;
    copie->activated = noeud->activated;

    return copie ;
}

/* Vérifie si le noeud passé en paramètre a un successeur de nom label */
int exist_Successeur(struct noeud* noeud, char* label){

    int i ; 
    for (i = 0 ; i < noeud->nombre_Voisin ; i ++)
	if ( strcmp (noeud->voisin[i]->label, label) == 0 )
	    return i;
    return -1;
}

/* Ajouter un lien entre le noeud label2 et le noeud label2 avec le coût*/
void ajouter_Lien (struct graph* graph, char* label1, char* label2, int cout){

    /*On vérifie si les deux noeuds existent*/
    int i1 = exist_Noeud (graph, label1);
    int i2 = exist_Noeud (graph, label2);

    /*Si au moins un est introuvable, il y a erreur */
    if ( i1 == -1 || i2 == -1 ){
	fprintf(stderr, "Erreur : l'un des noeud est introuvable !\n");
    }else if ((exist_Successeur(graph->liste_noeud[i1], label2) >=0)){
	return;
    }else{

	/* On crée des copies des deux noeuds*/
	struct noeud* copie_1 = copie_Noeud(graph->liste_noeud[i1]) ;
	struct noeud* copie_2 = copie_Noeud(graph->liste_noeud[i2]) ;

	int nb_vois_1 = graph->liste_noeud[i1]->nombre_Voisin ;
	int nb_vois_2 = graph->liste_noeud[i2]->nombre_Voisin ;

	/*Le cas où l'un des deux n'as pas de successeurs, on ajoute le successeur au début du tableau*/
	if ( (nb_vois_1 == 0) || (nb_vois_2 == 0) ){
	    if (nb_vois_1 == 0){
		/*Ajout du noued 2 comme successeur du noeud 1*/
		graph->liste_noeud[i1]->voisin[0] = copie_2 ;
		graph->liste_noeud[i1]->voisin[0]->cout = cout ;
		graph->liste_noeud[i1]->nombre_Voisin ++;

		graph->liste_noeud[i2]->voisin = realloc(graph->liste_noeud[i2]->voisin,
			(1 + graph->liste_noeud[i2]->nombre_Voisin)*sizeof(struct noeud*));
		/* Ajout du noeud 1 comme successeur du noeud 2*/
		graph->liste_noeud[i2]->voisin[nb_vois_2] = copie_1;
		graph->liste_noeud[i2]->voisin[nb_vois_2]->cout = cout;
		graph->liste_noeud[i2]->nombre_Voisin ++;

	    }else{/*Ajout du noeud 1 comme successeur du noeud 2*/
		graph->liste_noeud[i2]->voisin[0] = copie_1 ;
		graph->liste_noeud[i2]->voisin[0]->cout = cout ;
		graph->liste_noeud[i2]->nombre_Voisin ++;

		graph->liste_noeud[i1]->voisin = realloc(graph->liste_noeud[i1]->voisin,
			(1 + graph->liste_noeud[i1]->nombre_Voisin)*sizeof(struct noeud*));
		/*Ajout du noeud 2 comme successeur du noeud 1*/
		graph->liste_noeud[i1]->voisin[nb_vois_1] = copie_2 ;
		graph->liste_noeud[i1]->voisin[nb_vois_1]->cout = cout;
		graph->liste_noeud[i1]->nombre_Voisin ++;
	    }
	}else{ /*Sinon, on réalloue de la mémoire et on ajoute le noeud successeur*/
	    graph->liste_noeud[i1]->voisin = realloc(graph->liste_noeud[i1]->voisin,
		    (1 + graph->liste_noeud[i1]->nombre_Voisin)*sizeof(struct noeud*));
	    /*Ajout du noeud 2 comme successeur de noeud 1*/
	    graph->liste_noeud[i1]->voisin[nb_vois_1] = copie_2 ;
	    graph->liste_noeud[i1]->voisin[nb_vois_1]->cout = cout;
	    graph->liste_noeud[i1]->nombre_Voisin ++;

	    graph->liste_noeud[i2]->voisin = realloc(graph->liste_noeud[i2]->voisin,
		    (1 + graph->liste_noeud[i2]->nombre_Voisin)*sizeof(struct noeud*));
	    /* Ajout du noud 1 comme successeur du noeud 1*/
	    graph->liste_noeud[i2]->voisin[nb_vois_2] = copie_1;
	    graph->liste_noeud[i2]->voisin[nb_vois_2]->cout = cout;
	    graph->liste_noeud[i2]->nombre_Voisin ++;

	}
    }
}

/* Supprimer le lien entre les noeuds de noms label1 et label2 */
void supprimer_Lien(struct graph* graph, char* label1, const char* label2){

    /* On vérifie si les noeuds existent */
    int i1 = exist_Noeud(graph, label1);
    int i2 = exist_Noeud(graph, label2);

    /* Si les noeuds existent */
    if ( (i1 >= 0) && (i2 >= 0) ){
	int i;
	/* On cherche dans le noeud 1 le successeur du nom label2 */
	for (i=0; i<graph->liste_noeud[i1]->nombre_Voisin; i++){

	    if (strcmp(graph->liste_noeud[i1]->voisin[i]->label, label2) == 0){

		/* Si le successeur est le premier élément du tableau, et que le nombre de voisin est 1, il suffit de libérer la case mémoire et decrémenter le nombre de voisins */
		if ( (i == 0) && (graph->liste_noeud[i1]->nombre_Voisin == 1) ){

		    free (graph->liste_noeud[i1]->voisin[i]->label);
		    free (graph->liste_noeud[i1]->voisin[i]);
		    graph->liste_noeud[i1]->nombre_Voisin --;

		}
		/* Sinon si le successeur est le premier dans la liste et que le nombre de successeur est supérieur à 1, il faudra libérer la première case mémoire, et recopier les autres attribut dans les cases d'indices inférieurs */
		else if ((i==0) && (graph->liste_noeud[i1]->nombre_Voisin > 1)){

		    int j;
		    free( graph->liste_noeud[i1]->voisin[0]->label);
		    free( graph->liste_noeud[i1]->voisin[0]);
		    for ( j = 1 ; j < graph->liste_noeud[i1]->nombre_Voisin ; j++ ){
			graph->liste_noeud[i1]->voisin[j-1] = graph->liste_noeud[i1]->voisin[j];
		    }
		    /* Libérer la dernière case mémoire car elle sera doublé lors de la recopie */
		    graph->liste_noeud[i1]->voisin[j-1] = NULL ;
		    graph->liste_noeud[i1]->nombre_Voisin -- ;

		}
		/* Le cas où le successeur est le dernier élément du tableau */
		else if ((i == (graph->liste_noeud[i1]->nombre_Voisin-1))
			&& (graph->liste_noeud[i1]->nombre_Voisin > 1)){
		    free (graph->liste_noeud[i1]->voisin[i]->label);
		    free (graph->liste_noeud[i1]->voisin[i]);
		    graph->liste_noeud[i1]->voisin[i] = NULL ;
		    graph->liste_noeud[i1]->nombre_Voisin --;

		}
		/* Le cas où le successeur est au milieu du tableau, on le supprime et on recopie les éléments qui lui sont supérieur*/
		else if ((i > 0) && (i < graph->liste_noeud[i1]->nombre_Voisin)){

		    int j;
		    free(graph->liste_noeud[i1]->voisin[i]->label);
		    free(graph->liste_noeud[i1]->voisin[i]);
		    for ( j = i+1 ; j < graph->liste_noeud[i1]->nombre_Voisin ; j ++ ){
			graph->liste_noeud[i1]->voisin[j-1] = graph->liste_noeud[i1]->voisin[j];
		    }
		    /* On désalloue le dernier élément du tableau car i*/
		    graph->liste_noeud[i1]->voisin[j-1] = NULL;
		    graph->liste_noeud[i1]->nombre_Voisin --;
		}  
	    }
	}
	/*On fait le même traitement pour le noeud 2 */

	for (i=0; i<graph->liste_noeud[i2]->nombre_Voisin ; i++){

	    if (strcmp(graph->liste_noeud[i2]->voisin[i]->label, label1) == 0){

		if ( (i == 0) && (graph->liste_noeud[i2]->nombre_Voisin == 1) ){
		    free (graph->liste_noeud[i2]->voisin[i]->label);
		    free (graph->liste_noeud[i2]->voisin[i]);
		    graph->liste_noeud[i2]->nombre_Voisin --;

		}else if ((i==0) && (graph->liste_noeud[i2]->nombre_Voisin > 1)){

		    int j ;
		    free( graph->liste_noeud[i2]->voisin[0]->label);
		    free( graph->liste_noeud[i2]->voisin[0]);
		    for ( j = 1 ; j < graph->liste_noeud[i2]->nombre_Voisin ; j++ ){
			graph->liste_noeud[i2]->voisin[j-1] = graph->liste_noeud[i2]->voisin[j];
		    }
		    graph->liste_noeud[i2]->voisin[j-1] = NULL ;
		    graph->liste_noeud[i2]->nombre_Voisin -- ;

		}else if ((i == (graph->liste_noeud[i2]->nombre_Voisin-1))
			&& (graph->liste_noeud[i2]->nombre_Voisin > 1)){
		    free (graph->liste_noeud[i2]->voisin[i]->label);
		    free (graph->liste_noeud[i2]->voisin[i]);
		    graph->liste_noeud[i2]->voisin[i] = NULL ;
		    graph->liste_noeud[i2]->nombre_Voisin --;

		}else if ((i > 0) && (i < graph->liste_noeud[i2]->nombre_Voisin)){

		    int j;
		    free(graph->liste_noeud[i2]->voisin[i]->label);
		    free(graph->liste_noeud[i2]->voisin[i]);
		    for ( j = i+1 ; j < graph->liste_noeud[i2]->nombre_Voisin ; j ++ ){
			graph->liste_noeud[i2]->voisin[j-1] = graph->liste_noeud[i2]->voisin[j];
		    }
		    graph->liste_noeud[i2]->voisin[j-1] = NULL ;
		    graph->liste_noeud[i2]->nombre_Voisin --;
		}  
	    }
	}
    }
}

/* Deconnecter un routeur, il suffit d'appeler la fonction supprimer_Lien pour tous les noeuds successeur de ce routeur, les liens seront supprimés de manière symétrique */

void deconnecter_Routeur(struct graph* graph, char* label){
    int i = exist_Noeud(graph, label);

    if (i >= 0){
	int j = graph->liste_noeud[i]->nombre_Voisin ; 
	while (j>0 ){
	    supprimer_Lien(graph, label, graph->liste_noeud[i]->voisin[0]->label);
	    j--;
	}
    }else{
	fprintf(stderr,"Erreur : Noeud introuvable !\n");
    }
}

/* Modifier le coût entre les noeuds du nom label1, label2 */

void modifier_cout(struct graph* graph, char* label1, char* label2, int cout){

    int i1 = exist_Noeud(graph, label1);
    int i2 = exist_Noeud(graph, label2);

    if ( (i1>=0) && (i2>=0) ){
	int j1 = exist_Successeur(graph->liste_noeud[i1], label2);
	int j2 = exist_Successeur(graph->liste_noeud[i2], label1);
	if ( (j1>=0) && (j2 >=0) ){
	    graph->liste_noeud[i1]->voisin[j2]->cout = cout;
	    graph->liste_noeud[i2]->voisin[j1]->cout = cout;
	}else{
	    fprintf(stderr, "Erreur : noeud introuvable ! impossible de mettre à jour le cout !\n");
	}
    }
}

/* Fonction auxiliaire, permet de chercher la présence d'un élément dans un tableau*/
int existe_dans_tab(int* tab,int size,  int indice){
    int i;
    for ( i=0 ; i < size ; i ++)
	if (*(tab+i) == indice )
	    return i;
    return -1;
}

/* Fonction auxiliaire, permet de mettre l'entier -1 dans la case d'indice passée en paramètre */
void ajouter_indice(int* tab,int size,int indice){
    int i = existe_dans_tab(tab, size, indice);
    if (i == -1){
	int j ;
	for (j = 0; j < size ; j++)
	    if (*(tab+j) == -1){
		*(tab+j) = indice;
		return;
	    }
    }
}

/* Afficher la topology sur la sortie standard */
void show_Topology(struct graph* graph){

    int i; 
    int j = 0;
    int* tmp = malloc(graph->nombre_Noeud*sizeof(int));
    for (i = 0 ; i < graph->nombre_Noeud ; i++)
	*(tmp+i) = -1;

    for ( i = 0 ; i < graph->nombre_Noeud ; i++ ){
	printf("n%d [label=\"%s\"];\n", i+1, graph->liste_noeud[i]->label);
    }

    for (i = 0 ; i < graph->nombre_Noeud ; i++)
	for ( j = 0 ; j < graph->liste_noeud[i]->nombre_Voisin ; j ++){
	    if ( existe_dans_tab(tmp, graph->nombre_Noeud, 
			graph->liste_noeud[i]->voisin[j]->id) == -1){
		printf("n%d -- n%d [label=\"%d\"];\n", 
			graph->liste_noeud[i]->id,graph->liste_noeud[i]->voisin[j]->id,
			graph->liste_noeud[i]->voisin[j]->cout);
		ajouter_indice(tmp, graph->nombre_Noeud, graph->liste_noeud[i]->id);
	    }
	}
    free(tmp);
}

/* Sauvegarder la topology dans un fichier que l'on passe le nom en paramètre*/
void sauvegarder_Topology( struct graph* graph, char* fichier ){

    /* Si le fichier n'existe pas, on le crée */
    FILE *f = fopen(fichier, "w");
    if (!f){
	fprintf(stderr, "Erreur ouverture du fichier\n");
    }else{
	int i; 
	int j = 0;
	int* tmp = malloc((graph->nombre_Noeud)*sizeof(int));
	fprintf(f, "graph G {\n");
	for (i = 0 ; i < graph->nombre_Noeud ; i++)
	    *(tmp+i) = -1;
	for ( i = 0 ; i < graph->nombre_Noeud ; i++ ){
	    fprintf(f, "  n%d [label=\"%s\"];\n", i+1, graph->liste_noeud[i]->label);
	}

	for (i = 0 ; i < graph->nombre_Noeud ; i++)
	    for ( j = 0 ; j < graph->liste_noeud[i]->nombre_Voisin ; j ++){
		if ( existe_dans_tab(tmp, graph->nombre_Noeud, 
			    graph->liste_noeud[i]->voisin[j]->id) == -1){
		    fprintf(f, "  n%d -- n%d [label=\"%d\"];\n", 
			    graph->liste_noeud[i]->id,graph->liste_noeud[i]->voisin[j]->id,
			    graph->liste_noeud[i]->voisin[j]->cout);
		    ajouter_indice(tmp, graph->nombre_Noeud, graph->liste_noeud[i]->id);
		}
	    }
	fprintf(f, "}\n");
	free(tmp);

    }
    fclose(f);

}
/* Libérer le graphe */
void free_graph(struct graph* graph){

    int i, j;
    if(graph->nombre_Noeud > 0)
    {
	for ( i = graph->nombre_Noeud-1 ; i >=0 ; i-- ){
	    for ( j = 0 ; j < graph->liste_noeud[i]->nombre_Voisin ; j++ ){
		free(graph->liste_noeud[i]->voisin[j]->label);
		free(graph->liste_noeud[i]->voisin[j]);
	    }
	    free(graph->liste_noeud[i]->voisin);
	    free(graph->liste_noeud[i]->label);
	    free(graph->liste_noeud[i]);
	}
    }
    free(graph->liste_noeud);
    free(graph);
}

/*Fonction auxiliaire, permet d'afficher sur la sortie standard, la liste des successeurs du noeud label*/
void afficher_Successeurs(struct graph* graph, char* label){

    int i = exist_Noeud(graph, label);
    if ( i >= 0 ){
	int j ;
	printf("Successeurs de %s (Nb = %d ): ", label, graph->liste_noeud[i]->nombre_Voisin);
	for ( j = 0 ; j < graph->liste_noeud[i]->nombre_Voisin ; j++ )
	    printf(" (%s, %d) ", graph->liste_noeud[i]->voisin[j]->label,graph->liste_noeud[i]->voisin[j]->cout);
	printf("\n");
    }else{
	fprintf(stderr, "Erreur: noeud introuvable !\n");
    }
}

