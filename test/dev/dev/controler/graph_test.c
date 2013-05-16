#include <stdio.h>
#include <stdlib.h>

#include "graph.h"

int main ()
{
  struct graph* graph =  initialiser_Graph();
  
  ajouter_Noeud(graph, "N1");
  ajouter_Noeud(graph, "N2");
  ajouter_Noeud(graph, "N3");
  ajouter_Noeud(graph, "N4");
  ajouter_Noeud(graph, "N5");
  ajouter_Noeud(graph, "N6");
 
  printf("-------- Ajout de liens --------\n");
  ajouter_Lien(graph, "N1", "N2", 3);
  ajouter_Lien(graph, "N1", "N3", 1);
  ajouter_Lien(graph, "N1", "N4", 6);
  ajouter_Lien(graph, "N1", "N5", 8);
  ajouter_Lien(graph, "N1", "N6", 3);
  ajouter_Lien(graph, "N2", "N6", 6);
  ajouter_Lien(graph, "N2", "N3", 8);
  ajouter_Lien(graph, "N3", "N4",12);
  ajouter_Lien(graph, "N3", "N6", 18);
  ajouter_Lien(graph, "N5", "N4", 11);
  ajouter_Lien(graph, "N5", "N3", 6);
  ajouter_Lien(graph, "N6", "N4", 19);
  ajouter_Lien(graph, "N6", "N5", 20);


  afficher_Successeurs(graph, "N1");
  afficher_Successeurs(graph, "N2");
  afficher_Successeurs(graph, "N3");
  afficher_Successeurs(graph, "N4");
  afficher_Successeurs(graph, "N5");
  afficher_Successeurs(graph, "N6");

  show_Topology(graph);


  printf("------------- Supression des liens (N1,N2) (N6, N1) (N5, N6) (N3, N1)----------------\n");
  supprimer_Lien(graph, "N1", "N2");
  supprimer_Lien(graph, "N6", "N1");
  supprimer_Lien(graph, "N5", "N6");
  supprimer_Lien(graph, "N3", "N1");

  afficher_Successeurs(graph, "N1");
  afficher_Successeurs(graph, "N2");
  afficher_Successeurs(graph, "N3");
  afficher_Successeurs(graph, "N4");
  afficher_Successeurs(graph, "N5");
  afficher_Successeurs(graph, "N6");

  printf("---------Ajout de liens (N1, N6) ---------\n");
  ajouter_Lien(graph, "N1", "N6", 4);

  afficher_Successeurs(graph, "N1");
  afficher_Successeurs(graph, "N2");
  afficher_Successeurs(graph, "N3");
  afficher_Successeurs(graph, "N4");
  afficher_Successeurs(graph, "N5");
  afficher_Successeurs(graph, "N6");

  printf("---------- Deconnecter le noeud 3 -------------\n");
  deconnecter_Routeur(graph, "N3");
  afficher_Successeurs(graph, "N1");
  afficher_Successeurs(graph, "N2");
  afficher_Successeurs(graph, "N3");
  afficher_Successeurs(graph, "N4");
  afficher_Successeurs(graph, "N5");
  afficher_Successeurs(graph, "N6");

  printf("----------- Modifier cout : N4,N5 : 7 ----------\n");

  modifier_cout(graph, "N4", "N5", 7);
  
  afficher_Successeurs(graph, "N1");
  afficher_Successeurs(graph, "N2");
  afficher_Successeurs(graph, "N3");
  afficher_Successeurs(graph, "N4");
  afficher_Successeurs(graph, "N5");
  afficher_Successeurs(graph, "N6");

  printf("----------- Show Topology ----------\n");
  show_Topology(graph);
  
  printf("----------- Sauvegarder Topology ----------\n");
  sauvegarder_Topology(graph, "Topo.txt");

  free_graph(graph);
  return 0;
}

