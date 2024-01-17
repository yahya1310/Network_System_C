#ifndef CARTE_H
#define CARTE_H
#include <stdlib.h>

#define NOMBRE_CARTE 104
#define NOMBRE_LIGNE 4
#include <unistd.h>
#include <string.h>

typedef struct Carte
{
    int numero;
    int  tete_boeuf;
    int Couleur;

}Carte;


typedef struct Paquet
{
    Carte Cartes[NOMBRE_CARTE];
    int curseur;
}Paquet;

typedef struct Ligne
{
    Carte Cartes[6];
    int taille;
}Ligne;

typedef struct ListeJoueur ListeJoueur; // "Forward declaration"

/*Fonctions relative au paquet*/

void Construction_Paquet(Paquet *paquet);

void Melange_Paquet(Paquet *paquet);

void Affiche_Paquet(Paquet *paquet);

Carte Tire_Carte(Paquet *paquet);

/*Fonctions relatives aux rangées*/

void Construction_Ligne(Ligne *ligne);

void Pose_Carte_Ligne(Ligne *ligne,Carte carte);

void Efface_Ligne(Ligne * ligne);

Ligne* getBonneLigne(Ligne* lignes, Carte carte);


// Affiche toutes les lignes
void AffichesLignes(Ligne* lignes, const ListeJoueur* players);
// Notifie toutes les lignes
void NotifyLignes(Ligne* rows, const ListeJoueur* players);

#endif