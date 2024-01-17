#ifndef CLIENT_H
#define CLIENT_H

#include "inclusions.h"
#include <stdarg.h>

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   int idEquipe;
}Client;

typedef struct ListeClients
{
    Client *listeClient;
    int size;
    int capa;
}ListeClients;

//Méthodes pour gérer l'envoie et la réception des messages

void ClientPrint(const Client* c, const char* text);
void ClientPrintf(const Client* c, const char* format, ...);
void vClientPrintf(const Client* c, const char* format, va_list args);

void ClientNotifyf(const Client* c, const char* format, ...);
void vClientNotifyf(const Client* c, const char* format, va_list args);

void ClientScanf(const Client* c, const char* format, ...);
#endif 