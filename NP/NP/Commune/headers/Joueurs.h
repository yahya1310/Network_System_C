
#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "Client.h"
#include "Carte.h"
#define MAX_CARTES 10
typedef struct Joueur
{
    char nom[25];
    Carte main[MAX_CARTES];
    int score;
    Carte CarteChoisie;
    int IndiceCarteChoisie;
    char nomGroupe[1024];
    Client c;
    bool isInChat;
    bool hasEnteredChat;
}Joueur;

typedef struct ListeJoueur
{
    Joueur *listeJoueurs; 
    size_t size;
    int nbQuittersEquipeA;
    int nbQuittersEquipeB;
}ListeJoueur;

typedef struct Equipe
{
    ListeJoueur lj[2];
}Equipe;

//Affiche un message chez tous les joeurs
void AfficheMessage(const ListeJoueur joueurs , const char* format, ...);

//Affiche un message chez tous les joueur d'une équipe
void AfficheMessageEquipe(const Equipe equipe , const char* format, ...);

//Notifie tous les joueurs
void NotifieJoueurs(const ListeJoueur joueurs, const char* format, ...);

//Affiche les scores
void AfficheScores(const ListeJoueur joueurs);

//Notifie les scores
void NotifieScores(const ListeJoueur joueurs);

//Affiche la main de chaque joueur
void NotifieMains(const ListeJoueur joueurs);

//Affiche la main de chaque joueur dans le chat de son groupe
void NotifieMainsEquipe(const Equipe, char nom);

//Quand le joueur doit prendre toutes les cartes de la rangée
void PrendTouteLigne(Joueur* joueur, Ligne* ligne);

Joueur* JoueurCarteMin(const ListeJoueur joueurs);

int CheckEndGame(const ListeJoueur joueurs);