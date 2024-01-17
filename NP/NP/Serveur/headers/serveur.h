#include "inclusions.h"
#include "Client.h"

 void app(void);
 int init_connection(void);
 void end_connection(int sock);
 int read_client(SOCKET sock, char *buffer);
 void write_client(SOCKET sock, const char *buffer);
 void send_message_to_all_clients(Client *clients, Client client, int actual, const char *buffer, char from_server);
 void remove_client(Client *clients, int to_remove, int *actual);
 void clear_clients(ListeClients *lc, int actual);
 void send_message_to_team(Client *clients,Client sender,int actual,const char *buffer,char from_server);
 