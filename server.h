#ifndef PISKVORKY_SERVER_H
#define PISKVORKY_SERVER_H

#endif //PISKVORKY_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef struct {
    int pocetVytvorenychHier;
} mainData;

struct timeval timeout = {
        .tv_sec = 5
};

typedef struct {
    pthread_mutex_t * mutex;
    pthread_cond_t * mozeSaHrat;
    int hrac;
    int vitaz;
    int riadok;
    int stlpec;
    int tah;
    int sockfd;
    mainData* mainData;
} dataHra;

void* vlaknoHry(void*args);