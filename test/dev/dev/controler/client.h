#ifndef CLIENT_H
#define CLIENT_H

#include <time.h>

#include "controler.h"

typedef struct
{
    SOCKET sock;
    char name[BUF_SIZE];
    char * neighbors_list;
    time_t last_time;
    int port_number;
    char * address;
}Client;

#endif /* fin de client.h */

