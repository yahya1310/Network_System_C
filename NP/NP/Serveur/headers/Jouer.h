#include "Joueurs.h"
#include "Carte.h"
#include "string.h"
#include "serveur.h"

void DemanderNom(ListeJoueur lj);
void DemanderEquipe(ListeJoueur lj);
void startGame(ListeClients *lc,fd_set *rdfs);
void distruCartes(ListeJoueur lj,Paquet *p);
int Manche(fd_set *rdfs);
void Tour(ListeJoueur lj, fd_set *rdfs);
void *envoyerMessage(void *data);
void CommunicationEquipe(ListeJoueur lj,fd_set *rdfs);
void send_message_to_Equipe(ListeJoueur lj,Client sender,char* buffer);
void ExitChat(ListeJoueur *lj, int playerIndex);
void envoyerMainAuClient(ListeJoueur lj,int j);
void EnvoyerMainAuGroupe(ListeJoueur lj, int joueurActuel);
typedef struct {
    ListeJoueur lj;
    char message[1024];
    int j;
} ThreadData;


void AjouteCarteTable(ListeJoueur lj);
void DemanderCarte(ListeJoueur lj);
void AfficheInfosJoueurs(ListeJoueur lj, fd_set* rdfs);