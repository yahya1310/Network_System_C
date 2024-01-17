#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "serveur.h"

 void clear_clients(ListeClients *lc, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      close(lc->listeClient[i].sock);
   }
}

 void remove_client(Client *clients, int to_remove, int *actual)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*actual)--;
}
 
 void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if(sender.sock != clients[i].sock)
      {
         if(from_server == 0)
         {
            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i].sock, message);
      }
   }
}

void send_message_to_team(Client *clients,Client sender,int actual,const char *buffer,char from_server){
   
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      //we don't send message to the sender and we send the message only the players who are in the same team
      if(sender.sock != clients[i].sock)
      {
        if (sender.idEquipe == clients[i].idEquipe){ 
         if(from_server == 0)
         {
            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         if(clients[i].idEquipe == sender.idEquipe)
         {
            write_client(clients[i].sock, message);
         }
         
      }
      }
   }
 }

 int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }
 

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }
   else
   {
    printf("serveur lié à l'adresse\n");
   }

   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }
   else
   {
    printf("\t\tà l'écoute des connexions....\n");
   }
   return sock;
}

 void end_connection(int sock)
{
   closesocket(sock);
}

 int read_client(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      //perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}

 void write_client(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

void app(void)
{
   printf("\t\tWelcome\n \t\tI am the server\n");

    int actual;

   do {
        printf("Veuillez entrer le nombre de joueurs (entre 1 et 10) : ");
        
        if (scanf("%d", &actual) != 1) {
            // Si la conversion échoue, le caractère non lu doit être vidé du tampon d'entrée.
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            
            printf("Nombre de joueurs invalide. Veuillez entrer un nombre entre 1 et 10.\n");
        } else if (actual < 1 || actual > 10) {
            printf("Nombre de joueurs invalide. Veuillez entrer un nombre entre 1 et 10.\n");
        }
    } while (actual < 1 || actual > 10);

    SOCKET sock = init_connection();
    char buffer[BUF_SIZE];
    int max = sock;
    fd_set rdfs;
    ListeClients lc;
    lc.size = actual;
    lc.listeClient = (Client*)malloc(actual * sizeof(Client));
    int i = 0;
    while (1) {
        FD_ZERO(&rdfs);
        FD_SET(sock, &rdfs);

        if (select(max + 1, &rdfs, NULL, NULL, NULL) == -1) {
            
            perror("select()");
            exit(errno);
        }
        if (FD_ISSET(sock, &rdfs)) {
            SOCKADDR_IN csin = {0};
            size_t sinsize = sizeof csin;
            int csock = accept(sock, (SOCKADDR*)&csin, &sinsize);

            if (csock == SOCKET_ERROR) {
                perror("accept()");
                continue;
            }
            printf("socket client %d accepté\n",i+1); 
             /* what is the new maximum fd ? */
            max = csock > max ? csock : max;
            Client c = {csock};
            lc.listeClient[i] = c;
            i++;
        }
        if(actual == i)
        {
        
        printf("Le jeu va commencé\n");
        startGame(&lc,&rdfs); 
        //clear_clients(&lc, actual);
        //end_connection(sock);
    }
    }
   
 
         
}
int main()
{
   app();
   
   return 0;
}