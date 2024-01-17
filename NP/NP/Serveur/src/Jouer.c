#include "Jouer.h"
#include "SendRcv.h"
#include <ctype.h>

//#define BROADCAST(...) AfficheMessage(joueurs, __VA_ARGS__)
//#define NOTIFYALL(...) NotifieJoueurs(joueurs, __VA_ARGS__)
// Variables statiques (= limitées à cette unité de traduction)
static ListeJoueur joueurs;
static  Paquet p = { 0 };
static  Ligne Lignes[NOMBRE_LIGNE] = { 0 };
static size_t Nombre_Tour = 0, Nombre_Manche = 0;
static int nbA;
static int nbB;
void DemanderNom(ListeJoueur lj)
{
    char* phrase = "Veuillez entrez votre nom";
    for (int i = 0; i < lj.size; i++)
    {
        char buffer[1024];

        write_client(lj.listeJoueurs[i].c.sock,phrase);
        read_client(lj.listeJoueurs[i].c.sock, buffer);
        strcpy(lj.listeJoueurs[i].c.name, buffer);
        strcpy(lj.listeJoueurs[i].nom, buffer);
         // Initialiser isInChat à true lorsque vous créez un nouveau joueur
        lj.listeJoueurs[i].isInChat = true;
        lj.listeJoueurs[i].hasEnteredChat = false;
        printf("JOUEUR numero %d , %s\n",i, lj.listeJoueurs[i].c.name);
    }
}

void DemanderEquipe(ListeJoueur lj)
{
    char* phrase = "Veuillez choisir un groupe : [A,B]";
    for (int i = 0; i < lj.size; i++)
    {
        char buffer2[1024];
        int validChoice = 0;

        while (!validChoice) {
            write_client(lj.listeJoueurs[i].c.sock, phrase);
            //printf("Sent: %s\n", phrase);

            int bytesRead = read_client(lj.listeJoueurs[i].c.sock, buffer2);
            if (bytesRead <= 0)
            {
                // Handle client disconnection or error
                printf("Client disconnected or error while reading group.\n");
                // Optionally remove the player from the list or take appropriate action.
                break;  // Exit the loop if there's a disconnection or error
            }

            //printf("Received group choice: %s\n", buffer2);

            // Check if the choice is valid ('A' or 'B')
            if (strlen(buffer2) == 1 && (buffer2[0] == 'A' || buffer2[0] == 'B'))
            {
                printf("JOUEUR a choisi le groupe: %c\n", buffer2[0]);
                strcpy(lj.listeJoueurs[i].nomGroupe,buffer2);
                if(buffer2[0]== 'A')
                {
                    lj.listeJoueurs[i].c.idEquipe = 1;
                    //printf("id équipe : %d\n",lj.listeJoueurs[i].c.idEquipe);
                    nbA++;
                }
                else
                {
                   
                    lj.listeJoueurs[i].c.idEquipe = 2;
                    //printf("id équipe : %d\n",lj.listeJoueurs[i].c.idEquipe);
                    nbB++;
                }
                validChoice = 1;
            }
            else
            {
                char* msgerror  = "Choix invalide. veuillez choisir entre 'A' et 'B'.\n";
                write_client(lj.listeJoueurs[i].c.sock, msgerror);
                // Optionally, you can send a message to the client to inform them of the invalid choice
            }
        }
    }
}


void distruCartes(ListeJoueur lj,Paquet *p)
{
   for(int i=0 ; i<lj.size;i++)
   {
    printf("CARTE DU JOUEUR : %s \n",lj.listeJoueurs[i].nom);
    for(int j=0;j<10;j++)
    {
    lj.listeJoueurs[i].main[j] = Tire_Carte(p);
     printf("  %d [ \033[%um%3u  |  %-1u\033[0m ]    \n", j,lj.listeJoueurs[i].main[j].Couleur, lj.listeJoueurs[i].main[j].numero,lj.listeJoueurs[i].main[j].tete_boeuf);
    }
   }
}


void envoyerMainAuClient(ListeJoueur lj,int j) {
    // Construire une chaîne de caractères représentant la main
    char message[1024];
    message[0] = '\0';
    char carteStr[200];
    

     // En-tête
    sprintf(carteStr, "\n%-8s%-8s%-8s\n", "Indice", "Numero", "Boeuf");
    strncat(message, carteStr, sizeof(message) - strlen(message) - 1);

    for (int i = 0; i < 10; i++) {
        if (lj.listeJoueurs[j].main[i].numero == 0 && lj.listeJoueurs[j].main[i].tete_boeuf == 0) {
            // Carte jouée, ajoutez le texte approprié
            sprintf(carteStr, "  %2d [\033[%umCarteJouée\033[0m]    \n", i + 1, lj.listeJoueurs[j].main[i].Couleur);
        } else {
            sprintf(carteStr, "  %2d [ \033[%um%3u  |  %-1u\033[0m ] \n", i + 1, lj.listeJoueurs[j].main[i].Couleur, lj.listeJoueurs[j].main[i].numero, lj.listeJoueurs[j].main[i].tete_boeuf);
        }

        strncat(message, carteStr, sizeof(message) - strlen(message) - 1);
    }

    // Envoyer la main au client
    write_client(lj.listeJoueurs[j].c.sock, message);
    memset(carteStr, 0, sizeof(carteStr));
    memset(message,0,sizeof(message));
}

void envoyerTableJeu(Ligne *rows,ListeJoueur lj,int j)
{
     // Construire une chaîne de caractères représentant la table de jeu
    char message[1024];
    message[0] = '\0';
    char carteStr[200];
    
    sprintf(carteStr,"\n\n\t\t\t\t#############  TABLE DU JEU  ############# \n\n");
    strncat(message,carteStr,sizeof(message) - strlen(message) - 1);
    for (size_t i = 0; i < 4; i++) // On regarde dans les 4 colonnes
    {
        sprintf(carteStr, "\t\t\t\t[ ");
        strncat(message, carteStr, sizeof(message) - strlen(message) - 1);

        for (size_t k = 0; k < rows[i].taille; k++) // On a max 5 cartes par colonne (donc 5 lignes max)
        {
            sprintf(carteStr, "\033[%um%3u  |  %-1u\033[0m",rows[i].Cartes[k].Couleur, rows[i].Cartes[k].numero, rows[i].Cartes[k].tete_boeuf);
            strncat(message, carteStr, sizeof(message) - strlen(message) - 1);

            if (k < rows[i].taille - 1)
            {
                sprintf(carteStr, " , ");
                strncat(message, carteStr, sizeof(message) - strlen(message) - 1);
            }
        }

        sprintf(carteStr, " ] \n");
        strncat(message, carteStr, sizeof(message) - strlen(message) - 1);

        if (i < 3)
        {
            sprintf(carteStr, "\t\n");
            strncat(message, carteStr, sizeof(message) - strlen(message) - 1);
        }
    }
   

    // Envoyer la table de jeu au client
    write_client(lj.listeJoueurs[j].c.sock, message);


}
//FONCTION POUR LA COMMUNICATION ENTRE LES MEMBRES DE L'ÉQUIPE
int a = 1;
int count=0;


void Chat(ListeJoueur lj,fd_set *rdfs)
{ 
    a=1;
    while (a) {
        usleep(1000); // Pause de 1000 milliseconde
        for (int i = 0; i < lj.size; i++) {
            sleep(1);
            if (!lj.listeJoueurs[i].hasEnteredChat && lj.size>1 && lj.listeJoueurs[i].c.idEquipe==1) {
                write_client(lj.listeJoueurs[i].c.sock, "\n\t\t#############  BIENVENUE AU CHAT A ############# \n");
                lj.listeJoueurs[i].hasEnteredChat = true;  // Marquer comme entré dans le chat
            }else if(!lj.listeJoueurs[i].hasEnteredChat && lj.size>1 && lj.listeJoueurs[i].c.idEquipe==2)
            {
                 write_client(lj.listeJoueurs[i].c.sock, "\n\t\t#############  BIENVENUE AU CHAT B ############# \n");
                lj.listeJoueurs[i].hasEnteredChat = true;  // Marquer comme entré dans le chat
            }
            char buffer[1024];
            fcntl(lj.listeJoueurs[i].c.sock, F_SETFL, SOCK_NONBLOCK);
            if(FD_ISSET(lj.listeJoueurs[i].c.sock,rdfs) && lj.listeJoueurs[i].isInChat)
            {
               //printf("avant read_C\n");
                // On utilise recv avec l'option MSG_DONTWAIT pour rendre la lecture non bloquante
               int n = read_client(lj.listeJoueurs[i].c.sock,buffer);
               //printf("apres read_C\n");
               if(strlen(buffer)>0){
                printf("JOUEUR %s a dit : %s\n", lj.listeJoueurs[i].nom,buffer);
                
                 // Vérifier si le joueur veut quitter le chat
                    if (strcmp(buffer, "exitChat") == 0)
                    {
                        ExitChat(&lj, i);
                        //a=0;
                        memset(buffer, 0, sizeof(buffer));
                    }
                    else if (strcmp(buffer, "sendHand") == 0)
                    {
                        // Envoyer la main du joueur aux autres membres du groupe
                        EnvoyerMainAuGroupe(lj, i);
                        memset(buffer, 0, sizeof(buffer));
                    }
                    else
                    {
                    if(lj.listeJoueurs[i].c.idEquipe == 1)
                    {

                       printf(" idEquipe est %d: ",lj.listeJoueurs[i].c.idEquipe );
                       send_message_to_Equipe(lj,lj.listeJoueurs[i].c,buffer);
                       memset(buffer, 0, sizeof(buffer));
                    }
                    else{
                         
                         send_message_to_Equipe(lj,lj.listeJoueurs[i].c,buffer);
                         memset(buffer, 0, sizeof(buffer));
                        }
                    }
               }
               if(lj.listeJoueurs[i].isInChat==false && count==nbA)
            {
                a=0;

            }
            }            
            
        }
        printf("ok ok hh \n");
   }
   count = 0;
}

void update_fd_set(ListeJoueur lj, fd_set *rdfs) {
    FD_ZERO(rdfs);
    for (int i = 0; i < lj.size; i++) {
        FD_SET(lj.listeJoueurs[i].c.sock, rdfs);
    }
}
int openChat = 0;
    int nbOpenChatA = 0;
    int nbOpenChatB = 0;
    int avoidP = 0;

void AfficheInfosJoueurs(ListeJoueur lj, fd_set* rdfs)
{
    printf("\t\tScores et informations des joueurs :\n");
     
    char message[1024];
    for(int i = 0; i < lj.size; i++)
    {
        char msg[200];
        // Utilisez sprintf pour formater le message
        sprintf(msg, "\t\tJoueur %.50s (Groupe %.50s, Score %d)\n", lj.listeJoueurs[i].nom, lj.listeJoueurs[i].nomGroupe, lj.listeJoueurs[i].score);
        strncat(message,msg,sizeof(message) - strlen(message) - 1);
        // Utilisez strncat si vous souhaitez ajouter d'autres informations au message

        // Imprimez le message côté serveur (peut être supprimé si non nécessaire)
        printf("%s", msg);
    }
    for(int i=0;i<lj.size;i++)
    {

        // Utilisez write_client pour envoyer le message au joueur
        write_client(lj.listeJoueurs[i].c.sock, message);
    }

    printf("\n");
}
void Tour(ListeJoueur lj, fd_set *rdfs)
{
    
    Nombre_Tour++;
    
    AfficheInfosJoueurs(lj,rdfs);
    char* phrase = "'open' pour ouvrir le chat 'skip' pour ignorer\n";
    for (int i = 0; i < lj.size; i++)
    {
        envoyerTableJeu(Lignes,lj,i);
        usleep(20);
        envoyerMainAuClient(lj,i);
    }
    for (int i = 0; i < lj.size; i++)
    {

        char buffer[1024];
        
        while (!openChat && (nbA>1 || nbB>1))
        {
            write_client(lj.listeJoueurs[i].c.sock, phrase);
            read_client(lj.listeJoueurs[i].c.sock, buffer);

            if (strcmp(buffer, "open") == 0 || strcmp(buffer, "skip") == 0)
            {
                // La décision est correcte, on peut sortir de la boucle
                openChat = 1;

                // Si le joueur choisit 'openChat', incrémente le nombre d'ouvertures du chat
                if (strcmp(buffer, "open") == 0)
                {
                    lj.listeJoueurs[i].isInChat = true;
                    if(lj.listeJoueurs[i].c.idEquipe==1)nbOpenChatA++;
                    else nbOpenChatB++;
                }
                else if(strcmp(buffer , "skip") == 0)
                {
                    avoidP = 1;
                }

                
            }
        }
        openChat=0;
    }
    //printf("%d",nbA);
    //printf("%d",nbB);
    
    if((nbOpenChatA == nbA || nbOpenChatB == nbB) && avoidP == 0)
    {
        usleep(20);
        printf("CHAT HH\n");
        Chat(lj,rdfs);
        for(size_t i =0;i<lj.size;i++)
        {
            lj.listeJoueurs[i].hasEnteredChat = false;
        } 
    }
    nbOpenChatB = 0;
    nbOpenChatA = 0;
    avoidP = 0;
     //update_fd_set(lj,rdfs);
     DemanderCarte(lj);
     //update_fd_set(lj,rdfs);
     AjouteCarteTable(lj);
}

int Manche(fd_set *rdfs)
{
    Nombre_Manche++;
    Nombre_Tour = 0;
    Construction_Paquet(&p);
    distruCartes(joueurs,&p);
    // On remet les colonnes à zéro et on place une première carte dans chacune.
    for(size_t i = 0 ; i < NOMBRE_LIGNE ; i++)
    {
        Construction_Ligne(Lignes + i);
        Pose_Carte_Ligne(Lignes + i, Tire_Carte(&p));
    }

    for(int i=0 ;i<10;i++)
    {
        //update_fd_set(joueurs,rdfs);
        Tour(joueurs,rdfs);
        //update_fd_set(joueurs,rdfs);
    }
    printf("FIN MANCHE NUMERO : %ld \n",Nombre_Manche);
    Efface_Ligne(Lignes);
    for(int i=0;i<joueurs.size;i++){
    for(int j=0;j<10;j++)
        {
            joueurs.listeJoueurs[i].main[j].numero=0;
            joueurs.listeJoueurs[i].main[j].tete_boeuf=0;
        }
    }
    if(CheckEndGame(joueurs)) return FIN;
    else return CONTINUE;
}
void *envoyerMessage(void *data) {
    ThreadData *threadData = (ThreadData *)data;
    threadData->lj = joueurs;
    for (int i = 0; i < threadData->lj.size; i++) {
        if (threadData->lj.listeJoueurs[i].c.idEquipe == threadData->lj.listeJoueurs[threadData->j].c.idEquipe &&
            threadData->lj.listeJoueurs[i].c.sock != threadData->lj.listeJoueurs[threadData->j].c.sock) {
            // Envoyer le message à ce client spécifique
            write_client(threadData->lj.listeJoueurs[i].c.sock, threadData->message);
        }
    }

    pthread_exit(NULL);
}



void ExitChat(ListeJoueur *lj, int playerIndex)
{
    //a = 0;
    count ++;
    printf("JOUEUR %s a quitté le chat\n", lj->listeJoueurs[playerIndex].nom);
     lj->listeJoueurs[playerIndex].isInChat = false;

    // Informer les autres joueurs que ce joueur a quitté le chat
    char exitMessage[BUF_SIZE];
    snprintf(exitMessage, BUF_SIZE, "JOUEUR %s a quitté le chat\n", lj->listeJoueurs[playerIndex].nom);
    
    for (int j = 0; j < lj->size; j++)
    {
        if (lj->listeJoueurs[j].c.idEquipe == lj->listeJoueurs[playerIndex].c.idEquipe &&
            lj->listeJoueurs[j].c.sock != lj->listeJoueurs[playerIndex].c.sock)
        {
            fcntl(lj->listeJoueurs[j].c.sock, F_SETFL, SOCK_NONBLOCK);
            write_client(lj->listeJoueurs[j].c.sock, exitMessage);
        }
    }
}

void send_message_to_Equipe(ListeJoueur lj, Client sender, char *buffer)
{
    int i = 0;
    char message[BUF_SIZE];
    message[0] = 0;
    int une=1;
   
    printf("Sender: %s, Buffer: %s\n", sender.name, buffer);
    for (i = 0; i < lj.size; i++)
    {
        /* Ne pas envoyer de message à l'expéditeur */
        if (sender.sock != lj.listeJoueurs[i].c.sock && sender.idEquipe == lj.listeJoueurs[i].c.idEquipe && lj.listeJoueurs[i].isInChat)
        {
            printf("Sending to %s\n", lj.listeJoueurs[i].c.name);

            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);

            strncat(message, buffer, sizeof message - strlen(message) - 1);
            fcntl(lj.listeJoueurs[i].c.sock, F_SETFL, SOCK_NONBLOCK);
            write_client(lj.listeJoueurs[i].c.sock, message);
        }
    }
}

void EnvoyerMainAuGroupe(ListeJoueur lj, int joueurActuel)
{
    // Construire une chaîne de caractères représentant la main
    char message[1024];
    message[0] = '\0';
    char carteStr[200];
    sprintf(carteStr, "\n\t\tMain de %s :\n", lj.listeJoueurs[joueurActuel].nom);
    strncat(message, carteStr, sizeof(message) - strlen(message) - 1);
    snprintf(carteStr,sizeof(carteStr), "\t \t \n %s\n", "\t\tIndice  Numero  Boeuf");
    strncat(message, carteStr, sizeof(message) - strlen(message) - 1);
    for (int i = 0; i < 10; i++)
    {
    
        sprintf(carteStr, "\t\t  %d   [ \033[%um%3u  |  %-1u\033[0m ]    \n", i, lj.listeJoueurs[joueurActuel].main[i].Couleur, lj.listeJoueurs[joueurActuel].main[i].numero, lj.listeJoueurs[joueurActuel].main[i].tete_boeuf);
        strncat(message, carteStr, sizeof(message) - strlen(message) - 1);
    }

    // Envoyer la main aux autres membres du groupe
    for (int i = 0; i < lj.size; i++)
    {
        if (lj.listeJoueurs[i].c.idEquipe == lj.listeJoueurs[joueurActuel].c.idEquipe && i != joueurActuel)
        {
            write_client(lj.listeJoueurs[i].c.sock, message);
        }
    }
}

void DemanderCarte(ListeJoueur lj)
{
   
   
    char buffer[2];
    for(size_t i=0;i<lj.size;i++)
    {
        if(lj.listeJoueurs[i].isInChat == false)
        {
             int flags = fcntl(lj.listeJoueurs[i].c.sock, F_GETFL, 0);
             flags &= ~O_NONBLOCK;  // Supprimer le drapeau O_NONBLOCK
fcntl(lj.listeJoueurs[i].c.sock, F_SETFL, flags);

        
        Joueur *j = &lj.listeJoueurs[i];
        do
        {   
            
            write_client(lj.listeJoueurs[i].c.sock,"Selectionnez la carte que vous souhaitez jouer : \n");
            sleep(1);
            read_client(lj.listeJoueurs[i].c.sock, buffer);
            if(strlen(buffer)==0) break;
            if (strlen(buffer) > 0 && !isspace(buffer[0]))
            {
                j->IndiceCarteChoisie = atoi(buffer);

                // Check the range and validity of the chosen card
            }
            memset(buffer, 0, sizeof(buffer));


        }while(!(j->IndiceCarteChoisie >= 1 && j->IndiceCarteChoisie <= 10 && lj.listeJoueurs[i].main[j->IndiceCarteChoisie - 1].numero > 0));
        j->IndiceCarteChoisie --;
        j->CarteChoisie = j->main[j->IndiceCarteChoisie];
        j->main[j->IndiceCarteChoisie].numero = 0;//On retire la carte (num = 0)
        j->main[j->IndiceCarteChoisie].tete_boeuf = 0;//On retir la carte (boeuf = 0)
        envoyerMainAuClient(lj,i);
    }
    }
}

void AjouteCarteTable(ListeJoueur lj)
{
    for(size_t i = 0 ; i < lj.size ; i++)
    {
        // On récupère le joueur ayant la carte de plus faible valeur.
        // Cette valeur ne devrait jamais être NULL si le nombre d'itérations est correct.
        Joueur* J = JoueurCarteMin(lj);
        printf("Carte choisie par le joueur %s : %d\n",J->nom, J->CarteChoisie.numero);

        for (size_t i = 0; i < NOMBRE_LIGNE; i++)
{
    printf("Lignes[%zu]: taille=%d, carte=%d\n", i, Lignes[i].taille, Lignes[i].Cartes->numero);
}
        // On récupère la colonne la plus proche de la carte du joueur mais de valeur inférieure à la carte du joueur.
        Ligne* closestRow = getBonneLigne(Lignes, J->CarteChoisie);

        if(closestRow != NULL) // Si on a trouvé une colonne où le joueur peur poser sa carte
        {
            if(closestRow->taille < 5) // Si la colonne n'est pas complète
                Pose_Carte_Ligne(closestRow, J->CarteChoisie);
            else
            {
                PrendTouteLigne(J, closestRow);
                Pose_Carte_Ligne(closestRow, J->CarteChoisie);
            }
        }
        else // Aucune colonne ne peut accueillir la carte du joueur
        {
            // On doit demander au jouer quelle colonne il souhaite récupérer pour lui.

            int LigneChoisi = 0;
            char buff[2];
            do
            {
                
                write_client(J->c.sock, "Veuillez choisir une ligne a recuperer { 1 | 2 | 3 | 4 } : ");
                read_client(J->c.sock,buff);
                LigneChoisi = atoi(buff);
            } while (!(LigneChoisi >= 1 && LigneChoisi <= 4)); // Pour l'utilisateur, le choix est à faire entre 1 et 4.
            
            // Pour la machine, les colonnes sont numérotées de 0 à 3.
            LigneChoisi--;

            PrendTouteLigne(J, Lignes + LigneChoisi);
            Pose_Carte_Ligne(Lignes + LigneChoisi, J->CarteChoisie);
        }

        J->CarteChoisie.numero = 0;
        J->CarteChoisie.tete_boeuf = 0;
    }

}

void startGame(ListeClients *lc,fd_set *rdfs)
{
    
    //Joueurs
    printf("Création des joueurs en cours...\n");
    joueurs.listeJoueurs = (Joueur*)malloc(lc->size * sizeof(Joueur));
    joueurs.size = lc->size;
    for(int i=0; i <lc->size;i++)
    {
        joueurs.listeJoueurs[i].c = lc->listeClient[i];
        joueurs.listeJoueurs[i].CarteChoisie.numero=0; //Aucune carte choisie
        joueurs.listeJoueurs[i].IndiceCarteChoisie = 0;
        joueurs.listeJoueurs[i].score = 66; 
        FD_SET(joueurs.listeJoueurs[i].c.sock, rdfs);
    }

    
    //On demande au joueurs leur nom.
    DemanderNom(joueurs);
    DemanderEquipe(joueurs);
    sleep(1);
    printf("Construction de la table du jeu\n");
    printf("Distrubition de carte\n");
    printf("Maintenant on va communiquer avec les joueurs\n");
    while(Manche(rdfs) == FIN);//Boucle de jeu
    printf("GAME OVER\n");
    //Affiche_Paquet(&p);
    //distruCartes(joueurs,&p);
    //CommunicationEquipe(joueurs,rdfs);
    //Chat(joueurs,rdfs); //avant que j'ajoute la fonction Tour
    //Tour(joueurs,rdfs);
}