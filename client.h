#ifndef PISKVORKY_CLIENT_H
#define PISKVORKY_CLIENT_H
#endif //PISKVORKY_CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct timeval timeout = {
        .tv_sec = 5
};

void vypisHry(int sockfd);