#include "Client.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <unistd.h>

#include <errno.h>
#include "SendRcv.h"

void ClientPrintf(const Client* c, const char* format, ...)
{
    va_list params;
    va_start(params, format);

    vClientPrintf(c, format, params);
    
    va_end(params);
}

void ClientPrint(const Client* c, const char* text)
{
    Message msg;
        msg.action = MessageAction_Print;
        msg.type = MessageType_String;
        strcpy(msg.content, text);
        msg.contentSize = strlen(msg.content) + 1;
    
    SendMessage(c->sock, &msg);
}


void vClientPrintf(const Client* c, const char* format, va_list args)
{
    Message msg;
        msg.action = MessageAction_Print;
        msg.type = MessageType_String;
        vsprintf(msg.content, format, args);
        msg.contentSize = strlen(msg.content) + 1;
    
    SendMessage(c->sock, &msg);
}

void ClientNotifyf(const Client* c, const char* format, ...)
{
    va_list params;
    va_start(params, format);

    vClientNotifyf(c, format, params);
    
    va_end(params);
}

void vClientNotifyf(const Client* c, const char* format, va_list args)
{
    Message msg;
        msg.action = MessageAction_Notify;
        msg.type = MessageType_String;
        vsprintf(msg.content, format, args);
        msg.contentSize = strlen(msg.content) + 1;
    
    SendMessage(c->sock, &msg);
}

void ClientScanf(const Client* c, const char* format, ...)
{
    Message msg;
        msg.action = MessageAction_Ask;
        msg.type = MessageType_String;
        msg.contentSize = 0;
    
    SendMessage(c->sock, &msg);

    memset(&msg, 0, sizeof(Message));
    ReceiveMessage(c->sock, &msg);

    va_list params;
    va_start(params, format);

    vsscanf(msg.content, format, params);

    va_end(params);
}