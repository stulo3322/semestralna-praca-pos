
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

typedef struct {
    pthread_mutex_t * mutex;
    pthread_cond_t * hraPrebieha;
    pthread_cond_t * mozeSaHrat;
    int hrac;
    int vitaz;
    int riadok;
    int stlpec;
    int tah;
    int sockfd;
    mainData* mainData;
} dataHra;



void* vlaknoHry(void*args) {
    dataHra * dat = (dataHra *)args;
    pthread_mutex_lock(dat->mutex);
    int opacne = 0;
    int znova = 0;
    int n = 0;

    do {
        /*
        dat->vitaz = 0;
        char hraciaPlocha[3][3] = {{'1', '2', '3'},
                                   {'4', '5', '6'},
                                   {'7', '8', '9'}};
        for (int i = 0; i < 9 && dat->vitaz == 0; i++) {
            printf("zaciatok for\n");
            printf("\n\n");
            for (int o = 0; o < 3; o++) {
                printf(" %c | %c | %c\n", hraciaPlocha[o][0], hraciaPlocha[o][1], hraciaPlocha[o][2]);
                printf("---+---+---\n");
            }

            if (i == 0) {
                float nahodne =  rand() % 101;
                dat->hrac = nahodne < 50 ? 1 : 0;
                printf("hodnota rand pre hraca %f\n",nahodne);
                if (dat->hrac == 1) {
                    opacne = 1;
                    dat->tah = 5;
                }
                sleep(1);
                send(dat->sockfd, &dat->tah, sizeof(dat->tah), 0);
                printf("odoslanie opacne cez tah\n");
                printf("hodnota tahu %d\n",dat->tah);

            }
            if (opacne == 1) {
                if(i%2==0) {
                    dat->hrac =  2;
                } else {
                    dat->hrac =  1;
                }
                printf("hrame opacne\n");
            } else {
                if(i%2==0) {
                    dat->hrac =  1;
                } else {
                    dat->hrac =  2;
                }
                printf("hrame normalne\n");
            }

            int jebetomu = 0;
            do {
                jebetomu++;
                if (dat->hrac == 1) {
                    printf("Zadaj cislo stvorca: \n");
                    scanf("%d", &dat->tah);
                    send(dat->sockfd, &dat->tah, sizeof(dat->tah), 0);

                } else {
                    printf("Cakaj kym protihrac vyberie policko.\n");
                    n = recv(dat->sockfd, &dat->tah, 200, 0);
                    if (n < 1) {
                        printf("Nepodarilo sa ziskat tah");
                    }
                    printf("Protihrac vybral policko %d\n", dat->tah);
                }

                dat->riadok = --dat->tah / 3;
                dat->stlpec = dat->tah % 3;
                dat->tah++;

            } while ((dat->tah < 1 || dat->tah > 9 || hraciaPlocha[dat->riadok][dat->stlpec] > '9') && jebetomu < 5);

            hraciaPlocha[dat->riadok][dat->stlpec] = (dat->hrac == (opacne == 1 ? 2:1)) ? 'X' : 'O';

            if ((hraciaPlocha[0][0] == hraciaPlocha[1][1] && hraciaPlocha[0][0] == hraciaPlocha[2][2]) ||
                (hraciaPlocha[0][2] == hraciaPlocha[1][1] && hraciaPlocha[0][2] == hraciaPlocha[2][0])) {
                dat->vitaz = dat->hrac;
            } else {
                for (int j = 0; j <= 2; j++) {
                    if ((hraciaPlocha[j][0] == hraciaPlocha[j][1] && hraciaPlocha[j][0] == hraciaPlocha[j][2]) ||
                        (hraciaPlocha[0][j] == hraciaPlocha[1][j] && hraciaPlocha[0][j] == hraciaPlocha[2][j])) {
                        dat->vitaz = dat->hrac;
                    }
                }
            }
            printf("skontrolovanie vitaza\n");
            sleep(1);
            send(dat->sockfd, &dat->vitaz, sizeof(dat->vitaz), 0);
            printf("Odoslal som vitaza %d\n",dat->vitaz);
        }

        printf("\n\n");
        for (int o = 0; o < 3; o++) {
            printf(" %c | %c | %c\n", hraciaPlocha[o][0], hraciaPlocha[o][1], hraciaPlocha[o][2]);
            printf("---+---+---\n");
        }

        if(dat->vitaz == 0) {
            printf("Remiza\n");
        } else if (dat->vitaz == 1) {
            printf("Vyhral si\n");
        } else {
            printf("Prehral si\n");
        }
        */
        printf("Chces hrat znova? (1 - ano, 2 - nie)\n");
        scanf("%d", &znova);
        if(znova == 1) {
            send(dat->sockfd, &znova, sizeof(znova), 0);
            printf("Cakanie na potvrdenie od protihraca\n");
            if(recv(dat->sockfd, &znova, 200, 0) == 0) {
                znova = 2;
                printf("Protihrac zamietol ponuku o hranie znova\n");
            }
        } else {
            send(dat->sockfd, &znova, sizeof(znova), 0);
        }
        printf("Koniec DO WHILE\n");
    } while(znova == 1);
    printf("po DO WHILE\n");

    dat->mainData->pocetVytvorenychHier--;
    pthread_cond_broadcast(dat->mozeSaHrat);
    pthread_mutex_unlock(dat->mutex);
}

void* vlaknoKlient(void* args) {
    dataHra * dat = (dataHra *)args;
    if(dat->mainData->pocetVytvorenychHier > 1) {
        pthread_cond_wait(dat->mozeSaHrat,dat->mutex);
        printf("moze sa hrat\n");
    }
    pthread_t hra;
    if (pthread_create(&hra,NULL,&vlaknoHry,&dat) != 0) {
        perror("Nepodarilo sa vytvorit vlakno\n");
        return 1;
    } else {
        dat->mainData->pocetVytvorenychHier++;
        printf("pocetVytvorenychHier %d\n",dat->mainData->pocetVytvorenychHier++);
        pthread_join(hra,NULL);
        printf("vlakno\n");
    }
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2)
    {
        fprintf(stderr,"usage %s port\n", argv[0]);
        return 1;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 1;
    }

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error binding socket address");
        return 2;
    }

    listen(sockfd, 5);

    mainData mainData1 = {0};

    pthread_cond_t hraPrebieha;
    pthread_cond_t mozeSaHrat;
    pthread_mutex_t mutex;

    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&hraPrebieha,NULL);
    pthread_cond_init(&mozeSaHrat,NULL);


    int i = 0;
    while(1) {
        cli_len = sizeof(cli_addr);
        printf("Cakam na pripojenie protihraca... \n");
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len);
        printf("pripojenie\n");
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            return 3;
        }

        dataHra data = {&mutex,&hraPrebieha,&mozeSaHrat,0,
                        0,0,0,0,newsockfd,&mainData1};
        printf("data\n");

        pthread_t klient;
        if (pthread_create(&klient,NULL,&vlaknoKlient,&data) != 0) {
            perror("Nepodarilo sa vytvorit vlakno\n");
            return 1;
        } else {
            mainData1.pocetVytvorenychHier++;
            printf("pocetVytvorenychHier %d\n",mainData1.pocetVytvorenychHier);
            pthread_join(klient,NULL);
            printf("vlakno\n");
        }
    }

    close(newsockfd);
    close(sockfd);

    return 0;
}
