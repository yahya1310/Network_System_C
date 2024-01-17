#include "Joueurs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void AfficheMessage(const ListeJoueur joueurs , const char* format, ...)
{
    for(size_t i = 0 ; i < joueurs.size ; i++)
    {
        va_list params;
        va_start(params, format);

        vClientPrintf(&joueurs.listeJoueurs[i].c, format, params);

        va_end(params);
    }
}

void AfficheMessageEquipe(const Equipe equipe , const char* format, ...)
{
    
}

void NotifieJoueurs(const ListeJoueur joueurs, const char* format, ...)
{
    for(size_t i = 0 ; i < joueurs.size ; i++)
    {
        va_list params;
        va_start(params, format);

        vClientNotifyf(&joueurs.listeJoueurs[i].c, format, params);

        va_end(params);
    }

}

void AfficheScores(const ListeJoueur joueurs)
{
    AfficheMessage(joueurs,"Total des scores :\n");

    for(size_t i=0 ; i<joueurs.size;i++)
    {
        AfficheMessage(joueurs , "\n\t%s : %u points.",joueurs.listeJoueurs[i].nom,joueurs.listeJoueurs[i].score);
    }
    AfficheMessage(joueurs, "\n");

}

void NotifieScores(const ListeJoueur joueurs)
{
    char buffer[1024];

    strcat(buffer, "SCORES=[");

    for(size_t i = 0 ; i < joueurs.size ; i++)
    {
        sprintf(buffer + strlen(buffer), "%hhu", joueurs.listeJoueurs[i].score);

        if(i < joueurs.size - 1)
            strcat(buffer, ",");
    }

    strcat(buffer, "]");

    NotifieJoueurs(joueurs, buffer);

}

void NotifieMains(const ListeJoueur joueurs)
{
    for(size_t i = 0 ; i < joueurs.size ; i++)
    {
        Joueur* joueur = &joueurs.listeJoueurs[i];

        ClientPrintf(&joueur->c, "Voici votre main :");

        for(size_t j = 0 ; j < 10 ; j++)
        {
            if(joueur->main[j].numero > 0)
            {
                ClientPrintf
                (
                    &joueur->c,
                    "\n\tCarte %-2lu - \033[%umValeur %+3u | %u vaches\033[0m",
                    j + 1, joueur->main[j].Couleur, joueur->main[j].numero, joueur->main[j].tete_boeuf
                );
            }
            else
            {
                ClientPrintf
                (
                    &joueur->c,
                    "\n\tCarte %-2lu - Carte jouee.",
                    j + 1
                );
            }
        }

        ClientPrint(&joueur->c, "\n");
    }

}

void NotifieMainsEquipe(const Equipe, char nom)
{

}

int CheckEndGame(const ListeJoueur joueurs)
{
    for(size_t i = 0 ; i < joueurs.size ; i++)
    {
        if(joueurs.listeJoueurs[i].score < 66) return 1;
    }

    return 0;
}

void PrendTouteLigne(Joueur* joueur, Ligne* ligne)
{
    for(size_t i = 0 ; i < ligne->taille ; i++)
    {
        joueur->score -= ligne->Cartes[i].tete_boeuf;
        ligne->Cartes[i].numero = 0; // On supprime la carte de la colonne
        ligne->Cartes[i].tete_boeuf = 0;
    }

    ligne->taille = 0; // La colonne est maintenant vide.

}

Joueur* JoueurCarteMin(const ListeJoueur joueurs)
{
    Joueur* out = NULL;
    
    for(size_t i = 0 ; i < joueurs.size ; i++)
    {
        Joueur* J = &joueurs.listeJoueurs[i];

        // Si le joueur courant n'a choisi aucune carte, alors on passe au joueur suivant.
        if(J->CarteChoisie.numero == 0) continue;

        // Sinon : si "out" n'a pas encore été défini,
        // ou alors, si "out" est défini mais sa carte choisie a une plus grande valeur que la carte choisie du joueur courant,
        // alors "out" devient le joueur courant.
        if(out == NULL || J->CarteChoisie.numero < out->CarteChoisie.numero)
            out = J;
    }

    return out;

}
