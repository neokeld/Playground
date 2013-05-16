#ifndef __GRAPH_H__
#define __GRAPH_H__


struct graph {

  int nombre_Noeud;
  struct noeud** liste_noeud ;
  
};

struct noeud {
  
  char* label;
  int id;
  int nombre_Voisin;
  int cout;
  struct noeud** voisin ;
  int activated; /* 1 si actif, 0 sinon */
};


/* Liste des fonctions */

/* void charger_Topology ( FILE*, struct graph*) ; */

struct noeud* initialiser_Noeud (char*) ;

struct graph* initialiser_Graph(void);

void ajouter_Noeud (struct graph*, char*);

void ajouter_Lien(struct graph *, char*, char*, int);

int exist_Noeud (struct graph*, const char*);

char * obtenir_Nom_Noeud (struct graph*, int);

int est_actif_noeud( struct graph*, int);

void activer_Noeud (struct graph*, int);

void desactiver_Noeud (struct graph*, int);

int obtenir_Indice_Noeud_Non_Actif( struct graph*);

struct noeud* copie_Noeud(struct noeud*);

void supprimer_Lien(struct graph*, char*, const char*);

void afficher_Successeurs(struct graph*, char*);

int exist_Successeur(struct noeud*, char*);

void deconnecter_Routeur (struct graph*, char* );

void modifier_cout (struct graph*, char*, char*, int );

void show_Topology(struct graph*);

int existe_dans_tab(int*, int, int);

void ajouter_indice(int*, int, int);

void sauvegarder_Topology (struct graph*,  char*);

void free_graph(struct graph*);

#endif /* __GRAPH_H */
